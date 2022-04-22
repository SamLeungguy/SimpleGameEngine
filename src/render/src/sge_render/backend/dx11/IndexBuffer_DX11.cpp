#include "IndexBuffer_DX11.h"
#include "Renderer_DX11.h"

#if SGE_RENDER_HAS_DX11

namespace sge {

IndexBuffer_DX11::IndexBuffer_DX11(CreateDesc& desc_)
	:
	Base(desc_)
{
	HRESULT hr;

	_count = desc_.count;

	if (!_cpIndexBuffer)
	{
		auto* dev = Renderer_DX11::current()->d3dDevice();
		auto* ctx = Renderer_DX11::current()->d3dDeviceContext();

		// Fill in a buffer description.
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = Util::toBufferUsage_DX11(desc_.usageFlag);
		bufferDesc.ByteWidth = desc_.byteSize;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

#if 0
		// Create the buffer with the device.
		hr = dev->CreateBuffer(&bufferDesc, nullptr, _cpIndexBuffer.ptrForInit());
		Util::throwIfError(hr);

		D3D11_MAPPED_SUBRESOURCE ms = {};
		hr = ctx->Map(_cpIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
		Util::throwIfError(hr);

		memcpy(ms.pData, desc_.pData, bufferDesc.ByteWidth);
		ctx->Unmap(_cpIndexBuffer, 0);
#else
		// Define the resource data.
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = desc_.pData;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		// Create the buffer with the device.
		hr = dev->CreateBuffer(&bufferDesc, &InitData, _cpIndexBuffer.ptrForInit());
		Util::throwIfError(hr);
#endif // 0


		// Create the buffer with the device.
		// https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-buffers-index-how-to
		// hr = g_pd3dDevice->CreateBuffer(&bufferDesc, &InitData, &g_pIndexBuffer);
		// Util::throwIfError(hr);
	}
}

IndexBuffer_DX11::~IndexBuffer_DX11()
{
}

void IndexBuffer_DX11::bind()
{
	auto* ctx = Renderer_DX11::current()->d3dDeviceContext();

	ctx->IASetIndexBuffer(_cpIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

}

#endif