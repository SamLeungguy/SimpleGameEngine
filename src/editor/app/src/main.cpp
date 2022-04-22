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

	UPtr<RenderContext> _upRenderContext;

	// test
	UPtr<RenderMesh>	_upRenderMesh;
	UPtr<Shader>		_upShader;
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
	if (!_upRenderContext)
		return;

	Rect2i rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = 800;
	rect.h = 600;

	if (_upRenderMesh)
	{
		RenderCommandBuffer::setViewport(rect);
		RenderCommandBuffer::drawIndex(_upRenderMesh.get(), _upShader.get());
	}
	
	/*for (size_t i = 0; i < 100; i++)
	{
		Rect2i rect;
		rect.x = 0;
		rect.y = 1;
		rect.w = 2;
		rect.h = 3;

		RenderCommandBuffer::drawIndex(12);
		RenderCommandBuffer::setViewport(rect);
		RenderCommandBuffer::drawIndex(24);
	}*/

	_upRenderContext->render();

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

	// test
	{
		Mesh mesh;
#if draw_triangle
		u64 vertexCount = 3;
#else
		u64 vertexCount = 4;
#endif
		Vector<Tuple3f> positions;
		positions.reserve(vertexCount);
#if draw_triangle
		positions.push_back({ 0.0f,   0.5f, 0.0f });
		positions.push_back({ 0.5f,  -0.5f, 0.0f });
		positions.push_back({ -0.5f, -0.5f, 0.0f });
#else
		positions.push_back({ 0.5f,  0.5f, 0.0f });
		positions.push_back({ 0.5f, -0.5f, 0.0f });
		positions.push_back({ -0.5f, 0.5f, 0.0f });
		positions.push_back({ -0.5f, -0.5f, 0.0f });
#endif
		mesh.setPositions(positions);

		Vector<Tuple2f> uv0;
		uv0.reserve(vertexCount);
#if draw_triangle
		uv0.push_back({ 0.0f,   1.0f});
		uv0.push_back({ 0.5f,  -0.5f});
		uv0.push_back({ -0.5f, -0.5f});
#else
		uv0.push_back({ 1.0f, 1.0f });
		uv0.push_back({ 1.0f, 0.0f });
		uv0.push_back({ 0.0f, 1.0f });
		uv0.push_back({ 0.0f, 0.0f });
#endif
		mesh.setUVs(uv0, 0);

		Vector<Color4f> colors;
		colors.reserve(vertexCount);
#if draw_triangle
		colors.push_back({ 1.0f, 0.0f, 0.0f, 1.0f });
		colors.push_back({ 0.0f, 1.0f, 0.0f, 1.0f });
		colors.push_back({ 0.0f, 0.0f, 1.0f, 1.0f });
#else
		colors.push_back({ 1.0f, 0.0f, 0.0f, 1.0f });
		colors.push_back({ 0.0f, 1.0f, 0.0f, 1.0f });
		colors.push_back({ 0.0f, 0.0f, 1.0f, 1.0f });
		colors.push_back({ 1.0f, 1.0f, 1.0f, 1.0f });
#endif
		mesh.setColors(colors);

		Vector<u32> indices;
		indices.reserve(vertexCount);
#if draw_triangle
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
#else
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(3);
		indices.push_back(2);
		indices.push_back(1);
#endif
		mesh.setIndices(indices);

		_mainWin._upRenderMesh.reset(RenderMesh::create(mesh));

		Shader_CreateDesc shaderCD;
		shaderCD.filepath = L"Assets/Shaders/test2.hlsl";
		shaderCD.pVertexLayout = &_mainWin._upRenderMesh->vertexLayout;
		_mainWin._upShader.reset(Shader::create(shaderCD));
	}
}

#pragma endregion

}

int main() {
	sge::EditorApp app;
	sge::EditorApp::CreateDesc desc;

	app.run(desc);
}