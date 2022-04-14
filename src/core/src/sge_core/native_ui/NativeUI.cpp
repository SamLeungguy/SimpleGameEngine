#include "NativeUI.h"

namespace sge {

	NativeUIApp* NativeUIApp::_current = nullptr;

	NativeUIApp::NativeUIApp()
	{
		_current = this;
	}
}