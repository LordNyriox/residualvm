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

#ifndef GRAPHICS_OPENGL_SYSTEM_HEADERS_H
#define GRAPHICS_OPENGL_SYSTEM_HEADERS_H

#include "common/scummsys.h"

#ifdef USE_GLES2

#define GL_GLEXT_PROTOTYPES
#ifdef IPHONE
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif
#undef GL_GLEXT_PROTOTYPES

#ifndef GL_BGRA
#	define GL_BGRA GL_BGRA_EXT
#endif

#ifndef GL_COMPRESSED_RGBA_BPTC_UNORM
#	define GL_COMPRESSED_RGBA_BPTC_UNORM GL_COMPRESSED_RGBA_BPTC_UNORM_EXT
#endif

#if !defined(GL_UNPACK_ROW_LENGTH)
// The Android SDK does not declare GL_UNPACK_ROW_LENGTH_EXT
#define GL_UNPACK_ROW_LENGTH 0x0CF2
#endif

#else // USE_GLES2

// The OpenGL headers define directly all the functions in core up to version 1.3,
// however on Windows the OpenGL ABI specifies only functions up to version 1.1 can
// be linked with. So we also need to query function pointers for the functions we use
// from versions 1.2 and 1.3.
// Hide the OpenGL definitions that would conflict with our own function pointer definitions.
#define glActiveTexture        glActiveTexture_hidden
#define glCompressedTexImage2D glCompressedTexImage2D_hidden

#if defined(SDL_BACKEND) && defined(USE_OPENGL)
#include <SDL_opengl.h>
#elif defined(USE_OPENGL)
#include <GL/gl.h>
#endif

#undef glActiveTexture
#undef glCompressedTexImage2D

#endif // USE_GLES2

#if !defined(GL_MAX_SAMPLES)
// The Android SDK and SDL1 don't declare GL_MAX_SAMPLES
#define GL_MAX_SAMPLES 0x8D57
#endif

#endif // GRAPHICS_OPENGL_SYSTEM_HEADERS_H
