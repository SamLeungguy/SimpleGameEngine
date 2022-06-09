#pragma once

namespace sge {

struct Directory
{
	Directory() = delete;

	static void	setCurrentDir(StrView dir_);
	static String getCurrentDir();

	static void create(StrView path_);
	static bool exists(StrView path_);

private:
	static void _create(StrView path_);
};

}