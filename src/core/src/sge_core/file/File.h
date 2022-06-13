#pragma once

#include "../string/UtfUtil.h"

namespace sge {

using FileSize = u64;

enum FileMode
{
	CreateNew,
	OpenExists,
	OpenOrCreate,
};

enum class FileAccess
{
	Read,
	ReadWrite,
	WriteOnly,
};

enum class FileSharerMode
{
	None,
	Read,
	Write,
	ReadWrite,
};

struct File
{
	static bool exists(StrView filename_);
	static void rename(StrView src_, StrView dst_);

	static void writeText(StrView filename_, StrView text_);
	static void writeBytes(StrView filename_, ByteSpan data_);

	static char writeFile(StrView filename_, StrView  data_, bool createDir_, bool logResult_);
	static char writeFile(StrView filename_, ByteSpan data_, bool createDir_, bool logResult_ = true);

						static void readFile(StrView filename, Vector<u8>& outData)		{ _readFile(filename, outData); }
	template<size_t N>	static void readFile(StrView filename, Vector_<u8, N>& outData) { _readFile(filename, outData); }
	template<size_t N>	static void readFile(StrView filename, String_<N>& outData)		{ _readFile(filename, outData); }

	static char writeFileIfChanged(StrView filename_, ByteSpan data_, bool createDir_, bool logResult_ = true, bool logNoChange_ = false);
	static char writeFileIfChanged(StrView filename_, StrView  data_, bool createDir_, bool logResult_ = true, bool logNoChange_ = false);

private:
	template<class T> static void _readFile(StrView filename, T& outData);
};

template<class T> inline
void sge::File::_readFile(StrView filename, T& outData) {
	FileStream fs;
	fs.openRead(filename);
	auto size = fs.getFileSize();
	if (size > eastl::numeric_limits<size_t>::max())
		throw SGE_ERROR("file is too large");
	outData.resize(size);

	Span<u8> span(reinterpret_cast<u8*>(outData.data()), outData.size());
	fs.readBytes(span);
}

}