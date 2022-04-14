#include "NativeUIWindow_Win32.h"

#include "sge_core/base/Error.h"

#include <sge_core/string/UtfUtil.h>

#if	SGE_OS_WINDOWS
namespace sge {

	void NativeUIWindow_Win32::onCreate(CreateDesc& desc_)
	{
		Base::onCreate(desc_);

		const wchar_t* className = L"NativeUIWindow";

		auto hInstance = ::GetModuleHandle(nullptr);
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW; // | CS_DROPSHADOW;
		wc.lpfnWndProc = &s_wndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
		wc.hCursor = LoadCursor(hInstance, IDC_ARROW);
		wc.hbrBackground = nullptr; //(HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = className;
		wc.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

		if (!desc_.closeButton) {
			wc.style |= CS_NOCLOSE;
		}

		DWORD dwStyle = 0;
		DWORD dwExStyle = WS_EX_ACCEPTFILES;
		if (desc_.alwaysOnTop) dwExStyle |= WS_EX_TOPMOST;

		switch (desc_.type) {
			case CreateDesc::Type::ToolWindow:
			case CreateDesc::Type::NormalWindow: {
				dwStyle |= WS_OVERLAPPED | WS_SYSMENU;

				if (desc_.closeButton)	dwStyle	|= WS_SYSMENU;
				if (desc_.resizable)	dwStyle	|= WS_THICKFRAME;
				if (desc_.titleBar)		dwStyle	|= WS_CAPTION;
				if (desc_.minButton)	dwStyle	|= WS_MINIMIZEBOX;
				if (desc_.maxButton)	dwStyle	|= WS_MAXIMIZEBOX;
			}break;

			case CreateDesc::Type::PopupWindow: {
				dwStyle |= WS_POPUP | WS_BORDER;
			}break;
			default: SGE_ASSERT(false); break;
		}

		if (desc_.type == CreateDesc::Type::ToolWindow) {
			dwExStyle |= WS_EX_TOOLWINDOW;
		}

		WNDCLASSEX tmpWc;
		bool registered = (0 != ::GetClassInfoEx(hInstance, className, &tmpWc));
		if (!registered) {
			if (!::RegisterClassEx(&wc)) {
				throw SGE_ERROR("error RegisterClassEx");
			}
		}

		auto rect = desc_.rect;
		if (desc_.centerToScreen) {
			auto screenSize = Vec2f((float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN));
			rect.pos = (screenSize - rect.size) / 2;
		}

		_hwnd = ::CreateWindowExW(dwExStyle, className, className, dwStyle,
			(int)desc_.rect.x,
			(int)desc_.rect.y,
			(int)desc_.rect.w,
			(int)desc_.rect.h,
			nullptr, nullptr, hInstance, this);

		if (!_hwnd) {
			throw SGE_ERROR("cannot create native window");
		}

		ShowWindow(_hwnd, SW_SHOW);
	}

	void NativeUIWindow_Win32::onSetWindowTitleBar(StrView title_)
	{
		if (!_hwnd) return;
		TempStringW tmp;
		UtfUtil::convert(tmp, title_);
		::SetWindowText(_hwnd, tmp.c_str());
	}

	LRESULT WINAPI NativeUIWindow_Win32::s_wndProc(HWND hwnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_)
	{
		switch (msg_) 
		{
			case WM_CREATE: {
				auto cs = reinterpret_cast<CREATESTRUCT*>(lParam_);
				auto* thisObj = static_cast<This*>(cs->lpCreateParams);
				thisObj->_hwnd = hwnd_;
				::SetWindowLongPtr(hwnd_, GWLP_USERDATA, (LONG_PTR)thisObj);
			}break;

			case WM_DESTROY: {
				if (auto* thisObj = s_getThis(hwnd_)) {
					::SetWindowLongPtr(hwnd_, GWLP_USERDATA, (LONG_PTR)nullptr);
					thisObj->_hwnd = nullptr;
					sge_delete(thisObj);
				}
			}break;

			case WM_CLOSE: {
				if (auto* thisObj = s_getThis(hwnd_)) {
					thisObj->onCloseButton();
					return 0;
				}
			}break;

			case WM_ACTIVATE: {
				if (auto* thisObj = s_getThis(hwnd_)) {
					u16 a = LOWORD(wParam_);
					switch (a) {
					case WA_ACTIVE:		thisObj->onActive(true);  break;
					case WA_CLICKACTIVE:thisObj->onActive(true);  break;
					case WA_INACTIVE:	thisObj->onActive(false); break;
					}
				}
			}break;

			default: {
				if (auto* thisObj = s_getThis(hwnd_)) {
					return thisObj->_handleWin32Event(hwnd_, msg_, wParam_, lParam_);
				}
			}break;
		}
		return ::DefWindowProc(hwnd_, msg_, wParam_, lParam_);
	}

	LRESULT NativeUIWindow_Win32::_handleWin32Event(HWND hwnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_)
	{
		return ::DefWindowProc(hwnd_, msg_, wParam_, lParam_);
	}
}

#endif