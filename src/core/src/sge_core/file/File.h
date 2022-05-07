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

}