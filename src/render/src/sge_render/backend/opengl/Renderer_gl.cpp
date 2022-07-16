#include <sge_render-pch.h>

#include "Renderer_gl.h"
#include "RenderContext_gl.h"
#include "RenderGpuBuffer_gl.h"

#if SGE_RENDER_HAS_GL

namespace sge {

Renderer_gl::Renderer_gl(CreateDesc& desc_)
{
}

SPtr<RenderContext>	Renderer_gl::onCreateContext(RenderContext_CreateDesc& desc_)
{
	return new RenderContext_gl(desc_);
}

SPtr<RenderGpuBuffer> Renderer_gl::onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc_)
{
	return new RenderGpuBuffer_gl(desc_);
}

SPtr<Texture2D>	Renderer_gl::onCreateTexture2D(Texture2D_CreateDesc& desc_)
{
	return nullptr;
}

SPtr<Shader> Renderer_gl::onCreateShader(StrView filename_)
{
	return nullptr;
}

SPtr<Material> Renderer_gl::onCreateMaterial()
{
	return nullptr;
}

}

#endif