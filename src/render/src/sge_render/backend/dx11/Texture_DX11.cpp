#include "sge_render-pch.h"
#include "Texture_DX11.h"

#include "Renderer_DX11.h"

#if SGE_RENDER_HAS_DX11

namespace sge {

Texture_DX11::Texture_DX11(StrView filepath_, CreateDesc& desc_)
	:
	Base(filepath_, desc_)
{
	_init(desc_);
}

Texture_DX11::Texture_DX11(CreateDesc& desc_)
	:
	Base(desc_)
{
	_init(desc_);
}

void Texture_DX11::_init(CreateDesc& desc_)
{
	_initSampler(desc_);
	_initTextureView(desc_);

	uploadData(desc_.data);
}

void Texture_DX11::_initSampler(CreateDesc& desc_)
{
	using Util = DX11Util;

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	auto* pRenderer = Renderer_DX11::instance();
	auto* dev = pRenderer->d3dDevice();
	//auto* ctx = pRenderer->d3dDeviceContext();

	// Create the texture sampler state.
	auto hr = dev->CreateSamplerState(&samplerDesc, _cpSampleState.ptrForInit());
	Util::throwIfError(hr);

	// Set shader texture resource in the pixel shader.
	//deviceContext->PSSetShaderResources(0, 1, &texture);

	// Set the sampler state in the pixel shader.
	//deviceContext->PSSetSamplers(0, 1, &m_sampleState);
}

void Texture_DX11::_initTextureView(CreateDesc& desc_)
{
	using Util = DX11Util;

	auto* pRenderer = Renderer_DX11::instance();
	auto* dev = pRenderer->d3dDevice();
	auto* ctx = pRenderer->d3dDeviceContext();

	// Setup the description of the texture.
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Height = _height;
	textureDesc.Width = _width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	auto hr = dev->CreateTexture2D(&textureDesc, NULL, _cpTexture.ptrForInit());
	Util::throwIfError(hr);

	// Setup the shader resource view description.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view for the texture.
	hr = dev->CreateShaderResourceView(_cpTexture, &srvDesc, _cpTextureView.ptrForInit());
	Util::throwIfError(hr);

	// Generate mipmaps for this texture.
	ctx->GenerateMips(_cpTextureView);
}

inline void Texture_DX11::uploadData(Span<Color4b> data_)
{
	auto* pRenderer = Renderer_DX11::instance();
	auto* ctx = pRenderer->d3dDeviceContext();

	// Set the row pitch of the targa image data.
	u32 rowPitch = (_width * 4) * sizeof(unsigned char);

	ctx->UpdateSubresource(_cpTexture, 0, NULL, data_.data(), rowPitch, 0);
}

}

#endif // 0
