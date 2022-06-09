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
	static void write(StrView filename_, Span<const u8> data_);
	static void read(StrView filename_, Vector<u8> data_);

};

}