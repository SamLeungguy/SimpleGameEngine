#pragma once

#include <sge_render/backend/dx11/Render_Common_DX11.h>
#include <sge_render/textures/Texture.h>

namespace sge {

#if SGE_RENDER_HAS_DX11

class Texture2D_DX11 : public Texture2D
{
	using Base = Texture2D;
	using Util = DX11Util;
public:
	Texture2D_DX11(CreateDesc& desc);

	DX11_ID3DShaderResourceView*	resourceView();
	DX11_ID3DSamplerState*			samplerState();

private:
	ComPtr<DX11_ID3DTexture2D>			_tex;
	ComPtr<DX11_ID3DShaderResourceView>	_resourceView;
	ComPtr<DX11_ID3DSamplerState>		_samplerState;
};

#if 0
#pragma mark ===================== Texture2D_DX11
#endif // 0
#if 1    // Texture2D_DX11

inline DX11_ID3DShaderResourceView*	Texture2D_DX11::resourceView() { return _resourceView.ptr(); }
inline DX11_ID3DSamplerState*		Texture2D_DX11::samplerState() { return _samplerState.ptr(); }

#endif // 0    // Texture2D_DX11


#endif // 0


}