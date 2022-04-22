#pragma once

#if SGE_RENDER_HAS_DX11

#include "../../Render_Common.h"

#include <d3d11.h>
#include <d3d11_4.h>
#include <dxgi1_4.h>

#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace sge {

class Renderer_DX11;

using DX11_IDXGIFactory				= IDXGIFactory1;
using DX11_IDXGIDevice				= IDXGIDevice;
using DX11_IDXGIAdapter				= IDXGIAdapter3;
using DX11_IDXGISwapChain			= IDXGISwapChain;

using DX11_ID3DDevice				= ID3D11Device1;
using DX11_ID3DDeviceContext		= ID3D11DeviceContext4;
using DX11_ID3DDebug				= ID3D11Debug;

using DX11_ID3DTexture1D			= ID3D11Texture1D;
using DX11_ID3DTexture2D			= ID3D11Texture2D;
using DX11_ID3DTexture3D			= ID3D11Texture3D;

using DX11_ID3DRenderTargetView		= ID3D11RenderTargetView;
using DX11_ID3DDepthStencilView		= ID3D11DepthStencilView;

using DX11_ID3DBuffer				= ID3D11Buffer;
using DX11_ID3DBlob					= ID3DBlob;

using DX11_ID3DVertexShader			= ID3D11VertexShader;
using DX11_ID3DPixelShader			= ID3D11PixelShader;
using DX11_ID3DInputLayout			= ID3D11InputLayout;

struct DX11Util
{
	DX11Util() = delete;

	static void throwIfError(HRESULT hr_)
	{
		if (!_checkError(hr_))
		{
			reportError();
			throw SGE_ERROR("HRESULT = {}", hr_);
		}
	}

	static bool assertIfError(HRESULT hr_)
	{
		if (!_checkError(hr_))
		{
			reportError();
			SGE_ASSERT(false);
			return false;
		}
		return true;
	}

	static void reportError();

	static D3D11_USAGE toBufferUsage_DX11(BufferUsage usage_)
	{
		switch (usage_)
		{
			case BufferUsage::Static:	return D3D11_USAGE_DYNAMIC;
			case BufferUsage::Dynamic:	return D3D11_USAGE_DEFAULT;
			case BufferUsage::Default:	return D3D11_USAGE_DEFAULT;
			default: break;
		}
		SGE_ASSERT(0);
		return D3D11_USAGE_DEFAULT;
	}

	static D3D_PRIMITIVE_TOPOLOGY toPrimitiveType_DX11(PrimitiveType type_)
	{
		switch (type_)
		{
			case PrimitiveType::Triangle:			return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;		// D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
			case PrimitiveType::Triangle_Strip:		return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;		// D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
			case PrimitiveType::Line:				return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;		// D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
			case PrimitiveType::Line_Strip:			return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;		// D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
			default: break;
		}
		SGE_ASSERT(0);
		return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	static const StrView toSemanticName_DX11(VertexSematic sematic_)
	{
		switch (sematic_)
		{
			case VertexSematic::Pos:		return "POSITION";
			case VertexSematic::Uv0:		return "TEXCOORD";
			case VertexSematic::Uv1:		return "TEXCOORD";
			case VertexSematic::Uv2:		return "TEXCOORD";
			case VertexSematic::Uv3:		return "TEXCOORD";
			case VertexSematic::Uv4:		return "TEXCOORD";
			case VertexSematic::Uv5:		return "TEXCOORD";
			case VertexSematic::Uv6:		return "TEXCOORD";
			case VertexSematic::Uv7:		return "TEXCOORD";
			case VertexSematic::Color:		return "COLOR";
			default: break;
		}

		return nullptr;
	}


	static DXGI_FORMAT  toRenderDataFormat_DX11(RenderDataType type_)
	{
		switch (type_)
		{
			case RenderDataType::Float32:		return DXGI_FORMAT_R8G8B8A8_UNORM;
			case RenderDataType::Float32x2: 	return DXGI_FORMAT_R32G32_FLOAT;
			case RenderDataType::Float32x3: 	return DXGI_FORMAT_R32G32B32_FLOAT;
			case RenderDataType::Float32x4: 	return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case RenderDataType::Int8: 			return DXGI_FORMAT_R8_SINT;
			case RenderDataType::Int16: 		return DXGI_FORMAT_R16_SINT;
			case RenderDataType::Int32: 		return DXGI_FORMAT_R32_SINT;
			case RenderDataType::Int64:			SGE_ASSERT(0); return DXGI_FORMAT_UNKNOWN;
			default: break;
		}
		SGE_ASSERT(0);
		return DXGI_FORMAT_UNKNOWN;
	}

	static Renderer_DX11*			renderer();
	static DX11_ID3DDevice*			d3dDevice();
	static DX11_ID3DDeviceContext*	d3dDeviceContext();


private:
	static bool _checkError(HRESULT hr_)
	{
		if (FAILED(hr_))
			return false;
		return true;
	}

};

inline void DX11Util::reportError()
{
#if 0 && _DEBUG
	auto* d = renderer()->d3dDebug();
	if (d) {
		d->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
#endif
}

}

#endif