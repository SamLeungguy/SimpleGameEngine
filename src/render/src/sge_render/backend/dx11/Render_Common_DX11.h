#pragma once

#if SGE_RENDER_HAS_DX11

#include "../../Render_Common.h"

#include <d3d11.h>
#include <d3d11_4.h>
#include <dxgi1_4.h>

#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <sge_render/Render_Common.h>
#include <sge_render/vertex/Vertex.h>

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

using DX11_ID3DRasterizerState = ID3D11RasterizerState;
using DX11_ID3DDepthStencilState = ID3D11DepthStencilState;
using DX11_ID3DBlendState = ID3D11BlendState;

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

	static UINT castUINT(size_t v) { SGE_ASSERT(v < UINT_MAX); return static_cast<UINT>(v); }

	static D3D11_PRIMITIVE_TOPOLOGY	getDxPrimitiveTopology(RenderPrimitiveType t);
	static DXGI_FORMAT				getDxFormat(RenderDataType v);
	static const char* getDxSemanticName(Vertex_SemanticType t);

	static Renderer_DX11*			renderer();
	static DX11_ID3DDevice*			d3dDevice();
	static DX11_ID3DDeviceContext*	d3dDeviceContext();

	static String getStrFromHRESULT(HRESULT hr);

private:
	static bool _checkError(HRESULT hr) {
		return SUCCEEDED(hr);
	}
};

inline D3D11_PRIMITIVE_TOPOLOGY DX11Util::getDxPrimitiveTopology(RenderPrimitiveType t) {
	using SRC = RenderPrimitiveType;
	switch (t) {
	case SRC::Points:		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case SRC::Lines:		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case SRC::Triangles:	return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	default: throw SGE_ERROR("unknown RenderPrimitiveType");
	}
}

inline const char* DX11Util::getDxSemanticName(Vertex_SemanticType t) {
	using SRC = Vertex_SemanticType;
	switch (t) {
	case SRC::Pos:			return "POSITION";
	case SRC::Color:		return "COLOR";
	case SRC::TexCoord:		return "TEXCOORD";
	case SRC::Normal:		return "NORMAL";
	case SRC::Tangent:		return "TANGENT";
	case SRC::Binormal:		return "BINORMAL";
	default: throw SGE_ERROR("unknown VertexLayout_SemanticType");
	}
}

inline DXGI_FORMAT DX11Util::getDxFormat(RenderDataType v) 
{
	using SRC = RenderDataType;
	switch (v) {
	case SRC::Int8:			return DXGI_FORMAT_R8_SINT; break;
	case SRC::Int8x2:		return DXGI_FORMAT_R8G8_SINT; break;
		//		case SRC::Int8x3:		return DXGI_FORMAT_R8G8B8_SINT; break; //does not support in DX11
	case SRC::Int8x4:		return DXGI_FORMAT_R8G8B8A8_SINT; break;

	case SRC::UInt8:		return DXGI_FORMAT_R8_UINT; break;
	case SRC::UInt8x2:		return DXGI_FORMAT_R8G8_UINT; break;
		//		case SRC::UInt8x3:		return DXGI_FORMAT_R8G8B8_UINT; break; //does not support in DX11
	case SRC::UInt8x4:		return DXGI_FORMAT_R8G8B8A8_UINT; break;

	case SRC::UNorm8:		return DXGI_FORMAT_R8_UNORM; break;
	case SRC::UNorm8x2:		return DXGI_FORMAT_R8G8_UNORM; break;
		//		case SRC::UNorm8x3:		return DXGI_FORMAT_R8G8B8_UNORM; break; //does not support in DX11
	case SRC::UNorm8x4:		return DXGI_FORMAT_R8G8B8A8_UNORM; break;
		//--
	case SRC::Float16:		return DXGI_FORMAT_R16_FLOAT; break;
	case SRC::Float16x2:	return DXGI_FORMAT_R16G16_FLOAT; break;
		//		case SRC::Float16x3:	return DXGI_FORMAT_R16G16B16_FLOAT; break; //does not support in DX11
	case SRC::Float16x4:	return DXGI_FORMAT_R16G16B16A16_FLOAT; break;
		//---
	case SRC::Float32:		return DXGI_FORMAT_R32_FLOAT; break;
	case SRC::Float32x2:	return DXGI_FORMAT_R32G32_FLOAT; break;
	case SRC::Float32x3:	return DXGI_FORMAT_R32G32B32_FLOAT; break;
	case SRC::Float32x4:	return DXGI_FORMAT_R32G32B32A32_FLOAT; break;
		//---
	default: throw SGE_ERROR("unsupported RenderDataType");
	}
}

inline String DX11Util::getStrFromHRESULT(HRESULT hr) {
	const int bufSize = 4096;
	wchar_t buf[bufSize + 1];

	DWORD langId = 0; // MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hr, langId, buf, bufSize, nullptr);
	buf[bufSize] = 0; // ensure terminate with 0

	auto str = UtfUtil::toString(buf);
	return str;
}

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