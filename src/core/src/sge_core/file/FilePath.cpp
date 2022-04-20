#include "FilePath.h"

namespace sge {

String FilePath::getDir(StrView path_)
{
	auto* pEnd = path_.end();
	auto* pBegin = path_.begin();

	if (pEnd == nullptr)
		return String();

	auto* p = pEnd - 1;				// "abc\0"
	for (; p >= pBegin; p--)
	{
		if (*p == '/' || *p == '\\')
		{
			return String(pBegin, p);
		}
	}

	return String();
}

}