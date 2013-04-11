/*
 *  File.h
 *  Dungeon
 *
 *  Created by Peter Lewis on 18/04/2009.
 *  Copyright 2009 Roaring Fist Entertainment. All rights reserved.
 *
 */

#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

#include <stdio.h>
#include <string>
#include "File.h"

namespace VFS
{

// Strip a path down to it's base name
std::string BaseName(std::string in);

// Get the path of a file
std::string BasePath(const std::string& in);

// Get the file extension
std::string GetExtension(const std::string& in);

// Replace an extension. Make sure to provide extension with a leading dot!
std::string ReplaceExtension(std::string in, const char* const extension);

// Make a deep path
bool MakeNestedDirectories(const std::string& path);

}

#endif
