#pragma once

#if SGE_RENDER_HAS_GL

#include <sge_render/Renderer.h>
#include "Render_Common_gl.h"

namespace sge {

class Renderer_gl : public Renderer
{
	using Base = Renderer;
	using Util = GLUtil;
public:
	static Renderer_gl* current();

	Renderer_gl(CreateDesc& desc_);
	virtual ~Renderer_gl() = default;

protected:
	virtual RenderContext* onCreateContext(RenderContext_CreateDesc& desc_) override;
	virtual RenderGpuBuffer* onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc_) override;
	virtual RenderShader* onCreateShader(RenderShader_CreateDesc& desc_) override;
	virtual Material* onCreateMaterial(Material_CreateDesc& desc_) override;

private:

};

inline Renderer_gl* Renderer_gl::current() { return static_cast<Renderer_gl*>(_pCurrent); }

}

#endif