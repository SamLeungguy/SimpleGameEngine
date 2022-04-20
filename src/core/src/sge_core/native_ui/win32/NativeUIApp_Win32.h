#pragma once

#include <sge_core/base/sge_base.h>

#include "../base/NativeUIApp_Base.h"

#if SGE_OS_WINDOWS

namespace sge {

class NativeUIApp_Win32 : public NativeUIApp_Base
{
	using Base = NativeUIApp_Base;
public:

	virtual ~NativeUIApp_Win32() = default;

	MSG _win32_msg;

	//virtual void willQuit() {}

protected:
	virtual String	onGetExecutableFilename() override;
	virtual String	onGetCurrentDir() override;
	virtual void	onSetCurrentDir(StrView dir_) override;

	virtual void onCreate(CreateDesc& desc_) override;
	virtual void onRun() override;
	virtual void onQuit() override;
};

}
#endif