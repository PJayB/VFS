/*
 *  Stream.h
 *  Dungeon
 *
 *  Created by Peter Lewis on 18/04/2009.
 *  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
 *
 */

#ifndef __VFS_STREAM_H__
#define __VFS_STREAM_H__

#include <cstdint>

namespace VFS
{

enum FileSeek
{
	FSEEK_BEGIN,
	FSEEK_END,
	FSEEK_CURSOR
};

class InputStream
{
public:

	virtual ~InputStream() {}

	// Read some data
	virtual uint32_t Read(void* data, uint32_t size) = 0;
	
	// How far into the stream are we?
	virtual uint32_t Tell() = 0;
	
	// Are we at the end of the stream?
	virtual bool CanRead() = 0;

protected:
	
	InputStream() {}
};

class SeekableStream : public InputStream
{
public:

	virtual ~SeekableStream() {}

	// Seek in the stream
	virtual void Seek(FileSeek seek, int32_t offset) = 0;

protected:

	SeekableStream() {}
};

}

#endif
