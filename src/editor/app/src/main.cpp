#include <sge_editor.h>

#include <fmt/chrono.h>

namespace sge {

class MainWin : public NativeUIWindow
{
public:
	virtual void onCloseButton() override;
private:
};

class EditorApp : public NativeUIApp
{
	using Base = NativeUIApp;
public:
	virtual void onCreate(CreateDesc& desc_) override;
private:
	MainWin _mainWin;
};

void MainWin::onCloseButton()
{
	NativeUIApp::current()->quit(0);
}

void EditorApp::onCreate(CreateDesc& desc_)
{
	Base::onCreate(desc_);

	NativeUIWindow::CreateDesc winDesc;
	winDesc.isMainWindow = true;
	_mainWin.create(winDesc);
	_mainWin.setWindowTitle("SGE Editor");
}

}

int main() {
	sge::EditorApp app;
	sge::EditorApp::CreateDesc desc;

	app.run(desc);
}