#include "RenderContext.h"
#include "Renderer.h"

namespace sge {

RenderContext* RenderContext::create(CreateDesc& desc_)
{
	return Renderer::current()->onCreateContext(desc_);
}

RenderContext::RenderContext(CreateDesc desc_)
{
}

void RenderContext::render()
{
	onBeginRender();
	onClearColorAndDepthBuffer();

	onTestDraw();

	Renderer::current()->getRenderCommnadBuffer()->execute();

	onSwapBuffers();
	onEndRender();
}


}