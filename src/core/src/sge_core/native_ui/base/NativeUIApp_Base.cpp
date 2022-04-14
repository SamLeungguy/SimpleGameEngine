#include "NativeUIApp_Base.h"

namespace sge {

int NativeUIApp_Base::run(CreateDesc& desc_)
{
	onCreate(desc_);
	onRun();
	return _exitCode;
}

void NativeUIApp_Base::quit(int exitCode_)
{
	_exitCode = exitCode_;
	onQuit();
}

}
