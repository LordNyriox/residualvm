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

#include "common/math.h"
#include "common/config-manager.h"

#include "engines/myst3/scene.h"
#include "engines/myst3/gfx.h"
#include "engines/myst3/myst3.h"
#include "engines/myst3/node.h"
#include "engines/myst3/state.h"

#include "graphics/colormasks.h"

#include "math/vector2d.h"

namespace Myst3 {

Scene::Scene(Myst3Engine *vm) :
		_vm(vm),
		_mouseSpeed(50) {
	updateMouseSpeed();
}

void Scene::updateCamera(Common::Point &mouse) {
	float pitch = _vm->_state->getLookAtPitch();
	float heading = _vm->_state->getLookAtHeading();

	if (!_vm->_state->getCursorLocked()) {
		float speed = 25 / (float)(200 - _mouseSpeed);

		// Adjust the speed according to the resolution
		float scale = _vm->_layout->scale();
		speed /= scale;

		if (ConfMan.getBool("mouse_inverted")) {
			pitch += mouse.y * speed;
		} else {
			pitch -= mouse.y * speed;
		}
		heading += mouse.x * speed;
	}

	// Keep heading within allowed values
	if (_vm->_state->isCameraLimited()) {
		float minHeading = _vm->_state->getMinHeading();
		float maxHeading = _vm->_state->getMaxHeading();

		if (minHeading < maxHeading) {
			heading = CLIP(heading, minHeading, maxHeading);
		} else {
			if (heading < minHeading && heading > maxHeading) {
				uint distToMin = (uint)ABS(heading - minHeading);
				uint distToMax = (uint)ABS(heading - maxHeading);
				if (distToMin > distToMax)
					heading = maxHeading;
				else
					heading = minHeading;
			}
		}
	}

	// Keep heading in 0..360 range
	if (heading > 360.0f)
		heading -= 360.0f;
	else if (heading < 0.0f)
		heading += 360.0f;

	// Keep pitch within allowed values
	float minPitch = _vm->_state->getCameraMinPitch();
	float maxPitch = _vm->_state->getCameraMaxPitch();

	if (_vm->_state->isCameraLimited()) {
		minPitch = _vm->_state->getMinPitch();
		maxPitch = _vm->_state->getMaxPitch();
	}

	pitch = CLIP(pitch, minPitch, maxPitch);

	_vm->_state->lookAt(pitch, heading);
	_vm->_state->setCameraPitch((int32)pitch);
	_vm->_state->setCameraHeading((int32)heading);
}

void Scene::drawSunspotFlare(const SunSpot &s) {
	uint8 a = (uint8)(s.intensity * s.radius);
	uint8 r, g, b;
	Graphics::colorToRGB< Graphics::ColorMasks<888> >(s.color, r, g, b);
	uint32 color = Graphics::ARGBToColor< Graphics::ColorMasks<8888> >(a, r, g, b);

	FloatRect viewport = _vm->_layout->frameViewport();
	_vm->_gfx->setViewport(viewport, false);

	_vm->_gfx->drawRect2D(FloatRect::unit(), color);
}

Math::Vector3d Scene::directionToVector(float pitch, float heading) {
	Math::Vector3d v;

	float radHeading = Common::deg2rad(heading);
	float radPitch = Common::deg2rad(pitch);

	v.setValue(0, cos(radPitch) * cos(radHeading));
	v.setValue(1, sin(radPitch));
	v.setValue(2, cos(radPitch) * sin(radHeading));

	return v;
}

float Scene::distanceToZone(float spotHeading, float spotPitch, float spotRadius, float heading, float pitch) {
	Math::Vector3d vLookAt = directionToVector(pitch, heading);
	Math::Vector3d vSun = directionToVector(spotPitch, spotHeading);
	float dotProduct = Math::Vector3d::dotProduct(vLookAt, -vSun);

	float distance = (0.05 * spotRadius - (dotProduct + 1.0) * 90) / (0.05 * spotRadius);
	return CLIP<float>(distance, 0.0, 1.0);
}

void Scene::updateMouseSpeed() {
	_mouseSpeed = ConfMan.getInt("mouse_speed");
}

void Scene::screenPosToDirection(const Common::Point &screen, float &pitch, float &heading) const {
	FloatRect frame = _vm->_layout->frameViewport();

	// Screen coords to window coords
	Common::Point pos = Common::Point(screen.x - frame.left(), screen.y - frame.top());

	// Window coords to normalized coords
	Math::Vector4d in;
	in.x() = pos.x * 2 / (float) frame.width() - 1.0;
	in.y() = 1.0 - pos.y * 2 / (float) frame.height();
	in.z() = 1.0;
	in.w() = 1.0;

	// Normalized coords to direction
	Math::Matrix4 A = _vm->_gfx->getMvpMatrix();
	A.inverse();
	Math::Vector4d out = A.transform(in);

	Math::Vector3d direction(out.x(), out.y(), out.z());
	direction.normalize();

	// 3D coords to polar coords
	Math::Vector2d horizontalProjection = Math::Vector2d(direction.x(), direction.z());
	horizontalProjection.normalize();

	pitch = 90 - Math::Angle::arcCosine(direction.y()).getDegrees();
	heading = Math::Angle::arcCosine(horizontalProjection.getY()).getDegrees();

	if (horizontalProjection.getX() > 0.0)
		heading = 360 - heading;
}

Common::Point Scene::scalePoint(const Common::Point &screen) const {
	FloatRect viewport;
	FloatSize originalSize;
	if (_vm->_state->getViewType() == kMenu) {
		viewport = _vm->_layout->menuViewport();
		originalSize = FloatSize(Renderer::kOriginalWidth, Renderer::kOriginalHeight);
	} else {
		viewport = _vm->_layout->frameViewport();
		originalSize = FloatSize(Renderer::kOriginalWidth, Renderer::kFrameHeight);
	}

	Common::Point scaledPosition = screen;
	scaledPosition.x -= viewport.left();
	scaledPosition.y -= viewport.top();
	scaledPosition.x = CLIP<int16>(scaledPosition.x, 0, viewport.width());
	scaledPosition.y = CLIP<int16>(scaledPosition.y, 0, viewport.height());

	scaledPosition.x *= originalSize.width()  / viewport.width();
	scaledPosition.y *= originalSize.height() / viewport.height();

	return scaledPosition;
}

} // end of namespace Myst3
