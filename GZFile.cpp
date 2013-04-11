/*
 *  GZFile.cpp
 *  Dungeon
 *
 *  Created by Peter Lewis on 18/04/2009.
 *  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
 *
 */

#include "VFS.h"
#include "GZFile.h"

namespace VFS
{
using namespace VFS;

GZFile::GZFile(gzFile f)
: m_File(f)
{
}

GZFile::~GZFile()
{
	gzclose(m_File);
}

// Read a series of bytes. Returns the number of bytes read.
uint32_t GZFile::Read(void* dest, uint32_t len)
{
	uint8_t* c8ptr = (uint8_t*)dest;
	uint32_t read = 0;
	while (!gzeof(m_File) && read != len)
	{
		read += (uint32_t)gzread(m_File, &c8ptr[read], sizeof(uint8_t) * (len - read));
	}
	return read;
}

// Return the size of the file
uint32_t GZFile::GetFileSize()
{
	uint32_t p = gztell(m_File);
	gzseek(m_File, 0, SEEK_END);
	uint32_t e = gztell(m_File);
	gzseek(m_File, p, SEEK_SET);
	return e;
}

// Seek in the file
void GZFile::Seek(FileSeek seek, int32_t offset)
{
	switch (seek)
	{
		case FSEEK_BEGIN:
			gzseek(m_File, offset, SEEK_SET);
			break;
		case FSEEK_END:
			gzseek(m_File, offset, SEEK_END);
			break;
		case FSEEK_CURSOR:
			gzseek(m_File, offset, SEEK_CUR);
			break;
		default:
			break;
	}
}

// Tell where the cursor is in the file
uint32_t GZFile::Tell()
{
	return (uint32_t)gztell(m_File);
}

// Can we read from the file?
bool GZFile::CanRead()
{
	return !gzeof(m_File);
}

// Perform an FOpen
GZFile* GZFile::Open(const char* fullpath)
{
	gzFile f = gzopen(fullpath, "rb");
	if (f)
	{
		return new GZFile(f);
	}
	return nullptr;
}

}