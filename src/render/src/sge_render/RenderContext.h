#pragma once

namespace sge {

static constexpr int s_temp_width = 800;
static constexpr int s_temp_height = 600;

struct RenderContext_CreateDesc 
{
	NativeUIWindow* pWindow = nullptr;
};

class RenderContext : public NonCopyable
{
public:
	using CreateDesc = RenderContext_CreateDesc;

	static RenderContext* create(CreateDesc& desc_);

	RenderContext(CreateDesc& desc_);
	virtual ~RenderContext() = default;

	void render();
	
protected:
	virtual void onBeginRender() {};
	virtual void onTestDraw() {};
	virtual void onEndRender() {};
	virtual void onClearColorAndDepthBuffer() {}
	virtual void onSwapBuffers() {}
};

}