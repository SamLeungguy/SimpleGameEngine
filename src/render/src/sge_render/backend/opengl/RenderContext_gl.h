#pragma once

#if SGE_RENDER_HAS_GL

#include "Render_Common_gl.h"
#include <sge_render/RenderContext.h>

namespace sge {

class Renderer_gl;

class RenderContext_gl : public RenderContext
{
	using Base = RenderContext;
	using Util = GLUtil;
public:
	RenderContext_gl(CreateDesc desc_);
	virtual ~RenderContext_gl();

	void onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers & cmd_);
	void onCmd_SwapBuffers(RenderCommand_SwapBuffers & cmd_);
	void onCmd_DrawCall(RenderCommand_DrawCall & cmd_);

protected:
	void _onInit();

	virtual void onBeginRender() override;
	virtual void onEndRender() override;
	virtual void onSetFrameBufferSize(Vec2f newSize_) override;
	virtual void onCommit(RenderCommandBuffer & cmdBuf_) override;


	void _setTestShaders();
	u32 _getTestInputLayout(const VertexLayout * pVertexLayout_);

protected:
	Renderer_gl* _pRenderer = nullptr;
	
	HDC		_hdc = nullptr;
	HGLRC	_hrc = nullptr;

	GLuint _testFrameBufferID = 0;
	GLuint _testShaderID = 0;

	VectorMap<const VertexLayout*, GLuint> _testInputLayouts;
};

}

#endif