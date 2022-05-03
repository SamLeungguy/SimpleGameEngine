#include <sge_editor.h>

#include <sge_render.h>

#include <fmt/chrono.h>

#define draw_triangle 0

namespace sge {

class MainWin : public NativeUIWindow
{
	using Base = NativeUIWindow;
public:
	virtual void onCreate(CreateDesc& desc_) override;
	virtual void onCloseButton() override;

	virtual void onDraw() override;

	SPtr<RenderContext> _spRenderContext;
	RenderCommandBuffer _cmdBuf;
	RenderMesh	_renderMesh;

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
	SGE_DUMP_VAR(sizeof(Vertex_Pos));
	SGE_DUMP_VAR(sizeof(Vertex_PosColor));
	//		SGE_DUMP_VAR(sizeof(Vertex_PosColorUv));
	//		SGE_DUMP_VAR(sizeof(Vertex_PosColorUv2));

	Base::onCreate(desc_);

	RenderContext::CreateDesc renderContextDesc;
	renderContextDesc.pWindow = this;
	_spRenderContext.reset(Renderer::current()->createContext(renderContextDesc));

	{
		EditMesh editMesh;

		editMesh.positions.emplace_back(0.0f, 0.5f, 0.0f);
		editMesh.positions.emplace_back(0.5f, -0.5f, 0.0f);
		editMesh.positions.emplace_back(-0.5f, -0.5f, 0.0f);

		editMesh.colors.emplace_back(255, 0, 0, 255);
		editMesh.colors.emplace_back(0, 255, 0, 255);
		editMesh.colors.emplace_back(0, 0, 255, 255);

		_renderMesh.create(editMesh);

		VertexLayoutManager::current()->getLayout(Vertex_Pos::s_type);
	}
}
void MainWin::onCloseButton()
{
	NativeUIApp::current()->quit(0);
}

void MainWin::onDraw()
{
	Base::onDraw();
	//if (!_spRenderContext)
	//	return;

	_spRenderContext->setFrameBufferSize(clientRect().size);

	_spRenderContext->beginRender();

	_cmdBuf.reset();
	_cmdBuf.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });
	_cmdBuf.drawMesh(_renderMesh);
	_cmdBuf.swapBuffers();

	_spRenderContext->commit(_cmdBuf);

	_spRenderContext->endRender();
	
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
	winDesc.rect.w = 800;
	winDesc.rect.h = 600;
	_mainWin.setWindowTitle("SGE Editor");
}

#pragma endregion

}

int main() {
	sge::EditorApp app;
	sge::EditorApp::CreateDesc desc;

	app.run(desc);
}