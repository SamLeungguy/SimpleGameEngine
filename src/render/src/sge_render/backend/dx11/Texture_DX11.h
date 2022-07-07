#pragma once

#include "Render_Common_DX11.h"
#include <sge_render/texture/Texture.h>


#if SGE_RENDER_HAS_DX11

namespace sge {

class Texture_DX11 : public Texture
{
	using Base = Texture;
	using Util = DX11Util;
public:
	Texture_DX11(StrView filepath_, CreateDesc& desc_);
	Texture_DX11(CreateDesc& desc_);

	virtual ~Texture_DX11() = default;

	DX11_ID3DShaderResourceView* getTextureView();
	DX11_ID3DSamplerState* getSampler();

	void uploadData(Span<Color4b> data_);

private:
	void _init(CreateDesc& desc_);
	void _initSampler(CreateDesc& desc_);
	void _initTextureView(CreateDesc& desc_);

private:
	ComPtr<DX11_ID3DTexture2D>			 _cpTexture;
	ComPtr<DX11_ID3DShaderResourceView> _cpTextureView;

	// ps
	ComPtr<DX11_ID3DSamplerState> _cpSampleState;
};

#if 0
#pragma mark ------------- Texture_DX11
#endif // 0
#if 1    // Texture_DX11

inline DX11_ID3DShaderResourceView* Texture_DX11::getTextureView()	{ return _cpTextureView; }
inline DX11_ID3DSamplerState* Texture_DX11::getSampler()			{ return _cpSampleState; }

#endif // 1    // Texture_DX11

}

#endif // 0
