/*
 *  StreamingFile.h
 *  Dungeon
 *
 *  Created by Peter Lewis on 18/04/2009.
 *  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
 *
 */

#ifndef __STREAMING_FILE_H__
#define __STREAMING_FILE_H__

#include "File.h"

namespace VFS
{
using namespace VFS;

class StreamingFile
{
public:
	
	~StreamingFile();
	
	// Load a streaming file. This allocates space for the file, but no reading
	// takes place.
	static StreamingFile* Create(uint32_t size);
	
	// Is a file open?
	bool IsBound() const { return m_File != nullptr; }
	
	// Use this file
	bool Bind(File* file);

	// Unbind the file
	void Unbind();
	
	// Is a streaming file fully loaded?
	inline bool IsFullyLoaded() const { return m_Complete; }
	
	// How much has the file loaded so far?
	inline uint32_t GetProgress() const { return m_Progress; }
	
	// Return the capacity
	inline uint32_t GetCapacity() const { return m_Capacity; }
	
	// Progressive load
	uint32_t LoadProgressive(uint32_t numBytes);
	
	// Load the whole file
	uint32_t LoadAll();
	
	// Do not free this! Will return nullptr if not fully loaded.
	const void* GetMemory() const;
	
private:
	
	StreamingFile(void* data, uint32_t capacity);
	
	uint32_t Read(uint32_t size);
	uint8_t* RawData(uint32_t offset);
	void CheckComplete();
	
	void* m_Memory;
	File* m_File;
	uint32_t m_Progress;
	uint32_t m_Capacity;
	bool m_Complete;
};

}

#endif
