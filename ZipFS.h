
#ifndef __COMMON_ZIPFS_H
#define __COMMON_ZIPFS_H

#ifndef VFS_INTERNAL
#error		This is meant to be included by VFS only and isn't for public use.
#endif

#include "VFS.h"
#include "File.h"
#include <string>
#include <map>

#include "Zip/unzip.h"

namespace VFS
{
	using namespace VFS;

	class ZipFile;

	class ZipFS
	{
	public:

		ZipFS( const char* path, unzFile zipFile, const std::string& prepend );

		~ZipFS( );

		File* OpenFile( const char* path );

		void CloseFile( ZipFile* zf );
		
		// Check a file exists
		bool HasFile( const char* path );

		// Enumerates files under a given path
		void EnumerateFiles( const char* path, FileFilter filter );

		const std::string& GetPath() const { return m_path; }

	private:
	
		typedef std::map<std::string, unz_file_pos> FileStateMap;

		std::string		MakeFullPath( const char* path );

		std::string		m_path;
		unzFile			m_zipFile;
		ZipFile*		m_openFile;
		std::string		m_basePath;
		FileStateMap	m_index;
	};

	class ZipFile : public File
	{
	public:

		ZipFile( ZipFS* fs, unzFile f );
		~ZipFile( );

		// Read some data
		uint32_t Read(void* data, uint32_t size);

		// How far into the stream are we?
		uint32_t Tell();

		// Are we at the end of the stream?
		bool CanRead();

		// Return the size of the file
		uint32_t GetFileSize();

	private:

		unzFile			m_zipFile;
		ZipFS*			m_parent;
		unz_file_info	m_info;
	};
};

#endif