/*
 *  Blob.h
 *  Dungeon
 *
 *  Created by Peter Lewis on 15/04/2009.
 *  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
 *
 */


#ifndef __BLOB_H__
#define __BLOB_H__

namespace VFSTools
{

// A blob of data
class Blob
{
public:

	// Create a blob
	static Blob* Create(unsigned int size);

	// Free a blob
	static void Free(Blob*& blob);

	unsigned int m_Size;
	void* m_Data;

protected:

	Blob();
	~Blob();
};

}

#endif
