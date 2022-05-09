#include "MemMapFile.h"

namespace sge {

MemMapFile::~MemMapFile()
{
	close();
}

#if SGE_OS_WINDOWS

void MemMapFile::close()
{
	if (_span.size() <= 0)
		return;
	
	::UnmapViewOfFile(_span.data());
	if (_mapping)
	{
		::CloseHandle(_mapping);
		_mapping = nullptr;
	}

	_span = Span<const u8>();
	_fs.close();
}

void MemMapFile::openRead(StrView filename_)
{
	open(filename_, FileMode::OpenExists, FileAccess::Read, FileSharerMode::Read);
}

void MemMapFile::openAppend(StrView filename_)
{
	open(filename_, FileMode::OpenOrCreate, FileAccess::ReadWrite, FileSharerMode::Read);
	_fs.setPosFromEnd(0);
}

void MemMapFile::openWrite(StrView filename_, bool truncate_)
{
	open(filename_, FileMode::OpenOrCreate, FileAccess::ReadWrite, FileSharerMode::Read);
}

void MemMapFile::open(StrView filename_, FileMode mode_, FileAccess access_, FileSharerMode shareMode_)
{
	close();
	_fs.open(filename_, mode_, access_, shareMode_);

	if (_fs.getFileSize() >= SIZE_T_MAX)
		throw SGE_ERROR("memmap file size too larget");

	auto size = static_cast<size_t>(_fs.getFileSize());
	if (size <= 0)
		return;

	DWORD protect_flag = 0;
	DWORD access_flag = 0;

	switch (access_)
	{
		case FileAccess::Read:		protect_flag |= PAGE_READONLY;		break;
		case FileAccess::ReadWrite:	protect_flag |= PAGE_READWRITE;		break;
		case FileAccess::WriteOnly:	protect_flag |= PAGE_READWRITE;		break;
	}

	switch (access_)
	{
		case FileAccess::Read:		access_flag |= FILE_MAP_READ;					break;
		case FileAccess::ReadWrite:	access_flag |= FILE_MAP_READ | FILE_MAP_WRITE;	break;
		case FileAccess::WriteOnly:	access_flag |= FILE_MAP_WRITE;					break;
	}

	_mapping = ::CreateFileMapping(_fs.getNativeFd(), nullptr, protect_flag, 0, 0, nullptr);

	auto err = ::GetLastError();
	switch (err)
	{
		case ERROR_ACCESS_DENIED: {throw SGE_ERROR("ERROR_ACCESS_DENIED"); } break;
		case ERROR_ALREADY_EXISTS: throw SGE_ERROR("ERROR_ALREADY_EXISTS"); break;
	}

	if (!_mapping)
	{
		throw SGE_ERROR("memmap");
	}

	auto* data = reinterpret_cast<u8*>(::MapViewOfFile(_mapping, access_flag, 0, 0, 0));
	if (!data) {
		throw SGE_ERROR("memmap");
	}

	_span = Span<const u8>(data, size);
}

void MemMapFile::writeBytes(Span<const u8> data_)
{
	_fs.writeBytes(data_);
}

#if 0
void MemMapFile::open(StrView filename_)
{
	close();
	_fs.openRead(filename_);

	if (_fs.getFileSize() >= SIZE_T_MAX)
		throw SGE_ERROR("memmap file size too larget");

	auto size = static_cast<size_t>(_fs.getFileSize());
	if (size <= 0)
		return;

	_mapping = ::CreateFileMapping(_fs.getNativeFd(), nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (!_mapping)
	{
		throw SGE_ERROR("memmap");
	}

	auto* data = reinterpret_cast<u8*>(::MapViewOfFile(_mapping, FILE_MAP_READ, 0, 0, 0));
	if (!data) {
		throw SGE_ERROR("memmap");
	}

	_span = Span<const u8>(data, size);
}

#endif // 0

#else



#endif // SGE_OS_WINDOWS


}