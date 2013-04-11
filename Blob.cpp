/*
*  Blob.cpp
*  Dungeon
*
*  Created by Peter Lewis on 15/04/2009.
*  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
*
*/

#include <stdlib.h>
#include "Blob.h"

namespace VFSTools
{

Blob::Blob()
: m_Data(nullptr), m_Size(0) {}

Blob::~Blob() {}

// Make a blob
Blob* Blob::Create(unsigned int size)
{
	Blob* b = (Blob*)malloc(sizeof(Blob) + size);
	b->m_Size = size;
	b->m_Data = (void*)(b + 1);
	return b;
}

// Free a bundled file
void Blob::Free(Blob*& f)
{
	// Free the memory
	free(f);
	f = nullptr;
}

}
