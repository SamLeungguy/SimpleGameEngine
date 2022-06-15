#pragma once

#include <sge_core/app/AppBase.h>

namespace sge {

class NativeUIApp_Base : public AppBase
{
public:
	class CreateDesc
	{

	};
	virtual ~NativeUIApp_Base() = default;

	int run(CreateDesc& desc_);
	void quit(int exitCode_);

	virtual void willQuit() {}

protected:

	virtual void onCreate(CreateDesc& desc_) {}
	virtual void onRun() {}
	virtual void onQuit() {}

	int _exitCode = 0;
};

}