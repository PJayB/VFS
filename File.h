/*
 *  File.h
 *  Dungeon
 *
 *  Created by Peter Lewis on 18/04/2009.
 *  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
 *
 */

#ifndef __VFS_FILE_H__
#define __VFS_FILE_H__

#include "Stream.h"

namespace VFS
{

class File
{
public:

	virtual ~File() {}

	// Read some data
	virtual uint32_t Read(void* data, uint32_t size) = 0;

	// How far into the stream are we?
	virtual uint32_t Tell() = 0;

	// Are we at the end of the stream?
	virtual bool CanRead() = 0;

	// Return the size of the file
	virtual uint32_t GetFileSize() = 0;

	// Read a type. Reads sizeof(type) bytes;
	template<typename T> uint32_t Read( T& out )
	{
		return Read( &out, (uint32_t)sizeof(out) );
	}

protected:

	File() {}
};		

class SeekableFile : public File
{
public:
	
	virtual ~SeekableFile() {}

	// Seek in the file
	virtual void Seek(FileSeek seek, int32_t offset) = 0;

protected:
	
	SeekableFile() {}
};		

// Close a file
template<class T>
void CloseFile(T*& f)
{
	delete f;
	f = nullptr;
}

class InputFileStream : public InputStream
{
public:

	InputFileStream(File* f, bool take_ownership = false) 
		: m_file(f), m_takeOwnership( take_ownership )
	{
	}
	virtual ~InputFileStream() 
	{
		if ( m_takeOwnership ) { CloseFile( m_file ); }
	}

	// Read some data
	uint32_t Read(void* data, uint32_t size);

	// How far into the stream are we?
	uint32_t Tell();

	// Are we at the end of the stream?
	bool CanRead();

private:

	File* m_file;
	bool m_takeOwnership;
};

class SeekableFileStream : public SeekableStream
{
public:

	SeekableFileStream(SeekableFile* f, bool take_ownership = false)
		: m_file(f), m_takeOwnership( take_ownership )
	{
	}
	virtual ~SeekableFileStream() 
	{
		if ( m_takeOwnership ) { CloseFile( m_file ); }
	}

	// Read some data
	uint32_t Read(void* data, uint32_t size);

	// How far into the stream are we?
	uint32_t Tell();

	// Are we at the end of the stream?
	bool CanRead();

	// Seek in the file
	void Seek(FileSeek seek, int32_t offset);

private:

	SeekableFile* m_file;
	bool m_takeOwnership;
};

}

#endif