/*
 *  NormalFile.h
 *  Dungeon
 *
 *  Created by Peter Lewis on 18/04/2009.
 *  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
 *
 */

#ifndef __NORMAL_FILE_H__
#define __NORMAL_FILE_H__

#include <stdio.h>

namespace VFS
{
using namespace VFS;

class NormalFile : public SeekableFile
{
public:
	
	// Open a normal file
	static NormalFile* Open(const char* const fullpath);
	
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
	
	NormalFile(FILE* f);
	virtual ~NormalFile();	

	FILE* m_File;
};

}

#endif
