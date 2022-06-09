#include "Directory.h"
#include "FilePath.h"

#include <sge_core/string/UtfUtil.h>

namespace sge {

void Directory::create(StrView path_)
{
	if (exists(path_)) return;

	auto parent = FilePath::getDir(path_);
	if (parent.size())
	{
		create(parent);
	}

	_create(path_);
}

#if SGE_OS_WINDOWS
#if 0
#pragma mark ================= Windows ====================
#endif

void Directory::setCurrentDir(StrView dir_)
{
	TempStringW tmp = UtfUtil::toStringW(dir_);
	::SetCurrentDirectory(tmp.c_str());
}

String Directory::getCurrentDir()
{
	wchar_t tmp[MAX_PATH + 1];
	if (!::GetCurrentDirectory(MAX_PATH, tmp))
		throw SGE_ERROR("getCurrentDir");

	String o = UtfUtil::toString(tmp);
	return o;
}

bool Directory::exists(StrView path_)
{
	TempStringW pathW;
	UtfUtil::convert(pathW, path_);

	DWORD dwAttrib = ::GetFileAttributes(pathW.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void Directory::_create(StrView path_) {
	TempStringW pathW;
	UtfUtil::convert(pathW, path_);
	auto ret = ::CreateDirectory(pathW.c_str(), nullptr);
	if (!ret) throw SGE_ERROR("create directory {}", ""/*pathW*/);
}

#else

#if 0
#pragma mark ================= Unix ====================
#endif

bool Directory::exists(StrView path) {
	TempStringA pathA;
	UtfUtil::convert(pathA, path);

	struct stat s;
	if (0 != ::stat(pathA.c_str(), &s)) return false;
	return (s.st_mode & S_IFDIR) != 0;
}

void Directory::_create(StrView path) {
	TempStringW pathA;
	UtfUtil::convert(pathA, path);
	auto ret = ::mkdir(pathA.c_str(), 0755);
	if (ret != 0) throw SGE_ERROR("create directory {}", pathA);
}

#endif // SGE_OS_WINDOWS


}