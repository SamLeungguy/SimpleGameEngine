#include "RenderContext_DX11.h"
#include "Renderer_DX11.h"
#include "RenderGpuBuffer_DX11.h"


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

void RenderContext_DX11::onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd_)
{
	auto* ctx = _pRenderer->d3dDeviceContext();
	if (_cpRenderTargetView && cmd_.color.has_value()) {
		ctx->ClearRenderTargetView(_cpRenderTargetView, cmd_.color->data);
	}
	if (_cpDepthStencilView && cmd_.depth.has_value()) {
		ctx->ClearDepthStencilView(_cpDepthStencilView, D3D11_CLEAR_DEPTH, *cmd_.depth, 0);
	}
}

void RenderContext_DX11::onCmd_SwapBuffers(RenderCommand_SwapBuffers& cmd_)
{
	auto hr = _cpSwapChain->Present(_pRenderer->isVsync() ? 1 : 0, 0);
	Util::throwIfError(hr);
}

void RenderContext_DX11::onCmd_DrawCall(RenderCommand_DrawCall& cmd_)
{
	if (!cmd_.pVertexLayout) { SGE_ASSERT(false); return; }

	auto* vertexBuffer = static_cast<RenderGpuBuffer_DX11*>(cmd_.spVertexBuffer.ptr());
	if (!vertexBuffer) { SGE_ASSERT(false); return; }

	auto* indexBuffer = static_cast<RenderGpuBuffer_DX11*>(cmd_.spIndexBuffer.ptr());
	//if (!indexBuffer) { SGE_ASSERT(false); return; }

	if (cmd_.vertexCount <= 0) { SGE_ASSERT(false); return; }
	if (cmd_.primitive == RenderPrimitiveType::None) { SGE_ASSERT(false); return; }

	_setTestShaders();

	auto* ctx = _pRenderer->d3dDeviceContext();
	auto primitive = Util::getDxPrimitiveTopology(cmd_.primitive);
	ctx->IASetPrimitiveTopology(primitive);

	auto* inputLayout = _getTestInputLayout(cmd_.pVertexLayout);
	if (!inputLayout) { SGE_ASSERT(false); return; }

	ctx->IASetInputLayout(inputLayout);

	UINT stride = static_cast<UINT>(cmd_.pVertexLayout->stride);
	UINT offset = 0;
	UINT vertexCount = static_cast<UINT>(cmd_.vertexCount);

	DX11_ID3DBuffer* ppVertexBuffers[] = { vertexBuffer->getBuffer() };
	ctx->IASetVertexBuffers(0, 1, ppVertexBuffers, &stride, &offset);

	if (indexBuffer)
	{
		ctx->IASetIndexBuffer(indexBuffer->getBuffer(), DXGI_FORMAT_R16_UINT, 0);
		ctx->DrawIndexed(static_cast<UINT>(cmd_.indexCount), 0, 0);
	}
	else
	{
		ctx->Draw(vertexCount, 0);
	}
}

void RenderContext_DX11::_createRenderTarget()
{
	auto* renderer = Renderer_DX11::current();
	auto* dev = renderer->d3dDevice();
	HRESULT hr;

	ComPtr<ID3D11Texture2D> backBuffer;
	hr = _cpSwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.ptrForInit()));
	Util::throwIfError(hr);

	hr = dev->CreateRenderTargetView(backBuffer, nullptr, _cpRenderTargetView.ptrForInit());
	Util::throwIfError(hr);

	D3D11_TEXTURE2D_DESC backBufferDesc;
	backBuffer->GetDesc(&backBufferDesc);

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = {};
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
	hr = dev->CreateTexture2D(&descDepth, nullptr, _cpDepthStencilTexture.ptrForInit());
	Util::throwIfError(hr);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = dev->CreateDepthStencilView(_cpDepthStencilTexture.ptr(), &descDSV, _cpDepthStencilView.ptrForInit());
	Util::throwIfError(hr);
}

