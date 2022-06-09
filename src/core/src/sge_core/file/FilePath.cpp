#include "FilePath.h"
#include "Directory.h"
#include <sge_core/string/StringUtil.h>

namespace sge {

StrView FilePath::getDir(StrView path) {
	auto* end = path.end();
	auto* begin = path.begin();

	if (end == nullptr) return StrView();
	auto* p = end - 1;
	for (; p >= begin; p--) {
		if (*p == '/' || *p == '\\') {
			return StrView(begin, p - begin);
		}
	}

	return StrView();
}

bool FilePath::isRealpath(const StrView& path_)
{
	if (path_.size() < 1)	return false;
	if (path_[0] == '/')	return true;

	if (path_.size() < 2)	return false;

	if (std::isalpha(path_[0]) && path_[1] == ':')	return true;

	return false;
}

String FilePath::getRealPath(StrView path_)
{
	if (!path_.size()) return String();

	String outStr;

	bool needSlash = false;

	if (isRealpath(path_))
	{
		needSlash = (path_[0] == '/');
	}
	else
	{
		outStr = Directory::getCurrentDir();
		needSlash = true;
	}

	StrView p = path_;
	while (p.size())
	{
		auto s = StringUtil::splitByChar(p, "\\/");
		if (s.first == ".")
		{
			// skip '.'
		}
		else if (!s.first.size())
		{
			// skip '/'
		}
		else if (s.first == "..")
		{
			auto* parent = StringUtil::findCharFromEnd(outStr, "\\/", false);
			if (parent == nullptr)
			{
				outStr.clear(); // no more parent folder
				return String();
			}

			outStr.resize(parent - outStr.data());
		}
		else
		{
			if (needSlash)
			{
				outStr += '/';
			}
			outStr += s.first;
			needSlash = true;
		}
		p = s.second;
	}

	return outStr;
}

}