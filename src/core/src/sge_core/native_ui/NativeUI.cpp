#include "NativeUI.h"

namespace sge {

	NativeUIApp* NativeUIApp::_instance = nullptr;

	NativeUIApp::NativeUIApp()
	{
		_instance = this;
	}
}