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

	static Vertex_SemanticType getSemanticType(const char* name_);
	static RenderDataType getRenderDataType(const char* name_);
	static RenderDataType getRenderDataTypeBySemanticName(const char* name_);

	static const char* getShaderVersion(UINT version_);

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

inline Vertex_SemanticType DX11Util::getSemanticType(const char* name_)
{
	using SRC = Vertex_SemanticType;

	size_t hs = Math::hashStr(name_);
	static const size_t compares[] = { Math::hashStr("POSITION"), Math::hashStr("SV_POSITION"), Math::hashStr("COLOR"), Math::hashStr("TEXCOORD")
										, Math::hashStr("NORMAL"), Math::hashStr("TANGENT"), Math::hashStr("BINORMAL") };

	if (hs == compares[0]) return SRC::Pos;
	if (hs == compares[1]) return SRC::Pos;

	if (hs == compares[2]) return SRC::Color;
	if (hs == compares[3]) return SRC::TexCoord;
	if (hs == compares[4]) return SRC::Normal;
	if (hs == compares[5]) return SRC::Tangent;
	if (hs == compares[6]) return SRC::Binormal;

	throw SGE_ERROR("{}", "invalid");

	//return SRC::None;
}

inline RenderDataType DX11Util::getRenderDataType(const char* name_)
{
	using SRC = RenderDataType;

	size_t hs = Math::hashStr(name_);

#define MY_ARRAY_ELEMENTS(hash, type) Math::hashStr(#type), Math::hashStr(#type "2"), Math::hashStr(#type "3"), Math::hashStr(#type "4")

#define MY_TYPE_IF_NOT_0(type, biteSize, count, i)	\
	if(hs == compares[i]) return SRC::type ## biteSize ## x ## count	\
//---------------

#define MY_TYPE_IF(type, biteSize, col)								\
	if(hs == compares[col * 4 + 0]) return SRC::type ## biteSize;	\
	MY_TYPE_IF_NOT_0(type, biteSize, 2, col * 4 + 1);				\
	MY_TYPE_IF_NOT_0(type, biteSize, 3, col * 4 + 2);				\
	MY_TYPE_IF_NOT_0(type, biteSize, 4, col * 4 + 3);				\
//---------------

	// follow dx11 hlsl shader data type name
	static const size_t compares[] = {
	MY_ARRAY_ELEMENTS(h, half),
	MY_ARRAY_ELEMENTS(h, float),
	MY_ARRAY_ELEMENTS(h, int),


	//Math::hashStr("matrix"),
	//Math::hashStr("float4x4"),
	};

	MY_TYPE_IF(Float, 16,	0);
	MY_TYPE_IF(Float, 32,	1);
	MY_TYPE_IF(Int, 32,		2);

	//if (hs == compares[12]) return SRC::Float32x4x4;
	//if (hs == compares[13]) return SRC::Float32x4x4;

	throw SGE_ERROR("{}", "invalid");

	//return SRC::None;

#undef MY_ARRAY_ELEMENTS
#undef MY_TYPE_IF
#undef MY_TYPE_IF_NOT_0
}

inline RenderDataType DX11Util::getRenderDataTypeBySemanticName(const char* name_)
{
	using SRC = RenderDataType;

	size_t hs = Math::hashStr(name_);
	static const size_t compares[] = { Math::hashStr("POSITION"), Math::hashStr("SV_POSITION"), Math::hashStr("COLOR"), Math::hashStr("TEXCOORD")
										, Math::hashStr("NORMAL"), Math::hashStr("TANGENT"), Math::hashStr("BINORMAL") };

	if (hs == compares[0]) return SRC::Float32x4;
	if (hs == compares[1]) return SRC::Float32x4;

	if (hs == compares[2]) return SRC::Float32x4;
	if (hs == compares[3]) return SRC::Float32x2;
	if (hs == compares[4]) return SRC::Float32x3;
	if (hs == compares[5]) return SRC::Float32x3;
	if (hs == compares[6]) return SRC::Float32x3;

	throw SGE_ERROR("{}", "invalid");

	//return SRC::None;
}

