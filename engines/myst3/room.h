/* Residual - A 3D game interpreter
 *
 * Residual is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the AUTHORS
 * file distributed with this source distribution.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 *
 * $URL$
 * $Id$
 *
 */

#ifndef MYST3_ROOM_H
#define MYST3_ROOM_H

#ifdef SDL_BACKEND
#include <SDL_opengl.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "engines/myst3/archive.h"

#include "graphics/surface.h"
#include "graphics/jpeg.h"
#include "graphics/conversion.h"

namespace Myst3 {

class Room {
	private:
		static const int _cubeTextureSize = 1024;
		GLuint _cubeTextures[6];
		
	public:
		void setFaceTextureRGB(int face, Graphics::Surface *texture);
		void setFaceTextureJPEG(int face, Graphics::JPEG *jpeg);
		void draw();
		void load(Archive &archive, uint16 index);
};

} // end of namespace Myst3

#endif
