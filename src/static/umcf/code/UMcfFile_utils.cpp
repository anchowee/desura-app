/*
Desura is the leading indie game distribution platform
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)

$LicenseInfo:firstyear=2014&license=lgpl$
Copyright (C) 2014, Linden Research, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation;
version 2.1 of the License only.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, see <http://www.gnu.org/licenses/>
or write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
$/LicenseInfo$
*/


#pragma warning( disable : 4627 )

#include "Common.h"
#include "UMcfFile_utils.h"

#ifdef NIX

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

bool FileOpen(FILEHANDLE &fh, const wchar_t* path, uint32 mode)
{
	gcString p(path);

	if (mode == FILE_WRITE)
	{
		fh = fopen64(p.c_str(), "wb");
	}
	else
	{
		fh = fopen64(p.c_str(), "rb");
	}

	return IsValidFileHandle(fh);
}

void FileClose(FILEHANDLE fh)
{
	fclose(fh);
}

uint64 FileSize(FILEHANDLE fh)
{
	uint64 pos = ftell(fh);
	
	fseek(fh, 0, SEEK_END);
	uint64 size = ftell(fh);
	
	fseek(fh, pos, SEEK_SET);
	return size;
}

bool FileSeek(FILEHANDLE fh, uint64 pos)
{
	return fseek(fh, pos, SEEK_SET) == 0;
}

bool FileRead(FILEHANDLE fh, uint32 size, char* buff)
{
	if (size == 0)
		return true;

	size_t dwRead = fread(buff, size, 1, fh);

	if (dwRead == 0)
		return false;

	if (dwRead != 1)
		return false;

	return true;
}

bool FileWrite(FILEHANDLE fh, uint32 size, char* buff)
{
	if (size == 0)
		return true;

	size_t dwWrite = fwrite(buff, size, 1, fh);

	if (dwWrite == 0)
		return false;

	if (dwWrite != 1)
		return false;

	return true;
}

void FileDelete(const wchar_t* file)
{
	gcString f(file);
	remove(f.c_str());
}

bool IsValidFileHandle(FILEHANDLE fh)
{
	return fh != nullptr;
}

bool CreateDir(const wchar_t* path)
{
	gcString p(path);

	if (!mkdir(p.c_str(), 0777))
	{
		if (errno != EEXIST)
			return false;
	}	

	return true;
}

#else


bool FileOpen(FILEHANDLE &fh, const wchar_t* path, uint32 mode)
{
	if (mode == FILE_WRITE)
		fh = CreateFileW(path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
	else
		fh = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

	return IsValidFileHandle(fh);
}

void FileClose(FILEHANDLE fh)
{
	CloseHandle(fh);
}

uint64 FileSize(FILEHANDLE fh)
{
	DWORD high = 0;
	DWORD low = GetFileSize(fh, &high);

	if (low == 0xFFFFFFFF)
		return 0;

	uint64 res = (((uint64)high)<<32) + (uint64)low;
	return res;
}

bool FileSeek(FILEHANDLE fh, uint64 pos)
{
	LARGE_INTEGER mov;
	mov.QuadPart = pos;

	BOOL res = SetFilePointerEx(fh, mov, nullptr, FILE_BEGIN);
	return (res != 0);
}

bool FileRead(FILEHANDLE fh, uint32 size, char* buff)
{
	DWORD dwRead = 0;
	uint8 readRet = ReadFile(fh, buff, size, &dwRead, nullptr);

	if (!readRet)
		return false;

	if (dwRead != size)
		return false;

	return true;
}

bool FileWrite(FILEHANDLE fh, uint32 size, char* buff)
{
	DWORD dwWrite = 0;
	DWORD writeRes = WriteFile(fh, buff, size, &dwWrite, nullptr);

	if (writeRes == 0)
		return false;

	if (dwWrite != size)
		return false;

	return true;
}

void FileDelete(const wchar_t* file)
{
	DeleteFileW(file);
}

bool IsValidFileHandle(FILEHANDLE fh)
{
	return fh != INVALID_HANDLE_VALUE;
}

bool CreateDir(const wchar_t* path)
{
	if (!path)
		return false;

	if (!CreateDirectoryW(path, nullptr))
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
			return false;
	}

	return true;
}


#endif
