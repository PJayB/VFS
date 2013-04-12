
#include "VFS.h"
#include "File.h"
#include "Stream.h"
#include "FileUtils.h"
#include "GZFile.h"
#include "NormalFile.h"
#include "MemoryFile.h"

#include <vector>
#include <io.h>

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

	void SetRoot( const std::string& root )
	{
		m_root = root;
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

	void EnumerateFiles( const char* path, FileFilter filter )
	{
		for ( std::vector<ZipFS*>::iterator i = m_zips.begin();
			 i != m_zips.end(); ++i )
		{
			(*i)->EnumerateFiles( path, filter );
		}
	}

private:

	std::string m_root;
	std::vector<ZipFS*> m_zips;
};

static FileSystems g_fs;

std::string JoinPath( const char* fullpath )
{
	if ( !fullpath )
	{
		fullpath = "";
	}
	
	const std::string& root = g_fs.GetRoot();
	if ( root.size() )
	{
		if ( strncmp( fullpath, root.c_str(), root.size() ) == 0 )
			return fullpath;

		return root + fullpath;
	}
	else
	{
		return fullpath;
	}
}

std::string CleanPath( std::string fp, bool directory )
{
	// Canonicalize
	for (size_t i = 0; i < fp.size(); ++i)
	{
		if (fp[i] == '\\')
			fp[i] = '/';
	}

	// Condense double-//s and other bits
	size_t fill = 1;
	for (size_t i = 1; i < fp.size(); ++i)
	{
		if (fp[i-1] == '/' && fp[i] == '/')
			continue;

		fp[fill++] = fp[i];
	}

	fp[fill] = 0;

	size_t len = strlen(fp.c_str());

	// Update the new size
	fp.reserve(len+1);
	fp.resize(len);

	// Make sure we end in a trailing /
	if (directory && fp[fp.size()-1] != '/')
	{
		fp += '/';
	}

	return fp;
}

std::string MakeFullyQualifiedFileName( const char* fullpath )
{
	return CleanPath( JoinPath( fullpath ), false );
}

std::string MakeFullyQualifiedPath( const char* fullpath )
{
	return CleanPath( JoinPath( fullpath ), true );
}

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
	g_fs.SetRoot( CleanPath( root, true ) );
}

const char* GetRootDirectory()
{
	return g_fs.GetRoot().c_str(); 
}

bool AddZip( const char* path_to_vfs )
{
	std::string fullpath = MakeFullyQualifiedFileName( path_to_vfs );

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
	std::string prepend = BasePath( path_to_vfs );

	// Push it to the list of zips
	g_fs.AddZipFS( new ZipFS( fullpath.c_str(), zipFile, prepend ) );

	return true;
}

File* OpenFile( const char* fullpath, bool physical_only )
{
	std::string fn = MakeFullyQualifiedFileName( fullpath );

	if ( !physical_only )
	{
		// Try and find the file in the virtual file systems
		File* f = g_fs.OpenFileFromZip( fn.c_str() );
		if ( f )
		{
			return f;
		}
	}

	return OpenGeneralFile( fn.c_str() );
}

SeekableFile* OpenPhysicalFile( const char* fullpath )
{
	return OpenGeneralFile( MakeFullyQualifiedFileName( fullpath ).c_str() );
}

SeekableFile* OpenSeekableFile( const char* fullpath )
{
	std::string fn = MakeFullyQualifiedFileName( fullpath );

	// Try the VFS
	if ( g_fs.HasFileInZip( fn.c_str() ) )
	{
		File* f = g_fs.OpenFileFromZip( fn.c_str() );
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
		return OpenGeneralFile( fn.c_str() );
	}
}

// Checks if a file exists (returns false for existing directories)
bool FileExists( const char* path )
{
	std::string fn = MakeFullyQualifiedFileName( path );

	if ( g_fs.HasFileInZip( fn.c_str() ) )
	{
		return true;
	}

#ifdef WIN32
	// Win32
	if ( _access( fn.c_str(), 6 ) < 0 )
#else
	// *nix
	if ( access( fn.c_str(), 6 ) < 0 )
#endif
	{
		// Failure:
		return false;
	}

	FILE * f = nullptr;
	fopen_s( &f, fn.c_str(), "rb" );
	if ( f )
	{
		// It's a file
		fclose( f );
		return true;
	}
	return false;
}

// Checks if a file exists (returns false for existing files)
bool DirectoryExists( const char* path )
{
	std::string fn = MakeFullyQualifiedFileName( path );

#ifdef WIN32
	// Win32
	if ( _access( fn.c_str(), 6 ) < 0 )
#else
	// *nix
	if ( access( fn.c_str(), 6 ) < 0 )
#endif
	{
		// Failure:
		return false;
	}

	FILE * f = nullptr;
	fopen_s( &f, fn.c_str(), "rb" );
	if ( f )
	{
		// It's a file
		fclose( f );
		return false;
	}
	return true;
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

	out.reserve( out.size() + fileSize );

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
bool ReadWholeBinaryFile(const char* fullpath, std::vector<uint8_t>& out)
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

	out.resize(size);

	uint32_t r = file->Read( &out[0], size );

	CloseFile(file);

	if ( r != size )
	{
		return false;
	}

	return true;
}

void EnumeratePhysicalFiles( const std::string& path, FileFilter filter )
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
			EnumeratePhysicalFiles( path + ffd.cFileName + '/', filter );
		}
		else
		{
			filter( ( path + ffd.cFileName ).c_str() );
		}
	}
	while ( ::FindNextFileA( hFind, &ffd ) != 0 );

	::FindClose( hFind );
#else
#	error TODO
#endif
}

void EnumerateFiles( const char* path, FileFilter filter, bool physical_only )
{
	std::string fp = MakeFullyQualifiedPath( path );

	if ( !physical_only )
	{
		g_fs.EnumerateFiles( fp.c_str(), filter );
	}

	EnumeratePhysicalFiles( fp.c_str(), filter );
}

}
