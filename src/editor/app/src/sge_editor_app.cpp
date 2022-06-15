#include <sge_editor.h>

#include <sge_render/mesh/RenderMesh.h>
#include <sge_render/mesh/WavefrontObjLoader.h>
#include <sge_render/command/RenderCommand.h>
#include <sge_render/vertex/Vertex.h>
#include <sge_render/vertex/VertexLayoutManager.h>

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
	SPtr<Material> _material;

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

void MainWin::onCreate(CreateDesc& desc) {
	SGE_DUMP_VAR(sizeof(Vertex_Pos));
	SGE_DUMP_VAR(sizeof(Vertex_PosColor));
	//		SGE_DUMP_VAR(sizeof(Vertex_PosColorUv));
	//		SGE_DUMP_VAR(sizeof(Vertex_PosColorUv2));

	Base::onCreate(desc);
	auto* renderer = Renderer::instance();

	{
		RenderContext::CreateDesc renderContextDesc;
		renderContextDesc.pWindow = this;
		_spRenderContext = renderer->createContext(renderContextDesc);
	}

	auto shader = renderer->createShader("Assets/Shaders/test.shader");
	_material = renderer->createMaterial();
	_material->setShader(shader);

	EditMesh editMesh;

#if 1
	WavefrontObjLoader::loadFile(editMesh, "Assets/Mesh/test.obj");
	// the current shader need color
	for (size_t i = editMesh.colors.size(); i < editMesh.positions.size(); i++) {
		editMesh.colors.emplace_back(255, 255, 255, 255);
	}

	// the current shader has no uv or normal
	editMesh.uvs[0].clear();
	editMesh.normals.clear();

#else
	editMesh.pos.emplace_back( 0.0f,  0.5f, 0.0f);
	editMesh.pos.emplace_back( 0.5f, -0.5f, 0.0f);
	editMesh.pos.emplace_back(-0.5f, -0.5f, 0.0f);

	editMesh.color.emplace_back(255, 0, 0, 255);
	editMesh.color.emplace_back(0, 255, 0, 255);
	editMesh.color.emplace_back(0, 0, 255, 255);
#endif

	_renderMesh.create(editMesh);

	VertexLayoutManager::instance()->getLayout(Vertex_Pos::kType);
}
void MainWin::onCloseButton()
{
	NativeUIApp::current()->quit(0);
}

void MainWin::onDraw()
{
	Base::onDraw();
	if (!_spRenderContext) return;

	auto time = GetTickCount() * 0.001f;
	auto s = abs(sin(time * 2));

	_material->setParam("test_float", s * 0.5f);
	_material->setParam("test_color", Color4f(s, 0, 0, 1));

	_spRenderContext->setFrameBufferSize(clientRect().size);

	_spRenderContext->beginRender();

	_cmdBuf.reset();
	_cmdBuf.clearFrameBuffers()->setColor({0, 0, 0.2f, 1});
	_cmdBuf.drawMesh(SGE_LOC, _renderMesh, _material);
	_cmdBuf.swapBuffers();

	_spRenderContext->commit(_cmdBuf);

	_spRenderContext->endRender();
	drawNeeded();
}

void EditorApp::onCreate(CreateDesc& desc_)
{
	{
		String file = getExecutableFilename();
		String path = FilePath::getDir(file);
		path.append("/../../../../../../examples/Test101");
		Directory::setCurrentDir(path);

		auto dir = Directory::getCurrentDir();
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

}

int main() {
	sge::EditorApp app;
	sge::EditorApp::CreateDesc desc;

	app.run(desc);
}