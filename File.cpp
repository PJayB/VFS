/*
 *  File.cpp
 *  Dungeon
 *
 *  Created by Peter Lewis on 18/04/2009.
 *  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
 *
 */

#include "File.h"

namespace VFS
{

uint32_t		InputFileStream::Read(void* data, uint32_t size) { return m_file->Read( data, size ); }
uint32_t		InputFileStream::Tell() { return m_file->Tell(); }
bool	InputFileStream::CanRead() { return m_file->CanRead(); }

uint32_t		SeekableFileStream::Read(void* data, uint32_t size) { return m_file->Read( data, size ); }
uint32_t		SeekableFileStream::Tell() { return m_file->Tell(); }
bool	SeekableFileStream::CanRead() { return m_file->CanRead(); }
void	SeekableFileStream::Seek(FileSeek seek, int32_t offset) { return m_file->Seek( seek, offset ); }
}
