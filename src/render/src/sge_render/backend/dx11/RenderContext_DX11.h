#pragma once

#include "Render_Common_DX11.h"
#include "../../RenderContext.h"

#if SGE_OS_WINDOWS

namespace sge {

	class RenderContext_DX11 : public RenderContext
	{
		using Base = RenderContext;
		using Util = DX11Util;
	public:
		RenderContext_DX11(CreateDesc desc_);
		virtual ~RenderContext_DX11() = default;

	protected:
		void _createRenderTarget();

		virtual void onBeginRender() override;
		virtual void onEndRender() override;

		virtual void onTestDraw() override;

		virtual void onClearColorAndDepthBuffer() override;
		virtual void onSwapBuffers() override;

	protected:
		Renderer_DX11* _pRenderer = nullptr;

		ComPtr<DX11_IDXGISwapChain>			_cpSwapChain;
		ComPtr<DX11_ID3DRenderTargetView>	_cpRenderTargetView;
		ComPtr<DX11_ID3DTexture2D>			_cpDepthStencilTexture;
		ComPtr<DX11_ID3DDepthStencilView>	_cpDepthStencilView;

		ComPtr<DX11_ID3DBuffer>				_cpTestVertexBuffer;
		ComPtr<DX11_ID3DVertexShader>		_cpTestVertexShader;
		ComPtr<DX11_ID3DPixelShader>		_cpTestPixelShader;
		ComPtr<DX11_ID3DInputLayout>		_cpTestInputLayout;
	};

}

#endif // SGE_OS_WINDOWS
