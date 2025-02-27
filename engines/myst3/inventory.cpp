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

#include "engines/myst3/inventory.h"

#include "engines/myst3/cursor.h"
#include "engines/myst3/database.h"
#include "engines/myst3/resource_loader.h"
#include "engines/myst3/scene.h"
#include "engines/myst3/state.h"

namespace Myst3 {

const Inventory::ItemData Inventory::_availableItems[8] = {
	{   0, 41, 47, 481 },
	{  41, 38, 50, 480 },
	{  79, 38, 49, 279 },
	{ 117, 34, 48, 277 },
	{ 151, 35, 44, 345 },
	{ 186, 35, 44, 398 },
	{ 221, 35, 44, 447 },
	{   0,  0,  0,   0 }
};

Inventory::Inventory(Myst3Engine *vm) :
		_vm(vm),
		_texture(nullptr) {

	ResourceDescription desc = _vm->_resourceLoader->getRawData("GLOB", 1204);
	if (!desc.isValid())
		error("The inventory texture, GLOB-1204 was not found");

	TextureLoader textureLoader(*_vm->_gfx);
	_texture = textureLoader.load(desc, TextureLoader::kImageFormatTEX);
}

Inventory::~Inventory() {
	delete _texture;
}

bool Inventory::isMouseInside() {
	Common::Point mouse = _vm->_cursor->getPosition();
	FloatRect bottomBorder = _vm->_layout->bottomBorderViewport();
	return bottomBorder.contains(FloatPoint(mouse.x, mouse.y));
}

void Inventory::draw() {
	FloatRect screenViewport = _vm->_layout->unconstrainedViewport();
	_vm->_gfx->setViewport(screenViewport, false);

	if (_vm->isWideScreenModEnabled()) {
		// Draw a black background to cover the main game frame
		FloatRect bottomBorder = _vm->_layout->bottomBorderViewport()
		        .normalize(screenViewport.size());
		_vm->_gfx->drawRect2D(bottomBorder, 0xFF000000);
	}

	uint16 hoveredItemVar = hoveredItem();
	float textureScale = _texture->width / 256.f;

	for (ItemList::const_iterator it = _inventory.begin(); it != _inventory.end(); it++) {
		int32 state = _vm->_state->getVar(it->var);

		// Don't draw if the item is being dragged or is hidden
		if (state == -1 || state == 0)
			continue;

		const ItemData &item = getData(it->var);

		bool itemHighlighted = it->var == hoveredItemVar || state == 2;

		FloatRect textureRect = FloatSize(item.textureWidth, item.textureHeight)
		        .translate(FloatPoint(item.textureX, .0f))
		        .scale(textureScale)
		        .translate(FloatPoint(.0f, itemHighlighted ? _texture->height / 2.f : .0f))
		        .normalize(_texture->size());

		FloatRect itemRect = it->rect
		        .normalize(screenViewport.size());

		_vm->_gfx->drawTexturedRect2D(itemRect, textureRect, *_texture);
	}
}

void Inventory::reset() {
	_inventory.clear();
	reflow();
	updateState();
}

void Inventory::addItem(uint16 var, bool atEnd) {
	// Only add objects once to the inventory
	if (!hasItem(var)) {
		_vm->_state->setVar(var, 1);

		InventoryItem i;
		i.var = var;

		if (atEnd) {
			_inventory.push_back(i);
		} else {
			_inventory.push_front(i);
		}

		reflow();
		updateState();
	}
}

void Inventory::removeItem(uint16 var) {
	_vm->_state->setVar(var, 0);

	for (ItemList::iterator it = _inventory.begin(); it != _inventory.end(); it++) {
		if (it->var == var) {
			_inventory.erase(it);
			break;
		}
	}

	reflow();
	updateState();
}

void Inventory::addAll() {
	for (uint i = 0; _availableItems[i].var; i++)
		addItem(_availableItems[i].var, true);
}

bool Inventory::hasItem(uint16 var) {
	for (ItemList::iterator it = _inventory.begin(); it != _inventory.end(); it++) {
		if (it->var == var)
			return true;
	}

	return false;
}

const Inventory::ItemData &Inventory::getData(uint16 var) {
	for (uint i = 0; _availableItems[i].var; i++) {
		if (_availableItems[i].var == var)
			return _availableItems[i];
	}

	return _availableItems[7];
}

void Inventory::reflow() {
	float scale = _vm->_layout->scale();

	uint16 itemCount = 0;
	uint16 totalWidth = 0;

	for (uint i = 0; _availableItems[i].var; i++) {
		if (hasItem(_availableItems[i].var)) {
			totalWidth += _availableItems[i].textureWidth * scale;
			itemCount++;
		}
	}

	if (itemCount >= 2)
		totalWidth += 9 * scale * (itemCount - 1);

	FloatRect bottomBorder = _vm->_layout->bottomBorderViewport();
	uint left = (bottomBorder.width() - totalWidth) / 2;

	for (ItemList::iterator it = _inventory.begin(); it != _inventory.end(); it++) {
		const ItemData &item = getData(it->var);

		FloatSize itemSize = FloatSize(item.textureWidth, item.textureHeight)
		        .scale(scale);

		uint16 top = (bottomBorder.height() - itemSize.height()) / 2;

		it->rect = itemSize
		        .translate(FloatPoint(bottomBorder.left() + left, bottomBorder.top() + top));

		left += itemSize.width();

		if (itemCount >= 2)
			left += 9 * scale;
	}
}

uint16 Inventory::hoveredItem() {
	Common::Point mouse = _vm->_cursor->getPosition();

	for (ItemList::const_iterator it = _inventory.begin(); it != _inventory.end(); it++) {
		if(it->rect.contains(FloatPoint(mouse.x, mouse.y)))
			return it->var;
	}

	return 0;
}

void Inventory::useItem(uint16 var) {
	switch (var) {
	case 277: // Atrus
		closeAllBooks();
		_vm->_state->setJournalAtrusState(2);
		openBook(9, kRoomJournals, 100);
		break;
	case 279: // Saavedro
		closeAllBooks();
		_vm->_state->setJournalSaavedroState(2);
		openBook(9, kRoomJournals, 200);
		break;
	case 480: // Tomahna
		closeAllBooks();
		_vm->_state->setBookStateTomahna(2);
		openBook(8, kRoomNarayan, 220);
		break;
	case 481: // Releeshahn
		closeAllBooks();
		_vm->_state->setBookStateReleeshahn(2);
		openBook(9, kRoomJournals, 300);
		break;
	case 345:
		_vm->dragSymbol(345, 1002);
		break;
	case 398:
		_vm->dragSymbol(398, 1001);
		break;
	case 447:
		_vm->dragSymbol(447, 1000);
		break;
	default:
		debug("Used inventory item %d which is not implemented", var);
	}
}

void Inventory::closeAllBooks() {
	if (_vm->_state->getJournalAtrusState())
		_vm->_state->setJournalAtrusState(1);
	if (_vm->_state->getJournalSaavedroState())
		_vm->_state->setJournalSaavedroState(1);
	if (_vm->_state->getBookStateTomahna())
		_vm->_state->setBookStateTomahna(1);
	if (_vm->_state->getBookStateReleeshahn())
		_vm->_state->setBookStateReleeshahn(1);
}

void Inventory::openBook(uint16 age, uint16 room, uint16 node) {
	if (!_vm->_state->getBookSavedNode()) {
		_vm->_state->setBookSavedAge(_vm->_state->getLocationAge());
		_vm->_state->setBookSavedRoom(_vm->_state->getLocationRoom());
		_vm->_state->setBookSavedNode(_vm->_state->getLocationNode());
	}

	_vm->_state->setLocationNextAge(age);
	_vm->_state->setLocationNextRoom(room);
	_vm->goToNode(node, kTransitionFade);
}

void Inventory::addSaavedroChapter(uint16 var) {
	_vm->_state->setVar(var, 1);
	_vm->_state->setJournalSaavedroState(2);
	_vm->_state->setJournalSaavedroChapter(var - 285);
	_vm->_state->setJournalSaavedroPageInChapter(0);
	openBook(9, kRoomJournals, 200);
}

void Inventory::loadFromState() {
	Common::Array<uint16> items = _vm->_state->getInventory();

	_inventory.clear();
	for (uint i = 0; i < items.size(); i++)
		addItem(items[i], true);
}

void Inventory::updateState() {
	Common::Array<uint16> items;
	for (ItemList::iterator it = _inventory.begin(); it != _inventory.end(); it++)
		items.push_back(it->var);

	_vm->_state->updateInventory(items);
}

void Inventory::updateCursor() {
	uint16 item = hoveredItem();
	if (item > 0) {
		_vm->_cursor->changeCursor(1);
	} else {
		_vm->_cursor->changeCursor(8);
	}
}

DragItem::DragItem(Myst3Engine *vm, uint id):
		_vm(vm),
		_frame(1),
		_texture(nullptr) {
	ResourceDescription movieDesc = _vm->_resourceLoader->getStillMovie("DRAG", id);

	if (!movieDesc.isValid())
		error("Movie %d does not exist", id);

	// Load the video
	VideoLoader videoLoader;
	_movieStream = videoLoader.load(movieDesc);
	assert(_movieStream);

	_bink.setDefaultHighColorFormat(Texture::getRGBAPixelFormat());
	if (!_bink.loadStream(_movieStream)) {
		error("Invalid Bink video file '%s-%d'", "DRAG", id);
	}

	_bink.start();

	const Graphics::Surface *frame = _bink.decodeNextFrame();
	_texture = _vm->_gfx->createTexture(*frame);

	if (movieDesc.type() == Archive::kModdedMovie) {
		// For modded resources, the screen size is that from the original file
		 ResourceDescription::VideoData videoData = movieDesc.videoData();
		_screenSize = FloatSize(videoData.width, videoData.height);
	} else {
		_screenSize = _texture->size();
	}
}

DragItem::~DragItem() {
	delete _texture;
}

void DragItem::drawOverlay() {
	FloatRect viewport = _vm->_layout->unconstrainedViewport();

	FloatRect itemRect = getPosition()
	        .normalize(viewport.size());

	_vm->_gfx->setViewport(viewport, false);
	_vm->_gfx->drawTexturedRect2D(itemRect, FloatRect::unit(), *_texture, 0.99f);
}

void DragItem::setFrame(uint16 frame) {
	if (frame != _frame) {
		_frame = frame;
		_bink.seekToFrame(frame - 1);
		const Graphics::Surface *s = _bink.decodeNextFrame();
		_texture->update(*s);
	}
}

FloatRect DragItem::getPosition() {
	Common::Point mouse = _vm->_cursor->getPosition();
	FloatRect viewport = _vm->_layout->screenViewport();
	float scale = _vm->_layout->scale();

	FloatSize itemSize = _screenSize
	        .scale(scale);

	FloatPoint itemCenter = FloatPoint(
	            CLIP<float>(mouse.x, viewport.left() + itemSize.width()  / 2, viewport.right()  - itemSize.width()  / 2),
	            CLIP<float>(mouse.y, viewport.top()  + itemSize.height() / 2, viewport.bottom() - itemSize.height() / 2)
	);

	FloatRect screenRect = FloatRect::center(itemCenter, itemSize);
	return screenRect;
}

} // End of namespace Myst3
