
#include "VFS.h"
#include "File.h"
#include "Stream.h"
#include "FileUtils.h"
#include "GZFile.h"
#include "NormalFile.h"
#include "MemoryFile.h"
#include "Blob.h"

#include <vector>

#define VFS_INTERNAL
#include "ZipFS.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#ifdef RF_WIN32
#define kPathSep '\\'
#else 
#define kPathSep '/'
#endif

namespace VFS
{

class FileSystems
{
public:

	FileSystems( )
	{
	}

	~FileSystems( )
	{
		for ( std::vector<ZipFS*>::iterator i = m_zips.begin();
			  i != m_zips.end(); ++i )
		{
			delete ( *i );
		}
	}

	void SetRoot( const char* root )
	{
		if ( root && *root )
		{
			m_root = root;

			if ( m_root[ m_root.size() - 1 ] != '/' &&
				 m_root[ m_root.size() - 1 ] != '\\' )
			{
				m_root += kPathSep;
			}
		}
		else
		{
			m_root = "";
		}
	}

	const std::string& GetRoot( )
	{
		return m_root;
	}

	void AddZipFS( ZipFS* zfs )
	{
		m_zips.push_back( zfs );
	}

	ZipFS* FindZip( const char* path )
	{
		// Check we don't already have this file open
		for ( std::vector<ZipFS*>::iterator i = m_zips.begin();
			i != m_zips.end(); ++i )
		{
			if ( (*i)->GetPath( ) == std::string( path ) )
			{
				*i;
			}
		}
		return nullptr;
	}

	File* OpenFileFromZip( const char* fullpath )
	{
		File* f = nullptr;
		for ( std::vector<ZipFS*>::iterator i = m_zips.begin();
			i != m_zips.end() && f == nullptr; ++i )
		{
			f = (*i)->OpenFile( fullpath );
		}
		return f;
	}
	
	// Does this file exist in the VFS?
	bool HasFileInZip( const char* fullpath )
	{
		for ( std::vector<ZipFS*>::iterator i = m_zips.begin();
			 i != m_zips.end(); ++i )
		{
			if ( (*i)->HasFile( fullpath ) )
			{
				return true;
			}
		}
		return false;
	}

	void EnumerateFiles( const char* path, FileListing& out_listing )
	{
		for ( std::vector<ZipFS*>::iterator i = m_zips.begin();
			 i != m_zips.end(); ++i )
		{
			(*i)->EnumerateFiles( path, out_listing );
		}
	}

private:

