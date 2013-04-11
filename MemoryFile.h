/*
 *  MemoryFile.h
 *  Dungeon
 *
 *  Created by Peter Lewis on 18/04/2009.
 *  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
 *
 */

#ifndef __MEMORY_FILE_H__
#define __MEMORY_FILE_H__

#include "File.h"
#include "MemoryStream.h"

namespace VFS
{

class MemoryFile : public SeekableFile
{
public:

	MemoryFile( const void* data, uint32_t size )
	: SeekableFile( )
	, m_Stream( data, size )
	{
	}
	
	virtual ~MemoryFile( ) { }
	
	// Read some data
	virtual uint32_t Read(void* data, uint32_t size)	{ return m_Stream.Read( data, size ); }
	
	// How far into the stream are we?
	virtual uint32_t Tell()								{ return m_Stream.Tell( ); }
	
	// Are we at the end of the stream?
	virtual bool CanRead()								{ return m_Stream.CanRead( ); }
	
	// Return the size of the file
	virtual uint32_t GetFileSize()						{ return m_Stream.GetSize( ); }
	
	// Seek in the file
	virtual void Seek(FileSeek seek, int32_t offset)	{ return m_Stream.Seek( seek, offset ); }

protected:

	MemoryStream	m_Stream;
};

}

#endif
