#pragma once

#include <sge_core/base/sge_base.h>

#include "../base/NativeUIWindow_Base.h"


#if SGE_OS_WINDOWS
namespace sge {

class NativeUIWindow_Win32 : public NativeUIWindow_Base
{
	using This = NativeUIWindow_Win32;
	using Base = NativeUIWindow_Base;
public:

	virtual ~NativeUIWindow_Win32() = default;

	virtual void onCreate(CreateDesc& desc_) override;
	virtual void onSetWindowTitleBar(StrView title_) override;

	HWND _hwnd;

private:
	static LRESULT WINAPI s_wndProc(HWND hwnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_);

	static This* s_getThis(HWND hwnd_);

	LRESULT _handleWin32Event(HWND hwnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_);
};

SGE_INLINE NativeUIWindow_Win32* NativeUIWindow_Win32::s_getThis(HWND hwnd_) { return reinterpret_cast<NativeUIWindow_Win32*>(::GetWindowLongPtr(hwnd_, GWLP_USERDATA)); }

}
#endif // SGE_OS_WINDOWS