void RenderContext_DX11::_setTestShaders()
{
	HRESULT hr;
	const wchar_t* shaderFile = L"Assets/Shaders/test.hlsl";

	auto* dev = _pRenderer->d3dDevice();
	auto* ctx = _pRenderer->d3dDeviceContext();

	if (!_cpTestVertexShader) {
		ComPtr<ID3DBlob>	bytecode;
		ComPtr<ID3DBlob>	errorMsg;
		hr = D3DCompileFromFile(shaderFile, 0, 0, "vs_main", "vs_4_0", 0, 0, bytecode.ptrForInit(), errorMsg.ptrForInit());
		Util::throwIfError(hr);

		_cpTestVertexShaderBytecode = bytecode;

		hr = dev->CreateVertexShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, _cpTestVertexShader.ptrForInit());
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

	ctx->VSSetShader(_cpTestVertexShader, 0, 0);
	ctx->PSSetShader(_cpTestPixelShader, 0, 0);
}

void RenderContext_DX11::onBeginRender()
{
	auto* ctx = _pRenderer->d3dDeviceContext();
	if (!_cpRenderTargetView) {
		_createRenderTarget();
	}

	DX11_ID3DRenderTargetView* rt[] = { _cpRenderTargetView };
	//	ctx->OMSetRenderTargets(1, rt, _depthStencilView);
	ctx->OMSetRenderTargets(1, rt, nullptr);

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = _frameBufferSize.x;
	viewport.Height = _frameBufferSize.y;

	ctx->RSSetViewports(1, &viewport);
}

void RenderContext_DX11::onEndRender()
{
}

void RenderContext_DX11::onSetFrameBufferSize(Vec2f newSize_)
{
	_cpRenderTargetView.reset(nullptr); // release buffer and render target view before resize

	auto hr = _cpSwapChain->ResizeBuffers(0
		, static_cast<UINT>(Math::max(0.0f, newSize_.x))
		, static_cast<UINT>(Math::max(0.0f, newSize_.y))
		, DXGI_FORMAT_UNKNOWN
		, 0);
	Util::throwIfError(hr);
}

void RenderContext_DX11::onCommit(RenderCommandBuffer& cmdBuf_)
{
	_dispatch(this, cmdBuf_);
}

DX11_ID3DInputLayout* RenderContext_DX11::_getTestInputLayout(const VertexLayout* src)
{
	if (!src) 
		return nullptr;

	auto it = _testInputLayouts.find(src);
	if (it != _testInputLayouts.end()) {
		return it->second;
	}

#if 1 // original code
	Vector_<D3D11_INPUT_ELEMENT_DESC, 32> inputDesc;

	for (auto& e : src->elements) {
		auto& dst = inputDesc.emplace_back();
		auto semanticType = Vertex_SemanticUtil::getType(e.semantic);
		dst.SemanticName = Util::getDxSemanticName(semanticType);
		dst.SemanticIndex = Vertex_SemanticUtil::getIndex(e.semantic);
		dst.Format = Util::getDxFormat(e.dataType);
		dst.InputSlot = 0;
		dst.AlignedByteOffset = e.offset;
		dst.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		dst.InstanceDataStepRate = 0;
	}

	ComPtr<DX11_ID3DInputLayout>	outLayout;

	SGE_DUMP_VAR(_cpTestVertexShaderBytecode->GetBufferSize());
	SGE_DUMP_VAR(static_cast<UINT>(inputDesc.size()));
	SGE_DUMP_VAR(_cpTestVertexShaderBytecode->GetBufferPointer());

	auto* dev = _pRenderer->d3dDevice();
	auto hr = dev->CreateInputLayout(inputDesc.data()
		, static_cast<UINT>(inputDesc.size())
		, _cpTestVertexShaderBytecode->GetBufferPointer()
		, _cpTestVertexShaderBytecode->GetBufferSize()
		, outLayout.ptrForInit());
	Util::throwIfError(hr);

	_testInputLayouts[src] = outLayout;
	return outLayout;
#else
	ComPtr<DX11_ID3DInputLayout>	outLayout;
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	auto* dev = _pRenderer->d3dDevice();
	auto hr = dev->CreateInputLayout(layout
		, 3
		, _cpTestVertexShaderBytecode->GetBufferPointer()
		, _cpTestVertexShaderBytecode->GetBufferSize()
		, outLayout.ptrForInit());
	Util::throwIfError(hr);

	_testInputLayouts[src] = outLayout;
	return outLayout;
#endif // 0

}

}


#endif // sge_os_windows
