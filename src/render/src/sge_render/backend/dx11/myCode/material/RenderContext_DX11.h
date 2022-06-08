#if 0
#pragma once

#if SGE_RENDER_HAS_DX11

#include "Render_Common_DX11.h"
#include "../../RenderContext.h"

#include <sge_render/command/RenderCommand.h>

namespace sge {

class RenderContext_DX11 : public RenderContext
{
	using Base = RenderContext;
	using Util = DX11Util;
public:
	RenderContext_DX11(CreateDesc desc_);
	virtual ~RenderContext_DX11() = default;

	void onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd_);
	void onCmd_SwapBuffers(RenderCommand_SwapBuffers& cmd_);
	void onCmd_DrawCall(RenderCommand_DrawCall& cmd_);

protected:
	virtual void onBeginRender() override;
	virtual void onEndRender() override;
	virtual void onSetFrameBufferSize(Vec2f newSize_) override;
	virtual void onCommit(RenderCommandBuffer& cmdBuf_) override;

	DX11_ID3DInputLayout* _getTestInputLayout(const VertexLayout* src);

	void _createRenderTarget();
	void _setTestShaders();
	void _setTestRenderState();

protected:
	Renderer_DX11* _pRenderer = nullptr;

	ComPtr<DX11_IDXGISwapChain>			_cpSwapChain;
	ComPtr<DX11_ID3DRenderTargetView>	_cpRenderTargetView;
	ComPtr<DX11_ID3DTexture2D>			_cpDepthStencilTexture;
	ComPtr<DX11_ID3DDepthStencilView>	_cpDepthStencilView;

	ComPtr<DX11_ID3DBuffer>				_cpTestVertexBuffer;
	ComPtr<DX11_ID3DVertexShader>		_cpTestVertexShader;
	ComPtr<DX11_ID3DPixelShader>		_cpTestPixelShader;
	ComPtr<DX11_ID3DBlob>				_cpTestVertexShaderBytecode;

	ComPtr<DX11_ID3DRasterizerState>	_cpTestRasterizerState;
	ComPtr<DX11_ID3DDepthStencilState>	_cpTestDepthStencilState;
	ComPtr<DX11_ID3DBlendState>			_cpTestBlendState;

	MemMapFile _testVertexMemMap;

	VectorMap<const VertexLayout*, ComPtr<DX11_ID3DInputLayout>> _testInputLayouts;
};

}

#endif // SGE_OS_WINDOWS

#endif // 0
