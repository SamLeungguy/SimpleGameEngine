#include "RenderContext_DX11.h"
#include "Renderer_DX11.h"

#if SGE_OS_WINDOWS

namespace sge {

RenderContext_DX11::RenderContext_DX11(CreateDesc desc_)
	:
	Base(desc_)
{
	_pRenderer = Renderer_DX11::current();

	auto* pWin = static_cast<NativeUIWindow*>(desc_.pWindow);

	auto* pDev = _pRenderer->d3dDevice();
	auto* pDxgiFactory = _pRenderer->dxgiFactory();

	// init swapchain
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferDesc.Width = 8;
		swapChainDesc.BufferDesc.Height = 8;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = pWin->_hwnd;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		auto hr = pDxgiFactory->CreateSwapChain(pDev, &swapChainDesc, _cpSwapChain.ptrForInit());
		Util::throwIfError(hr);
	}
}

void RenderContext_DX11::_createRenderTarget()
{
	auto* pRenderer = Renderer_DX11::current();
	auto* pDev = pRenderer->d3dDevice();
	HRESULT hr;

	hr = _cpSwapChain->ResizeBuffers(0, 800, 600, DXGI_FORMAT_UNKNOWN, 0);
	Util::throwIfError(hr);

	ComPtr<ID3D11Texture2D> cpBackBuffer;
	hr = _cpSwapChain->GetBuffer(0, IID_PPV_ARGS(cpBackBuffer.ptrForInit()));
	Util::throwIfError(hr);

	hr = pDev->CreateRenderTargetView(cpBackBuffer, nullptr, _cpRenderTargetView.ptrForInit());
	Util::throwIfError(hr);

	D3D11_TEXTURE2D_DESC backBufferDesc;
	cpBackBuffer->GetDesc(&backBufferDesc);

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = {0};
	descDepth.Width = backBufferDesc.Width;
	descDepth.Height = backBufferDesc.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = pDev->CreateTexture2D(&descDepth, nullptr, _cpDepthStencilTexture.ptrForInit());
	Util::throwIfError(hr);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = pDev->CreateDepthStencilView(_cpDepthStencilTexture.ptr(), &descDSV, _cpDepthStencilView.ptrForInit());
	Util::throwIfError(hr);
}

void RenderContext_DX11::onBeginRender()
{
	auto* pContex = _pRenderer->d3dDeviceContext();
	if (!_cpRenderTargetView) {
		_createRenderTarget();
	}

	DX11_ID3DRenderTargetView* rt[] = { _cpRenderTargetView };
	//	pContex->OMSetRenderTargets(1, rt, _depthStencilView);
	pContex->OMSetRenderTargets(1, rt, nullptr);
}

void RenderContext_DX11::onEndRender()
{
}

void RenderContext_DX11::onTestDraw()
{
	auto* dev = _pRenderer->d3dDevice();
	auto* ctx = _pRenderer->d3dDeviceContext();

	HRESULT hr;

	// testing
	struct Vertex {
		float x, y, z;
		u8 color[4];
	};

	static Vertex vertexData[] =
	{
		{ 0.0f,  0.5f, 0.0f, { 255, 0, 0, 255 }},
		{ 0.5f, -0.5f, 0.0f, { 0, 255, 0, 255 }},
		{-0.5f, -0.5f, 0.0f, { 0, 0, 255, 255 }}
	};

	UINT vertexCount = sizeof(vertexData) / sizeof(vertexData[0]);

	if (!_cpTestVertexBuffer) {

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(Vertex) * vertexCount;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		hr = dev->CreateBuffer(&bd, nullptr, _cpTestVertexBuffer.ptrForInit());
		Util::throwIfError(hr);

		D3D11_MAPPED_SUBRESOURCE ms = {};
		hr = ctx->Map(_cpTestVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
		Util::throwIfError(hr);

		memcpy(ms.pData, vertexData, bd.ByteWidth);
		ctx->Unmap(_cpTestVertexBuffer, 0);
	}

	const wchar_t* shaderFile = L"Assets/Shaders/test.hlsl";

	if (!_cpTestVertexShader) {
		ComPtr<ID3DBlob>	bytecode;
		ComPtr<ID3DBlob>	errorMsg;
		hr = D3DCompileFromFile(shaderFile, 0, 0, "vs_main", "vs_4_0", 0, 0, bytecode.ptrForInit(), errorMsg.ptrForInit());
		Util::throwIfError(hr);

		hr = dev->CreateVertexShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, _cpTestVertexShader.ptrForInit());
		Util::throwIfError(hr);

		D3D11_INPUT_ELEMENT_DESC inputDesc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		hr = dev->CreateInputLayout(inputDesc
			, sizeof(inputDesc) / sizeof(inputDesc[0])
			, bytecode->GetBufferPointer()
			, bytecode->GetBufferSize()
			, _cpTestInputLayout.ptrForInit());
		Util::throwIfError(hr);
	}

	if (!_cpTestPixelShader) {
		ComPtr<ID3DBlob>	bytecode;
		ComPtr<ID3DBlob>	errorMsg;
		hr = D3DCompileFromFile(shaderFile, 0, 0, "ps_main", "ps_4_0", 0, 0, bytecode.ptrForInit(), errorMsg.ptrForInit());
		Util::throwIfError(hr);

		dev->CreatePixelShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, _cpTestPixelShader.ptrForInit());
		Util::throwIfError(hr);
	}

	// draw
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 800;
	viewport.Height = 600;

	ctx->RSSetViewports(1, &viewport);

	ctx->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ctx->IASetInputLayout(_cpTestInputLayout);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer* pVBuffer[] = { _cpTestVertexBuffer };
	ctx->IASetVertexBuffers(0, 1, pVBuffer, &stride, &offset);

	ctx->VSSetShader(_cpTestVertexShader, 0, 0);
	ctx->PSSetShader(_cpTestPixelShader, 0, 0);

	ctx->Draw(vertexCount, 0);
}

void RenderContext_DX11::onClearColorAndDepthBuffer()
{
	auto* ctx = _pRenderer->d3dDeviceContext();
	if (_cpRenderTargetView) {
		float color[4] = { 0, 0, 0.5f, 1 };
		ctx->ClearRenderTargetView(_cpRenderTargetView, color);
	}
	if (_cpDepthStencilView) {
		float depth = 0;
		ctx->ClearDepthStencilView(_cpDepthStencilView, D3D11_CLEAR_DEPTH, depth, 0);
	}
}

void RenderContext_DX11::onSwapBuffers()
{
	auto hr = _cpSwapChain->Present(_pRenderer->isVsync() ? 1 : 0, 0);
	Util::throwIfError(hr);
}

}


#endif // sge_os_windows
