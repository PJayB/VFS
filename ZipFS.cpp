
#define VFS_INTERNAL
#include "ZipFS.h"
#include "File.h"
#include "Stream.h"
#include "FileUtils.h"

#ifndef WIN32
#define _stricmp strcasecmp
#endif

#ifdef _DEBUG
#include <assert.h>
#endif

namespace VFS
{

void ReplaceSlashes( std::string& filename )
{
	// Convert the slashes
	for ( size_t i = 0; i < filename.size(); ++i )
	{
		if ( filename[i] == '\\' ) filename[i] = '/';
	}
}

ZipFS::ZipFS( const char* path, unzFile zipFile, const std::string& prepend )
: m_path( path )
, m_zipFile( zipFile )
, m_openFile( nullptr )
, m_basePath( prepend )
{
	ReplaceSlashes( m_basePath );
}

ZipFS::~ZipFS( )
{
	unzClose( m_zipFile );
}

std::string ZipFS::MakeFullPath( const char* path )
{
	std::string fullpath( path );
	ReplaceSlashes( fullpath );
	
	// Check the root directory
	if ( m_basePath.size() )
	{
		// If the path is too short, abort
		if ( fullpath.size() <= m_basePath.size() )
		{
			return "";
		}
		
		// Compare the start of the string
		std::string path = fullpath.substr( 0, m_basePath.size() );
		if ( _stricmp( path.c_str(), m_basePath.c_str() ) != 0 )
		{
			return "";
		}
		
		fullpath = fullpath.substr( m_basePath.size() );
	}
	
	return fullpath;
}

File* ZipFS::OpenFile( const char* path )
{
	if ( m_openFile )
	{
#ifdef _DEBUG
		assert( 0 && "You cannot have more than one zip file active at a time." );
#endif
		return nullptr;
	}

	std::string fullpath = MakeFullPath(path);
	
	if ( !fullpath.size() || unzLocateFile( m_zipFile, fullpath.c_str(), 2 ) != UNZ_OK )
	{
		return nullptr;
	}

	m_openFile = new ZipFile( this, m_zipFile );
	return m_openFile;
}

bool ZipFS::HasFile( const char* path )
{
	std::string fullpath = MakeFullPath(path);
	
	if ( !fullpath.size() || unzLocateFile( m_zipFile, fullpath.c_str(), 2 ) != UNZ_OK )
	{
		return false;
	}
	
	return true;
}

void ZipFS::CloseFile( ZipFile* zf )
{
#ifdef _DEBUG
	assert( zf == m_openFile );
#endif
	m_openFile = nullptr;
}

void ZipFS::EnumerateFiles( const char* path, FileListing& out_listing )
{
	std::string fullpath = MakeFullPath(path);
	
	if ( !fullpath.size() )
		return;

	unz_saved_state zstate;
	unzSaveState( m_zipFile, &zstate );

    int err = unzGoToFirstFile( m_zipFile );

    while (err == UNZ_OK)
    {
        char szCurrentFileName[256];
        err = unzGetCurrentFileInfo( 
			m_zipFile,
			NULL,                
			szCurrentFileName,
			sizeof(szCurrentFileName)-1,
			NULL,
			0,
			NULL,
			0);

        if (err == UNZ_OK)
        {
            // TODO
            err = unzGoToNextFile( m_zipFile );
        }
    }

    unzRestoreState( m_zipFile, &zstate );
}

ZipFile::ZipFile( ZipFS* fs, unzFile f )
: m_zipFile( f )
, m_parent( fs )
{
	unzOpenCurrentFile( m_zipFile );
	unzGetCurrentFileInfo( m_zipFile, &m_info, nullptr, 0, nullptr, 0, nullptr, 0 ); 
}

ZipFile::~ZipFile( )
{
	unzCloseCurrentFile( m_zipFile );
	m_parent->CloseFile( this );
}

// Read some data
uint32_t ZipFile::Read(void* data, uint32_t size)
{
	return (uint32_t)unzReadCurrentFile( m_zipFile, data, size );
}

// How far into the stream are we?
uint32_t ZipFile::Tell()
{
	return unztell( m_zipFile );
}

// Are we at the end of the stream?
bool ZipFile::CanRead()
{
	return !unzeof( m_zipFile );
}

// Return the size of the file
uint32_t ZipFile::GetFileSize()
{
	return (uint32_t)m_info.uncompressed_size;
}

}
