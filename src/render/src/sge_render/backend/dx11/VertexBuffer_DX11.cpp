#include "VertexBuffer_DX11.h"
#include "Renderer_DX11.h"

#if SGE_RENDER_HAS_DX11

namespace sge {

VertexBuffer_DX11::VertexBuffer_DX11(CreateDesc& desc_)
	:
	Base(desc_)
{
	SGE_ASSERT(desc_.pVertexLayout, "no verte layout found!");

	_pVertexLayout = desc_.pVertexLayout;
	_vertexCount = desc_.vertexCount;

	HRESULT hr;

	// TODO: remove temp
	if (desc_.pData == nullptr)
	{
		SGE_LOG_WARN("Vertex buffer is nullptr!");
	}

	if (!_cpVertexBuffer) {

		auto* dev = Renderer_DX11::current()->d3dDevice();
		auto* ctx = Renderer_DX11::current()->d3dDeviceContext();

		D3D11_BUFFER_DESC bd = {0};
		bd.Usage = Util::toBufferUsage_DX11(desc_.usageFlag);
		//bd.ByteWidth = sizeof(Vertex) * vertexCount;
		bd.ByteWidth = desc_.byteSize;

		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		hr = dev->CreateBuffer(&bd, nullptr, _cpVertexBuffer.ptrForInit());
		Util::throwIfError(hr);

		D3D11_MAPPED_SUBRESOURCE ms = {0};
		hr = ctx->Map(_cpVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
		Util::throwIfError(hr);

		memcpy(ms.pData, desc_.pData, bd.ByteWidth);
		ctx->Unmap(_cpVertexBuffer, 0);
	}
}

VertexBuffer_DX11::~VertexBuffer_DX11()
{
}

void VertexBuffer_DX11::bind()
{
	auto* ctx = Renderer_DX11::current()->d3dDeviceContext();

	UINT stride = _pVertexLayout->totalBytes;
	UINT offset = 0;
	ID3D11Buffer* pVBuffer[] = { _cpVertexBuffer };
	ctx->IASetVertexBuffers(0, 1, pVBuffer, &stride, &offset);
}

u64 VertexBuffer_DX11::getCount()
{
	return _vertexCount;
}

}


#endif // SGE_RENDER_HAS_DX11
