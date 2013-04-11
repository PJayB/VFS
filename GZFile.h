/*
 *  GZFile.h
 *  Dungeon
 *
 *  Created by Peter Lewis on 18/04/2009.
 *  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
 *
 */

#ifndef __GZ_FILE_H__
#define __GZ_FILE_H__

#include "File.h"
#include <zlib/zlib.h>

namespace VFS
{
using namespace VFS;

class GZFile : public SeekableFile
{
public:

	// Open a GZ file
	static GZFile* Open(const char* const fullpath);
	
	// Read a series of bytes. Returns the number of bytes read.
	uint32_t Read(void* dest, uint32_t len);
	
	// Return the size of the file
	uint32_t GetFileSize();
	
	// Seek in the file
	void Seek(FileSeek seek, int32_t offset);
	
	// Tell where the cursor is in the file
	uint32_t Tell();
	
	// Can we read from this file?
	bool CanRead();

private:
	
	GZFile(gzFile f);
	virtual ~GZFile();	

	gzFile m_File;
};

}

#endif
