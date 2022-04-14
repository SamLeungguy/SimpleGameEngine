#pragma once

#include "../base/GraphicsContext_Base.h"

#if SGE_OS_WINDOWS

namespace sge {

	class GraphicsContext_D3d11 : public GraphicsContext_Base
	{
	public:
		GraphicsContext_D3d11() = default;
		virtual ~GraphicsContext_D3d11();

		virtual void init(void* hWnd_) override;
		virtual void destroy() override;

		virtual void swapBuffers() override;

		//virtual void makeContextCurrent() override;
		//inline virtual void* getHandle() const override;
		//virtual void enableVsync(bool isVsync_) override;

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
