/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/textconsole.h"
#include "common/util.h"

#if defined(USE_OPENGL) && !defined(AMIGAOS)

#include "graphics/opengl/framebuffer.h"
#include "graphics/opengl/context.h"
#include "graphics/opengl/functions.h"

#ifdef USE_GLES2
#define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
#define GL_DEPTH_COMPONENT24 GL_DEPTH_COMPONENT24_OES
#endif

namespace OpenGL {

static bool usePackedBuffer() {
	return OpenGLContext.packedDepthStencilSupported;
}
static bool useDepthComponent24() {
	return OpenGLContext.OESDepth24;
}

FrameBuffer::FrameBuffer(uint width, uint height) :
		Texture(width, height) {
	if (!OpenGLContext.framebufferObjectSupported) {
		error("FrameBuffer Objects are not supported by the current OpenGL context");
	}


	init();
}

FrameBuffer::FrameBuffer(GLuint texture_name, uint width, uint height, uint texture_width, uint texture_height) :
		Texture(texture_name, width, height, texture_width, texture_height) {
	init();
}

FrameBuffer::~FrameBuffer() {
	glDeleteRenderbuffers(2, _renderBuffers);
	glDeleteFramebuffers(1, &_frameBuffer);
}

void FrameBuffer::init() {
	glGenFramebuffers(1, &_frameBuffer);
	glGenRenderbuffers(2, _renderBuffers);

	glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

	if (usePackedBuffer()) {
		glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffers[0]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _texWidth, _texHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _renderBuffers[0]);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _renderBuffers[0]);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	} else {
		glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffers[0]);
		glRenderbufferStorage(GL_RENDERBUFFER, useDepthComponent24() ? GL_DEPTH_COMPONENT24 : GL_DEPTH_COMPONENT16, _texWidth, _texHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _renderBuffers[0]);

		glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffers[1]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, _texWidth, _texHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _renderBuffers[1]);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
	GLenum status=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		error("Framebuffer is not complete! status: %d", status);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::attach() {
	glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
	glViewport(0,0, _width, _height);

	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::detach() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#if !defined(USE_GLES2) && !defined(AMIGAOS)
MultiSampleFrameBuffer::MultiSampleFrameBuffer(uint width, uint height, int samples)
		: FrameBuffer(width,height) {
	if (!OpenGLContext.framebufferObjectMultisampleSupported) {
		error("The current OpenGL context does not support multisample framebuffer objects!");
	}

	if (samples > OpenGLContext.multisampleMaxSamples) {
		warning("Requested anti-aliasing with '%d' samples, but the current OpenGL context supports '%d' samples at most",
		        samples, OpenGLContext.multisampleMaxSamples);
	}

	_msSamples = MIN(samples, OpenGLContext.multisampleMaxSamples);

	init();
}

MultiSampleFrameBuffer::~MultiSampleFrameBuffer() {
	glDeleteRenderbuffers(1, &_msColorId);
	glDeleteRenderbuffers(1, &_msDepthId);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &_msFrameBufferId);
}

void MultiSampleFrameBuffer::init() {
	glGenFramebuffers(1, &_msFrameBufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, _msFrameBufferId);

	glGenRenderbuffers(1, &_msColorId);
	glBindRenderbuffer(GL_RENDERBUFFER, _msColorId);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, _msSamples, GL_RGBA8, getTexWidth(), getTexHeight());

	glGenRenderbuffers(1, &_msDepthId);
	glBindRenderbuffer(GL_RENDERBUFFER, _msDepthId);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, _msSamples, GL_DEPTH24_STENCIL8, getTexWidth(), getTexHeight());

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _msColorId);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _msDepthId);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _msDepthId);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		error("Framebuffer is not complete! status: %d", status);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MultiSampleFrameBuffer::attach() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, getFrameBufferName());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _msFrameBufferId);
	glViewport(0,0, getWidth(), getHeight());

	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MultiSampleFrameBuffer::detach() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _msFrameBufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getFrameBufferName());
	glBlitFramebuffer(0, 0, getWidth(), getHeight(), 0, 0, getWidth(), getHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#endif // !defined(USE_GLES2) && !defined(AMIGAOS)

} // End of namespace OpenGL

#endif
