#include "NativeUIApp_Win32.h"

namespace sge {

void NativeUIApp_Win32::onCreate(CreateDesc& desc_)
{
	Base::onCreate(desc_);
}

void NativeUIApp_Win32::onRun()
{
	Base::onRun();

	while (GetMessage(&_win32_msg, NULL, 0, 0)) {
		TranslateMessage(&_win32_msg);
		DispatchMessage(&_win32_msg);
	}

	willQuit();
}

void NativeUIApp_Win32::onQuit()
{
	Base::onQuit();

	::PostQuitMessage(_exitCode);
}

}
