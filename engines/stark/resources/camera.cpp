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

#include "engines/stark/resources/camera.h"

#include "engines/stark/debug.h"
#include "engines/stark/formats/xrc.h"
#include "engines/stark/scene.h"
#include "engines/stark/services/services.h"

namespace Stark {

Camera::~Camera() {
}

Camera::Camera(Resource *parent, byte subType, uint16 index, const Common::String &name) :
		Resource(parent, subType, index, name),
		_f1(0),
		_fov(45),
		_nearClipPlane(100.0),
		_farClipPlane(64000.0) {
	_type = TYPE;
}

void Camera::setClipPlanes(float near, float far) {
	_nearClipPlane = near;
	_farClipPlane = far;
}

void Camera::readData(XRCReadStream *stream) {
	_position = stream->readVector3();
	_lookDirection = stream->readVector3();
	_f1 = stream->readFloat();
	_fov = stream->readFloat();
	_viewport = stream->readRect();
	_v4 = stream->readVector3();
}

void Camera::onEnterLocation() {
	Resource::onEnterLocation();

	Scene *scene = StarkServices::instance().scene;
	scene->initCamera(_position, _lookDirection, _fov, _viewport, _nearClipPlane, _farClipPlane);
}

void Camera::printData() {
	Common::Debug debug = streamDbg();
	debug << "position: " << _position << "\n";
	debug << "lookDirection: " << _lookDirection << "\n";
	debug << "f1: " << _f1 << "\n";
	debug << "fov: " << _fov << "\n";
	_viewport.debugPrint(0, "viewport:");
	debug << "v4: " << _v4 << "\n";
}

} // End of namespace Stark