	std::string m_root;
	std::vector<ZipFS*> m_zips;
};

static FileSystems g_fs;

/*
	TODO - make this better
*/

// Perform an FOpen
SeekableFile* OpenGeneralFile(const char* fullpath)
{
	SeekableFile* file = nullptr;

	// Does the file contain a .gz?
	if (strstr(fullpath, ".gz"))
	{
		file = GZFile::Open(fullpath);
	}
	else
	{
		file = NormalFile::Open(fullpath);
	}

	return file;
}

void SetRootDirectory( const char* root )
{
	g_fs.SetRoot( root );
}

bool AddZip( const char* path_to_vfs, bool fully_qualified )
{
	std::string fullpath = fully_qualified ? path_to_vfs : MakeFullPath( path_to_vfs );

	if ( g_fs.FindZip( fullpath.c_str() ) )
	{
		return true;
	}

	// Check we can open the zipfile
	unzFile zipFile = unzOpen( fullpath.c_str() );
	if ( !zipFile )
	{
		return false;
	}

	// Get the prepend path
	std::string prepend;
	if ( !fully_qualified )
	{
		prepend = BasePath( path_to_vfs );
	}

	// Push it to the list of zips
	g_fs.AddZipFS( new ZipFS( fullpath.c_str(), zipFile, prepend ) );

	return true;
}

File* OpenFile( const char* fullpath, bool physical_only )
{
	if ( !physical_only )
	{
		// Try and find the file in the virtual file systems
		File* f = g_fs.OpenFileFromZip( fullpath );
		if ( f )
		{
			return f;
		}
	}

	return OpenGeneralFile( MakeFullPath( fullpath ).c_str() );
}

SeekableFile* OpenPhysicalFile( const char* fullpath )
{
	return OpenGeneralFile( MakeFullPath( fullpath ).c_str() );
}

SeekableFile* OpenSeekableFile( const char* fullpath )
{
	// Try the VFS
	if ( g_fs.HasFileInZip( fullpath ) )
	{
		File* f = g_fs.OpenFileFromZip( fullpath );
		if ( !f )
		{
			return nullptr;
		}
		
		// Read the whole file
		uint32_t size = f->GetFileSize();
		uint8_t* data = new uint8_t[ size ];
		size = f->Read( data, size );
		
		// Close the file
		CloseFile( f );
		
		// New create a new stream
		return new MemoryFile( data, size );
	}
	else 
	{
		return OpenPhysicalFile( fullpath );
	}
}

std::string JoinPath( const char* fullpath )
{
	if ( !fullpath )
	{
		fullpath = "";
	}
	
	if ( g_fs.GetRoot().size() )
	{
		return g_fs.GetRoot() + fullpath;
	}
	else
	{
		return fullpath;
	}
}

std::string MakeFullPath( const char* fullpath )
{
	std::string fp = JoinPath( fullpath );

	// Condense double-//s and other bits

	// Make sure we end in a trailing /
	todo
}

// Read a whole file
bool ReadWholeTextFile(const char* fullpath, std::string& out)
{
	File* file = OpenFile(fullpath);
	if ( !file )
	{
		return false;
	}
	
	int32_t fileSize = (int32_t)file->GetFileSize();

	// Keep reading until we can't read no more.
	out = "";
	do 
	{
		char text[257];
		uint32_t r = file->Read(text, min(256, fileSize));
		
		text[r] = 0;
		out += text;
		fileSize -= r;
	} while ( file->CanRead( ) && fileSize > 0 );

	CloseFile(file);
	return true;
}

// Read a whole file. Free the blob afterwards.
bool ReadWholeBinaryFile(const char* fullpath, VFSTools::Blob** out)
{
	File* file = OpenFile(fullpath);
	if ( !file )
	{
		return false;
	}

	// Get the size of the file
	uint32_t size = file->GetFileSize( );
	if ( !size )
	{
		return false;
	}

	// Allocate space for it
	*out = VFSTools::Blob::Create(size);
	if ( !*out )
	{
		return false;
	}

	// Read the data
	uint32_t r = file->Read( ( *out )->m_Data, size );

	CloseFile(file);

	if ( r != size )
	{
		VFSTools::Blob::Free(*out);
		return false;
	}

	return true;
}

void EnumeratePhysicalFiles( const std::string& path, FileListing& out_listing )
{
#ifdef WIN32
	WIN32_FIND_DATA ffd;

	std::string search = path + "*";

	HANDLE hFind = ::FindFirstFileA( search.c_str(), &ffd );
	if ( hFind == INVALID_HANDLE_VALUE )
		return;

	do
	{
		if ( *ffd.cFileName == '.' )
			continue;

		if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			// Recurse
			EnumeratePhysicalFiles( path + ffd.cFileName + '/', out_listing );
		}
		else
		{
			out_listing.insert( path + ffd.cFileName );
		}
	}
	while ( ::FindNextFileA( hFind, &ffd ) != 0 );

	::FindClose( hFind );
#else
#	error TODO
#endif
}

void EnumerateFiles( const char* path, FileListing& out_listing, bool physical_only )
{
	std::string fp = MakeFullPath( path );

	if ( !physical_only )
	{
		g_fs.EnumerateFiles( fp.c_str(), out_listing );
	}

	EnumeratePhysicalFiles( fp.c_str(), out_listing );
}

}
