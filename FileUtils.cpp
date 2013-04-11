/*
 *  File.cpp
 *  Dungeon
 *
 *  Created by Peter Lewis on 18/04/2009.
 *  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
 *
 */

#include "FileUtils.h"

#include <direct.h>

namespace VFS
{

std::string BaseName(std::string in)
{
	// Find the location of the file and extension
	std::string tmp = in;
	std::string::size_type dotpos = tmp.find_last_of(".");
	std::string::size_type dirpos = tmp.find_last_of("/\\");
	
	// If there were no path slashes, start from the beginning
	if (dirpos == std::string::npos) { dirpos = 0; }
	// Otherwise start from the character after
	else { dirpos++; }
	
	// If there was no dot, read to the end
	// Ensure the dot comes before the slash
	if (dotpos == std::string::npos || dotpos < dirpos) 
	{ 
		dotpos = tmp.size(); 
	}
	
	// Strip the file and extension
	return tmp.substr(dirpos, dotpos - dirpos);
}

std::string BasePath(const std::string& in)
{
	// Find the location of the file and extension
	std::string::size_type dirpos = in.find_last_of("/\\");
	
	return in.substr(0, dirpos + 1);
}

// Get a file extension
std::string GetExtension(const std::string& in)
{
	// Find the last .
	std::string::size_type dotPos = in.find_last_of(".");
	if (dotPos == std::string::npos)
	{
		return "";
	}
	
	// Return the substring
	return in.substr(dotPos + 1);	
}

// Replace an extension
std::string ReplaceExtension(std::string in, const char* const extension)
{
	// Find the last .
	std::string::size_type dotPos = in.find_last_of(".");
	if (dotPos == std::string::npos)
	{
		return in + extension;
	}
	
	// Replace the string
	return in.substr(0, dotPos) + extension;
}

bool MakeNestedDirectories(const std::string& path)
{
	int last_return_value = 0;

	std::string::size_type i = 0;
	while ( ( i = path.find_first_of("\\/", i+1) ) != std::string::npos )
	{
		std::string subpath = path.substr(0, i);
		last_return_value = _mkdir(subpath.c_str());
	}
	return last_return_value != -1;
}

}

