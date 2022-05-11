#if SGE_RENDER_HAS_GL

#include "Renderer_gl.h"
#include "RenderContext_gl.h"
#include "RenderGpuBuffer_gl.h"

namespace sge {

Renderer_gl::Renderer_gl(CreateDesc& desc_)
{
}

RenderContext* Renderer_gl::onCreateContext(RenderContext_CreateDesc& desc_)
{
	return new RenderContext_gl(desc_);
}

RenderGpuBuffer* Renderer_gl::onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc_)
{
	return new RenderGpuBuffer_gl(desc_);
}

RenderShader* Renderer_gl::onCreateShader(RenderShader_CreateDesc& desc_)
{
	return nullptr;
}

}

#endif