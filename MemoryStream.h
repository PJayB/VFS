/*
 *  MemoryStream.h
 *  Dungeon
 *
 *  Created by Peter Lewis on 18/04/2009.
 *  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
 *
 */

#ifndef __MEMORY_STREAM_H__
#define __MEMORY_STREAM_H__

#include "Stream.h"

namespace VFS
{

class MemoryStream : public SeekableStream
{
public:

	MemoryStream( const void* data, uint32_t size );
	virtual ~MemoryStream( ) {}
	
	// Get the size of the stream
	virtual uint32_t GetSize() const;
	
	// Read some data
	virtual uint32_t Read(void* data, uint32_t size);
	
	// How far into the stream are we?
	virtual uint32_t Tell();
	
	// Are we at the end of the stream?
	virtual bool CanRead();
	
	// Seek in the stream
	virtual void Seek(FileSeek seek, int32_t offset);
	
protected:

	uint32_t			m_Cursor;
	const uint8_t*		m_Data;
	uint32_t			m_Size;
};

}

#endif
