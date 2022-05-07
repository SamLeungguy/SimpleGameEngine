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

#else



#endif // SGE_OS_WINDOWS


}