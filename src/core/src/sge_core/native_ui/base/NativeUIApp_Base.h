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

	void setCurrentDir(StrView dir_) { onSetCurrentDir(dir_); }
	String getCurrentDir() { return onGetCurrentDir(); }

	String getExecutableFilename() { return onGetExecutableFilename(); }

protected:
	virtual String onGetCurrentDir()				= 0;
	virtual String onGetExecutableFilename()		= 0;
	virtual void onSetCurrentDir(StrView dir_)	= 0;

	virtual void onCreate(CreateDesc& desc_) {}
	virtual void onRun() {}
	virtual void onQuit() {}

	int _exitCode = 0;
};

}