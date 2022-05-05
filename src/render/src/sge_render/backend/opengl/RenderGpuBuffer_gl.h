#pragma once

#if SGE_RENDER_HAS_GL

#include <sge_render/buffer/RenderGpuBuffer.h>
#include "Render_Common_gl.h"

namespace sge {

class RenderGpuBuffer_gl : public RenderGpuBuffer
{
	using Base = RenderGpuBuffer;
	using Util = GLUtil;
public:
	RenderGpuBuffer_gl(CreateDesc desc_);
	virtual ~RenderGpuBuffer_gl();

	GLuint getRendererID();
	GLuint getRendererID() const;

protected:
	virtual void onUploadToGpu(Span<const u8> data_, size_t offset_) override;

private:
	GLuint _rendererID = 0;
};

inline GLuint RenderGpuBuffer_gl::getRendererID()			{ return _rendererID; }
inline GLuint RenderGpuBuffer_gl::getRendererID() const		{ return _rendererID; }

}

#endif