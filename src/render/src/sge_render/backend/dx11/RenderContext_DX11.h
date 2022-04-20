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

	private:
		ID3D11Device* _pDev				= nullptr;                     // the pointer to our Direct3D device interface
		ID3D11DeviceContext* _pDevcon	= nullptr;           // the pointer to our Direct3D device context

		// temporary--Start--
		// similar to framebuffer
		IDXGISwapChain* _pSwapchain		= nullptr;             // the pointer to the swap chain interface
		ID3D11RenderTargetView* _pRTV	= nullptr;

		// shader
		ID3DBlob* _pVS_Blob				= nullptr;
		ID3DBlob* _pPS_Blob				= nullptr;
		ID3DBlob* _pErrorMsgs			= nullptr;

		ID3D11VertexShader* _pVS		= nullptr;;    // the vertex shader
		ID3D11PixelShader* _pPS			= nullptr;     // the pixel shader

		// Vertex buffer
		ID3D11Buffer* _pVBuffer;    // global

		struct VertexLayout
		{
			float x, y, z;      // position
			float color[4];    // color
		};


		// temporary--End--
	};

}

#endif // SGE_OS_WINDOWS
