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
	static Renderer_gl* instance();

	Renderer_gl(CreateDesc& desc_);
	virtual ~Renderer_gl() = default;

protected:
	virtual SPtr<RenderContext>		onCreateContext		(RenderContext_CreateDesc& desc_)	override;
	virtual SPtr<RenderGpuBuffer>	onCreateGpuBuffer	(RenderGpuBuffer_CreateDesc& desc_)	override;
	virtual SPtr<Shader>			onCreateShader		(StrView filename_)					override;
	virtual SPtr<Material>			onCreateMaterial	()									override;

private:

};

inline Renderer_gl* Renderer_gl::instance() { return static_cast<Renderer_gl*>(s_pInstance); }

}

#endif