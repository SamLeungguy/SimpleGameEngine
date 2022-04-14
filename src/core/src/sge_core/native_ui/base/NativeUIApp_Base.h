#pragma once

namespace sge {

class NativeUIApp_Base : public NonCopyable
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