inline DXGI_FORMAT DX11Util::getDxFormat(RenderDataType v) {
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

		case SRC::SNorm8:		return DXGI_FORMAT_R8_SNORM; break;
		case SRC::SNorm8x2:		return DXGI_FORMAT_R8G8_SNORM; break;
			//		case SRC::SNorm8x3:		return DXGI_FORMAT_R8G8B8_SNORM; break; //does not support in DX11
		case SRC::SNorm8x4:		return DXGI_FORMAT_R8G8B8A8_SNORM; break;

		case SRC::UNorm8:		return DXGI_FORMAT_R8_UNORM; break;
		case SRC::UNorm8x2:		return DXGI_FORMAT_R8G8_UNORM; break;
			//		case SRC::UNorm8x3:		return DXGI_FORMAT_R8G8B8_UNORM; break; //does not support in DX11
		case SRC::UNorm8x4:		return DXGI_FORMAT_R8G8B8A8_UNORM; break;

		case SRC::Int16:		return DXGI_FORMAT_R16_SINT; break;
		case SRC::Int16x2:		return DXGI_FORMAT_R16G16_SINT; break;
			//		case SRC::Int16x3:		return DXGI_FORMAT_R16G16B16_SINT; break; //does not support in DX11
		case SRC::Int16x4:		return DXGI_FORMAT_R16G16B16A16_SINT; break;

		case SRC::UInt16:		return DXGI_FORMAT_R16_UINT; break;
		case SRC::UInt16x2:		return DXGI_FORMAT_R16G16_UINT; break;
			//		case SRC::UInt16x3:		return DXGI_FORMAT_R16G16B16_UINT; break; //does not support in DX11
		case SRC::UInt16x4:		return DXGI_FORMAT_R16G16B16A16_UINT; break;

		case SRC::SNorm16:		return DXGI_FORMAT_R16_SNORM; break;
		case SRC::SNorm16x2:	return DXGI_FORMAT_R16G16_SNORM; break;
			//		case SRC::SNorm16x3:	return DXGI_FORMAT_R16G16B16_SNORM; break; //does not support in DX11
		case SRC::SNorm16x4:	return DXGI_FORMAT_R16G16B16A16_SNORM; break;

		case SRC::UNorm16:		return DXGI_FORMAT_R16_UNORM; break;
		case SRC::UNorm16x2:	return DXGI_FORMAT_R16G16_UNORM; break;
			//		case SRC::UNorm16x3:	return DXGI_FORMAT_R16G16B16_UNORM; break; //does not support in DX11
		case SRC::UNorm16x4:	return DXGI_FORMAT_R16G16B16A16_UNORM; break;

		case SRC::Int32:		return DXGI_FORMAT_R32_SINT; break;
		case SRC::Int32x2:		return DXGI_FORMAT_R32G32_SINT; break;
			//		case SRC::Int32x3:		return DXGI_FORMAT_R32G32B32_SINT; break; //does not support in DX11
		case SRC::Int32x4:		return DXGI_FORMAT_R32G32B32A32_SINT; break;

		case SRC::UInt32:		return DXGI_FORMAT_R32_UINT; break;
		case SRC::UInt32x2:		return DXGI_FORMAT_R32G32_UINT; break;
			//		case SRC::UInt32x3:		return DXGI_FORMAT_R32G32B32_UINT; break; //does not support in DX11
		case SRC::UInt32x4:		return DXGI_FORMAT_R32G32B32A32_UINT; break;

			//		case SRC::SNorm32:		return DXGI_FORMAT_R32_SNORM; break;
			//		case SRC::SNorm32x2:	return DXGI_FORMAT_R32G32_SNORM; break;
			//		case SRC::SNorm32x3:	return DXGI_FORMAT_R32G32B32_SNORM; break; //does not support in DX11
			//		case SRC::SNorm32x4:	return DXGI_FORMAT_R32G32B32A32_SNORM; break;

			//		case SRC::UNorm32:		return DXGI_FORMAT_R32_UNORM; break;
			//		case SRC::UNorm32x2:	return DXGI_FORMAT_R32G32_UNORM; break;
			//		case SRC::UNorm32x3:	return DXGI_FORMAT_R32G32B32_UNORM; break; //does not support in DX11
			//		case SRC::UNorm32x4:	return DXGI_FORMAT_R32G32B32A32_UNORM; break;

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

inline const char* DX11Util::getShaderVersion(UINT version_)
{
#define MY_CASE_SHADER_VERSION_PS(ver)	case 0x000 ## ver ## 0: return  "ps_" #ver "_0"; break
#define MY_CASE_SHADER_VERSION_VS(ver)	case 0x100 ## ver ## 0: return  "vs_" #ver "_0"; break
#define MY_CASE_SHADER_VERSION_GS(ver)	case 0x200 ## ver ## 0: return  "gs_" #ver "_0"; break
#define MY_CASE_SHADER_VERSION_DS(ver)	case 0x300 ## ver ## 0: return  "hs_" #ver "_0"; break
#define MY_CASE_SHADER_VERSION_HS(ver)	case 0x400 ## ver ## 0: return  "ds_" #ver "_0"; break
#define MY_CASE_SHADER_VERSION_CS(ver)	case 0x500 ## ver ## 0: return  "cs_" #ver "_0"; break

#define MY_CASE_SHADER_VERSION(ver)	\
	MY_CASE_SHADER_VERSION_PS(ver); \
	MY_CASE_SHADER_VERSION_VS(ver); \
	MY_CASE_SHADER_VERSION_GS(ver); \
	MY_CASE_SHADER_VERSION_DS(ver); \
	MY_CASE_SHADER_VERSION_HS(ver); \
	MY_CASE_SHADER_VERSION_CS(ver) \
//--------------

#define MY_CASE_SHADER_VERSION_4()	\
	MY_CASE_SHADER_VERSION_PS(4); \
	MY_CASE_SHADER_VERSION_VS(4); \
	MY_CASE_SHADER_VERSION_GS(4); \
	MY_CASE_SHADER_VERSION_CS(4) \
//--------------

	using SRC = RenderShaderType;
	switch (version_)
	{
		MY_CASE_SHADER_VERSION_4();
		MY_CASE_SHADER_VERSION(5);
		MY_CASE_SHADER_VERSION(6);

		default: throw SGE_ERROR("unknown Shader Version");
	}
	//return nullptr;

#undef MY_CASE_SHADER_VERSION_PS
#undef MY_CASE_SHADER_VERSION_VS
#undef MY_CASE_SHADER_VERSION_GS
#undef MY_CASE_SHADER_VERSION_DS
#undef MY_CASE_SHADER_VERSION_HS
#undef MY_CASE_SHADER_VERSION_CS
#undef MY_CASE_SHADER_VERSION
#undef MY_CASE_SHADER_VERSION_4
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