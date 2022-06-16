#pragma once

#include <sge_core/base/sge_base.h>

#if SGE_OS_WINDOWS

#include "sge_core/native_ui/win32/NativeUIApp_Win32.h"
#include "sge_core/native_ui/win32/NativeUIWindow_Win32.h"

namespace sge {
	using NativeUIApp_Impl = sge::NativeUIApp_Win32;
	using NativeUIWindow_Impl = sge::NativeUIWindow_Win32;
}

#else
	#error "unsupported platform"

#endif

namespace sge {

class NativeUIWindow : public NativeUIWindow_Impl
{
public:
	NativeUIWindow() = default;
	virtual ~NativeUIWindow() = default;
private:
};

class NativeUIApp : public NativeUIApp_Impl
{
public:
	NativeUIApp();
	virtual ~NativeUIApp() = default;

	static NativeUIApp* instance();

private:
	static NativeUIApp* _instance;
};

SGE_INLINE NativeUIApp* NativeUIApp::instance() { return NativeUIApp::_instance; }

}
