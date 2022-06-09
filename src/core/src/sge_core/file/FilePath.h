#pragma once

namespace sge {

struct FilePath
{
	static StrView	getDir(StrView path_);

	static bool		isRealpath(const StrView& path_);
	static String	getRealPath(StrView path_);
};

}