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
	RenderMesh	_renderMesh2;

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

		/*editMesh.colors.emplace_back(255, 0, 0, 255);
		editMesh.colors.emplace_back(0, 255, 0, 255);
		editMesh.colors.emplace_back(0, 0, 255, 255);*/

		editMesh.uvs[0].emplace_back(0.0f, 0.0f);
		editMesh.uvs[0].emplace_back(0.5f, 0.0f);
		editMesh.uvs[0].emplace_back(-0.5f,0.0f);

		editMesh.normals.emplace_back(0.0f, 0.5f, 0.0f);
		editMesh.normals.emplace_back(0.5f, -0.5f, 0.0f);
		editMesh.normals.emplace_back(-0.5f, -0.5f, 0.0f);

		editMesh.indices.emplace_back(0);
		editMesh.indices.emplace_back(1);
		editMesh.indices.emplace_back(2);

		WavefrontObjLoader loader;
		//loader.load(editMesh, "Assets/Meshes/sphere.obj");
		//loader.load(editMesh, "Assets/Meshes/pentagon.obj");
		//loader.load(editMesh, "Assets/Meshes/monkey.obj");
		//loader.load(editMesh, "Assets/Meshes/cube.obj");

		WavefrontObjLoader::loadFile(editMesh, "Assets/Meshes/ico_sphere.obj");
		_renderMesh.create(editMesh);

		WavefrontObjLoader::loadFile(editMesh, "Assets/Meshes/monkey.obj");
		_renderMesh2.create(editMesh);

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

#if 0
	if (_renderMesh.getVertexCount() > 0)
	{
		_cmdBuf.drawMesh(SGE_LOC, _renderMesh);
	}
	else if (_renderMesh2.getVertexCount() > 0)
	{
		_cmdBuf.drawMesh(SGE_LOC, _renderMesh2);
	}
#else
	static u64 frame = 0;
	if (frame % 40 > 20)
	{
		_cmdBuf.drawMesh(SGE_LOC, _renderMesh);
	}
	else
	{
		_cmdBuf.drawMesh(SGE_LOC, _renderMesh2);
	}
	frame++;
#endif // 0

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