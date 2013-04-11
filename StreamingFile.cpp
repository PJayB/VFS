/*
 *  StreamingFile.cpp
 *  Dungeon
 *
 *  Created by Peter Lewis on 18/04/2009.
 *  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
 *
 */

#include "StreamingFile.h"
#include "Blob.h"
#include "FileUtils.h"
#include "File.h"

namespace VFS
{

StreamingFile::StreamingFile(void* data, uint32_t capacity)
: m_File(nullptr), m_Memory(data), m_Capacity(capacity), m_Progress(0), m_Complete(false)
{
}

StreamingFile::~StreamingFile()
{
	free(m_Memory);
}

// Create a streaming file
StreamingFile* StreamingFile::Create(uint32_t capacity)
{
	void* data = malloc(capacity);
	if (!data) { return nullptr; }
	return new StreamingFile(data, capacity);
}

// Open with this file
bool StreamingFile::Bind(File* file)
{
	m_File = file;
	m_Progress = 0;
	m_Complete = false;
	
	// Create the streaming file
	return true;
}

void StreamingFile::Unbind()
{
	m_Progress = 0;
	m_Complete = false;
}

void StreamingFile::CheckComplete()
{
	m_Complete = !m_File->CanRead();
}

// Progressive load
uint32_t StreamingFile::LoadProgressive(uint32_t numBytes)
{
	if (!m_File) { return 0; }
	if (m_Complete) { return 0; }
	if (!numBytes) { CheckComplete(); return 0; }
	uint32_t read = Read(numBytes);
	if (read != numBytes)
	{
		CheckComplete();
	}
	return read;
}

// Load the whole file
uint32_t StreamingFile::LoadAll()
{
	uint32_t read = 0;
	
	// Load the remaining data
	while(m_File && m_File->CanRead())
	{
		read += LoadProgressive(m_Capacity - m_Progress);
	}
		
	return read;
}

// Get this file as a blob. Do not free this! Will return nullptr if not fully loaded.
const void* StreamingFile::GetMemory() const
{
	if (IsFullyLoaded())
	{
		return m_Memory;
	}
	return nullptr;
}

uint32_t StreamingFile::Read(uint32_t size)
{
	// Oh no, we run out of buffer space!
	if (m_Progress > m_Capacity)
	{
		return 0;
	}
	
	// Check that we can read at all
	if (!m_File->CanRead() || size == 0)
	{
		return 0;
	}
	
	// Load the data
	uint32_t loaded = m_File->Read(RawData(m_Progress), size);
	
	// Move on the progress
	m_Progress += loaded;
	
	return loaded;
}

uint8_t* StreamingFile::RawData(uint32_t offset)
{
	return &((uint8_t*)m_Memory)[offset];
}

}
