#include "File.h"

#include <sge_core/log/Log.h>

#include "Directory.h"
#include "FilePath.h"
#include "FileStream.h"
#include "MemMapFile.h"

namespace sge {

void File::writeText(StrView filename_, StrView text_)
{
	writeBytes(filename_, ByteSpan_make(text_));
}

void File::writeBytes(StrView filename_, ByteSpan data_)
{
	FileStream fs;
	fs.openWrite(filename_, true);
	fs.writeBytes(data_);
}

char File::writeFile(StrView filename_, StrView data_, bool createDir_, bool logResult_)
{
	auto byteSpan = ByteSpan_make(data_);
	return writeFile(filename_, byteSpan, createDir_, logResult_);
}

char File::writeFile(StrView filename_, ByteSpan data_, bool createDir_, bool logResult_)
{
	char op = '+';

	auto realPath = FilePath::getRealPath(filename_);

	if (File::exists(realPath))
	{
		op = 'U';
	}

	if (logResult_)
	{
		SGE_LOG("[{}] {}, size={}", op, realPath, data_.size());
	}

	if (createDir_)
	{
		auto dir = FilePath::getDir(realPath);
		if (dir.size())
		{
			Directory::create(dir);
		}
	}

	writeBytes(realPath, data_);

	return op;
}

void File::readFile(StrView filename_, Vector<u8> data_)
{
	assert(0);
}

char File::writeFileIfChanged(StrView filename_, StrView data_, bool createDir_, bool logResult_, bool logNoChange_)
{
	auto byteSpan = ByteSpan_make(data_);
	return writeFileIfChanged(filename_, byteSpan, createDir_, logResult_, logNoChange_);
}

char File::writeFileIfChanged(StrView filename_, ByteSpan data_, bool createDir_, bool logResult_, bool logNoChange_)
{
	char op = '+';

	auto realPath = FilePath::getRealPath(filename_);

	if (exists(realPath)) {
		MemMapFile map;
		map.open(realPath);

		if (map.span() == data_) {
			op = '=';
		}
		else {
			op = 'U';
		}
	}

	if (logResult_) {
		if (op != '=' || logNoChange_) {
			SGE_LOG("[{}] {}, size={}", op, realPath, data_.size());
		}
	}

	if (op == '=') return op;
	return writeFile(realPath, data_, createDir_, false);
}

#if SGE_OS_WINDOWS
#if 0
#pragma mark ================= Windows ====================
#endif

bool File::exists(StrView filename_)
{
	TempStringW filenameW;
	UtfUtil::convert(filenameW, filename_);
	DWORD dwAttrib = ::GetFileAttributes(filenameW.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void File::rename(StrView src_, StrView dst_)
{
	TempStringW srcW, dstW;
	UtfUtil::convert(srcW, src_);
	UtfUtil::convert(dstW, dst_);

	if (0 != ::_wrename(srcW.c_str(), dstW.c_str())) {
		throw SGE_ERROR("rename file {}->{}", src_, dst_);
	}
}

#else

bool File::exists(StrView filename_)
{
	TempStringA filenameA;
	UtfUtil::convert(filenameA, filename_);
	struct stat s;
	if (0 != ::stat(filenameA.c_str(), &s)) return false;
	return (s.st_mode & S_IFDIR) == 0;
}

void File::rename(StrView src_, StrView dst_)
{
	TempStringA srcA, dstA;
	UtfUtil::convert(srcA, src_);
	UtfUtil::convert(dstA, dst_);

	auto ret = ::rename(srcA.c_str(), dstA.c_str());
	if (ret != 0) {
		throw SGE_ERROR("rename file {}->{}", src, dst);
	}
}

#endif

}