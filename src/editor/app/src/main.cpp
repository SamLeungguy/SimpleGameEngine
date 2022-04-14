#include <sge_editor.h>

#include <sge_render.h>

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

	virtual void onRun() override;

private:
	MainWin _mainWin;

	UPtr<GraphicsContext> _upGraphicsContext;
};

#pragma region MainWin_impl
void MainWin::onCloseButton()
{
	NativeUIApp::current()->quit(0);
}

#pragma endregion

#pragma region EditorApp_Imlpl
void EditorApp::onCreate(CreateDesc& desc_)
{
	Base::onCreate(desc_);

	NativeUIWindow::CreateDesc winDesc;
	winDesc.isMainWindow = true;
	_mainWin.create(winDesc);
	winDesc.rect.w = s_temp_width;
	winDesc.rect.h = s_temp_height;
	_mainWin.setWindowTitle("SGE Editor");

	_upGraphicsContext = eastl::make_unique<GraphicsContext>();
	_upGraphicsContext->init(_mainWin._hwnd);
}

void EditorApp::onRun()
{
	_upGraphicsContext->swapBuffers();

	while (GetMessage(&_win32_msg, NULL, 0, 0)) {
		TranslateMessage(&_win32_msg);
		DispatchMessage(&_win32_msg);
	}

	willQuit();
}

#pragma endregion

}

int main() {
	sge::EditorApp app;
	sge::EditorApp::CreateDesc desc;

	app.run(desc);
}