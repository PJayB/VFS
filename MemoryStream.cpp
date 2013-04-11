/*
 *  MemoryStream.cpp
 *  Dungeon
 *
 *  Created by Peter Lewis on 18/04/2009.
 *  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
 *
 */

#include "MemoryStream.h"

#include <memory.h>
#include <algorithm>

namespace VFS
{

MemoryStream::MemoryStream( const void* data, uint32_t size )
: SeekableStream()
, m_Cursor( 0 )
, m_Data( (const uint8_t*) data )
, m_Size( size )
{
}

uint32_t MemoryStream::GetSize() const
{
	return m_Size;
}

uint32_t MemoryStream::Read( void* data, uint32_t size )
{
	if ( !m_Size || !m_Data || !size || !data || m_Cursor >= m_Size )
	{
		return 0;
	}
	
	uint32_t real_size = std::min( m_Size, m_Cursor + size ) - m_Cursor;
	memcpy( data, &m_Data[m_Cursor], real_size);
	m_Cursor += real_size;
	
	return real_size;
}

uint32_t MemoryStream::Tell()
{
	return m_Cursor;
}

bool MemoryStream::CanRead()
{
	return m_Cursor < m_Size && m_Data != nullptr;
}

void MemoryStream::Seek( FileSeek seek, int32_t offset )
{
	switch ( seek )
	{
	case FSEEK_BEGIN: 	m_Cursor = offset; break;
	case FSEEK_END:		m_Cursor = m_Size - offset; break;
	case FSEEK_CURSOR:	m_Cursor += offset; break;
	}

	if ( m_Cursor < 0 ) { m_Cursor = 0; }
	if ( m_Cursor > m_Size ) { m_Cursor = m_Size; }
}

}

