#include "engines/myst3/directorysubentry.h"
#include "common/str.h"
#include "common/debug.h"
#include "common/file.h"
#include "common/memstream.h"

void DirectorySubEntry::readFromStream(Common::SeekableReadStream &inStream) {
	_offset = inStream.readUint32LE();
	_size = inStream.readUint32LE();
	_padding = inStream.readUint16LE();
	_face = inStream.readByte();
	_type = inStream.readByte();

	dump();

	if (_padding == 2) {
		uint32 _padding2 = inStream.readUint32LE();
		uint32 _padding3 = inStream.readUint32LE();
		debug("position x %d y %d", _padding2, _padding3);
	} /*else if (_padding == 10) {
		uint32 _padding2 = inStream.readUint32LE();
		uint32 _padding3 = inStream.readUint32LE();
		inStream.skip(8 * sizeof(uint32));
		debug("position x %d y %d", _padding2, _padding3);
	}*/ else {
		inStream.skip(_padding * sizeof(uint32));
	}
}

void DirectorySubEntry::dump() {
	debug("offset : %x size: %d padding : %d face : %d type : %d", _offset, _size, _padding, _face, _type);
}

void DirectorySubEntry::dumpToFile(Common::SeekableReadStream &inStream, uint16 index) {
	char fileName[255];
	
	switch (_type) {
		case 0:
		case 5:
			sprintf(fileName, "dump/%d-%d.jpg", index, _face);
			break;
		case 1:
			sprintf(fileName, "dump/%d-%d.mask", index, _face);
			break;
		case 8:
			sprintf(fileName, "dump/%d.bik", index);
			break;
		default:
			sprintf(fileName, "dump/%d-%d.%d", index, _face, _type);
			break;
	}
	
	
	debug("Extracted %s", fileName);
	
	Common::DumpFile outFile;
	outFile.open(fileName);
	
	inStream.seek(_offset);
	
	uint8 *buf = new uint8[_size];
	
	inStream.read(buf, _size);
	outFile.write(buf, _size);
	
	delete[] buf;
	
	outFile.close();
}

Common::MemoryReadStream *DirectorySubEntry::dumpToMemory(Common::SeekableReadStream &inStream) {
	inStream.seek(_offset);
	return static_cast<Common::MemoryReadStream *>(inStream.readStream(_size));
}
