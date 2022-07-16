#pragma once

#include "Render_Common_DX11.h"
#include <sge_render/Renderer.h>

#if SGE_RENDER_HAS_DX11

namespace sge {

class Renderer_DX11 : public Renderer
{
	using Base = Renderer;
	using Util = DX11Util;
public:
	static Renderer_DX11* instance();

	Renderer_DX11(CreateDesc& desc_);
	virtual ~Renderer_DX11() = default;

	DX11_IDXGIFactory*				dxgiFactory()				{ return _cpDxgiFactory; }
	DX11_IDXGIDevice*				dxgiDevice()				{ return _cpDxgiDevice; }
	DX11_IDXGIAdapter*				dxgiAdapter()				{ return _cpDxgiAdapter; }
	DX11_ID3DDevice*				d3dDevice()					{ return _cpD3dDevice; }
	DX11_ID3DDeviceContext*			d3dDeviceContext()			{ return _cpD3dDeviceContext; }
	DX11_ID3DDebug*					d3dDebug()					{ return _cpD3dDebug; }

	void validateContext();

protected:
	virtual SPtr<RenderContext>		onCreateContext		(RenderContext_CreateDesc&		desc_)	override;
	virtual SPtr<RenderGpuBuffer>	onCreateGpuBuffer	(RenderGpuBuffer_CreateDesc&	desc_)	override;
	virtual SPtr<Texture2D>			onCreateTexture2D	(Texture2D_CreateDesc&			desc_)	override;
	virtual SPtr<Shader>			onCreateShader		(StrView filename_)						override;
	virtual SPtr<Material>			onCreateMaterial	()										override;

	ComPtr<DX11_IDXGIFactory>		_cpDxgiFactory;
	ComPtr<DX11_IDXGIDevice>		_cpDxgiDevice;
	ComPtr<DX11_IDXGIAdapter>		_cpDxgiAdapter;
	ComPtr<DX11_ID3DDevice>			_cpD3dDevice;
	ComPtr<DX11_ID3DDeviceContext>	_cpD3dDeviceContext;
	ComPtr<DX11_ID3DDebug>			_cpD3dDebug;
};

inline Renderer_DX11* Renderer_DX11::instance() { return static_cast<Renderer_DX11*>(s_pInstance); }

}

#endif