
#ifndef __COMMON_VFS_H
#define __COMMON_VFS_H

#include "File.h"
#include <string>
#include <functional>
#include <unordered_set>

namespace VFSTools
{
	class Blob;
}

namespace VFS
{
	// Set the root directory for all default file reads.
	void SetRootDirectory( const char* root );

	// Get the root directory
	const char* GetRootDirectory();

	// Mount a zip fs at a path. By default it loads it from the value passed to
	// SetRootDirectory. If you already have a fully qualified path, specify this
	// to load the zip from anywhere on disk.
	bool AddZip( const char* path_to_vfs );

	// Checks if a file exists (returns false for existing directories)
	bool FileExists( const char* path );

	// Checks if a file exists (returns false for existing files)
	bool DirectoryExists( const char* path );

	// Open a file for reading and return a file pointer to it. You can request that 
	// the file come from the physical system if you wish.
	File* OpenFile( const char* path, bool physical_only = false );

	// Opens a file from the physical disk ONLY. The cool thing
	// about this is that you can seek! Hurrah!
	SeekableFile* OpenPhysicalFile( const char* path );
	
	// Opens a file that can be seeked into. Note that because some FS types
	// don't support seeking (e.g. ZipFS), the file may be copied into memory
	// for random access.
	SeekableFile* OpenSeekableFile( const char* path );

	// Returns the fully qualified path name for a file.
	// Note, this will ONLY return a full path within the default
	// file system mount point. It will NEVER return a file path
	// valid for a virtualized file system.
	std::string MakeFullyQualifiedFileName( const char* path );

	// Returns the fully qualified path name for a DIRECTORY, including trailing /.
	// Note, this will ONLY return a full path within the default
	// file system mount point. It will NEVER return a file path
	// valid for a virtualized file system.
	std::string MakeFullyQualifiedPath( const char* path );

	// Read a whole file
	bool ReadWholeTextFile( const char* fullpath, std::string& out );

	// Read a whole file. Free the blob afterwards.
	bool ReadWholeBinaryFile( const char* fullpath, std::vector<uint8_t>& out );

	// Handy structure for storing lists of files without dupes
	typedef std::unordered_set<std::string> FileListing;

	// Enumerates a directory for all subfiles and folders
	typedef std::function<void (const char*)> FileFilter;
	void EnumerateFiles( 
		const char* path, 
		FileFilter filter, 
		bool physical_only = false );
};

#endif