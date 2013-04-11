/*
 *  NormalFile.cpp
 *  Dungeon
 *
 *  Created by Peter Lewis on 18/04/2009.
 *  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
 *
 */

#include "VFS.h"
#include "NormalFile.h"

#ifdef RF_DEBUG
#	include <errno.h>
#endif

using namespace VFS;

namespace VFS
{

NormalFile::NormalFile(FILE* f)
: m_File(f)
{
}

NormalFile::~NormalFile()
{
	fclose(m_File);
}

// Read a series of bytes. Returns the number of bytes read.
uint32_t NormalFile::Read(void* dest, uint32_t len)
{
	uint8_t* c8ptr = (uint8_t*)dest;
	size_t read = 0;
	while (!feof(m_File) && read != len)
	{
		size_t thisRead = fread(&c8ptr[read], sizeof(uint8_t), (len - read), m_File);
		
		// If we didn't read anything, some error occurred
		if (thisRead == 0)
		{
			// oh fu-
			break;
		}
		
		read += thisRead;
	}
	
#ifdef RF_DEBUG
	// Throw up a warning :|
	if (read != len)
	{
		Log(LOG_WARNING, "File read did not complete: %d of %d bytes. (#%d)",
				read, len, errno);
	}
#endif
	
	return (uint32_t)read;
}

// Return the size of the file
uint32_t NormalFile::GetFileSize()
{
	long p = ftell(m_File);
	fseek(m_File, 0, SEEK_END);
	long e = ftell(m_File);
	fseek(m_File, p, SEEK_SET);
	return (uint32_t)e;
}

// Seek in the file
void NormalFile::Seek(FileSeek seek, int32_t offset)
{
	switch (seek)
	{
		case FSEEK_BEGIN:
			fseek(m_File, offset, SEEK_SET);
			break;
		case FSEEK_END:
			fseek(m_File, offset, SEEK_END);
			break;
		case FSEEK_CURSOR:
			fseek(m_File, offset, SEEK_CUR);
			break;
		default:
			break;
	}
}

// Tell where the cursor is in the file
uint32_t NormalFile::Tell()
{
	return (uint32_t)ftell(m_File);
}

// Can we read from the file?
bool NormalFile::CanRead()
{
	return !feof(m_File);
}

// Perform an FOpen
NormalFile* NormalFile::Open(const char* const fullpath)
{
#ifdef WIN32
	FILE* f = nullptr;
	fopen_s(&f, fullpath, "rb");
#else
	FILE* f = fopen(fullpath, "rb");
#endif
	if (f)
	{
		return new NormalFile(f);
	}
	return nullptr;
}

}


