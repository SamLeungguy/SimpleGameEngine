#pragma once

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

	static void readFile(StrView filename_, Vector<u8> data_);

	static char writeFileIfChanged(StrView filename_, ByteSpan data_, bool createDir_, bool logResult_ = true, bool logNoChange_ = false);
	static char writeFileIfChanged(StrView filename_, StrView  data_, bool createDir_, bool logResult_ = true, bool logNoChange_ = false);
};

}