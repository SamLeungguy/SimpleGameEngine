#include <sge_editor.h>

#include <sge_render.h>

#include <fmt/chrono.h>

namespace sge {

class MainWin : public NativeUIWindow
{
	using Base = NativeUIWindow;
public:
	virtual void onCreate(CreateDesc& desc_) override;
	virtual void onCloseButton() override;

	virtual void onDraw() override;

	UPtr<RenderContext> _upRenderContext;

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

#pragma region MainWin_impl
void MainWin::onCreate(CreateDesc& desc_)
{
	Base::onCreate(desc_);

	RenderContext::CreateDesc renderContextDesc;
	renderContextDesc.pWindow = this;
	_upRenderContext.reset(RenderContext::create(renderContextDesc));
}
void MainWin::onCloseButton()
{
	NativeUIApp::current()->quit(0);
}

void MainWin::onDraw()
{
	Base::onDraw();
	if (_upRenderContext) {
		_upRenderContext->render();
	}
	drawNeeded();
}

#pragma endregion

#pragma region EditorApp_Imlpl
void EditorApp::onCreate(CreateDesc& desc_)
{
	{
		String file = getExecutableFilename();
		String path = FilePath::getDir(file);
		path.append("/../../../../../../examples/Test101");
		setCurrentDir(path);

		auto dir = getCurrentDir();
		SGE_LOG("dir = {}", dir);
	}

	Base::onCreate(desc_);

	Renderer::CreateDesc renderDesc;
	//renderDesc.apiType = Renderer::ApiType::OpenGL;
	Renderer::create(renderDesc);

	//--
	NativeUIWindow::CreateDesc winDesc;
	winDesc.isMainWindow = true;
	_mainWin.create(winDesc);
	winDesc.rect.w = s_temp_width;
	winDesc.rect.h = s_temp_height;
	_mainWin.setWindowTitle("SGE Editor");

}

#pragma endregion

}

int main() {
	sge::EditorApp app;
	sge::EditorApp::CreateDesc desc;

	app.run(desc);
}