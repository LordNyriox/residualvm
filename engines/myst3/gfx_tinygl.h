/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the AUTHORS
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef GFX_TINYGL_H_
#define GFX_TINYGL_H_

#include "common/rect.h"
#include "common/system.h"
#include "math/vector3d.h"

#include "engines/myst3/gfx.h"
#include "graphics/tinygl/zgl.h"

namespace Myst3 {

class TinyGLRenderer : public Renderer {
public:
	TinyGLRenderer(OSystem *system);
	~TinyGLRenderer() override;

	void init() override;
	void setViewport(const FloatRect &viewport, bool is3d) override;
	void clear() override;

	Texture *createTexture(const Graphics::Surface &surface) override;

	void drawRect2D(const FloatRect &screenRect, uint32 color) override;
	void drawTexturedRect2D(const FloatRect &screenRect, const FloatRect &textureRect, Texture &texture,
	                        float transparency = -1.0, bool additiveBlending = false) override;
	void drawTexturedRect3D(const Math::Vector3d &topLeft, const Math::Vector3d &bottomLeft,
	                        const Math::Vector3d &topRight, const Math::Vector3d &bottomRight,
	                        Texture &texture) override;

	void drawCube(Texture **textures) override;

	Graphics::Surface *getScreenshot(const Common::Rect &screenViewport) override;

	virtual void flipBuffer() override;
private:
	void drawFace(uint face, Texture *texture);

	TinyGL::FrameBuffer *_fb;
};

} // End of namespace Myst3

#endif // GFX_H_
