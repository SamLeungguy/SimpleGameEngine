#include <sge_render-pch.h>
#include "Texture_DX11.h"
#include "Renderer_DX11.h"

namespace sge {

#if SGE_RENDER_HAS_DX11

Texture2D_DX11::Texture2D_DX11(CreateDesc& desc_)
	:
	Base(desc_)
{
	auto* renderer = Renderer_DX11::instance();
	auto* dev = renderer->d3dDevice();

	D3D11_TEXTURE2D_DESC dxDesc = {};
	dxDesc.Width		= desc_.size.x;
	dxDesc.Height		= desc_.size.y;
	dxDesc.MipLevels	= desc_.mipmapCount;

	dxDesc.ArraySize	= 1;
	dxDesc.Format = Util::getDxColorType(desc_.colorType);

	dxDesc.SampleDesc.Count = 1;
	dxDesc.SampleDesc.Quality = 0;
	dxDesc.Usage = D3D11_USAGE_DEFAULT;
	dxDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // D3D11_BIND_RENDER_TARGET
	dxDesc.CPUAccessFlags = 0; // D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE
	dxDesc.MiscFlags = 0; // D3D11_RESOURCE_MISC_GENERATE_MIPS


	bool uploadImage = false;
	D3D11_SUBRESOURCE_DATA initData = {};

	if (desc_.imageToUpload.colorType() != ColorType::None) {
		auto& info = desc_.imageToUpload.info();

		initData.pSysMem			= desc_.imageToUpload.dataPtr();
		initData.SysMemPitch		= info.strideInBytes;
		initData.SysMemSlicePitch	= 0;

		uploadImage = true;
	}

	auto hr = dev->CreateTexture2D(&dxDesc, uploadImage ? &initData : nullptr, _tex.ptrForInit());
	Util::throwIfError(hr);

	//---------
	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter   = Util::getDxTextureFilter(desc_.samplerState.filter);
	samplerDesc.AddressU = Util::getDxTextureWrap(desc_.samplerState.wrapU);
	samplerDesc.AddressV = Util::getDxTextureWrap(desc_.samplerState.wrapV);
	samplerDesc.AddressW = Util::getDxTextureWrap(desc_.samplerState.wrapW);
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = desc_.samplerState.minLOD;
	samplerDesc.MaxLOD = desc_.samplerState.maxLOD;

	hr = dev->CreateSamplerState(&samplerDesc, _samplerState.ptrForInit());
	Util::throwIfError(hr);

	//---------
	D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc = {};
	rvDesc.Format = dxDesc.Format;
	rvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	rvDesc.Texture2D.MipLevels = dxDesc.MipLevels ? dxDesc.MipLevels : -1;
	rvDesc.Texture2D.MostDetailedMip = 0;
	hr = dev->CreateShaderResourceView(_tex, &rvDesc, _resourceView.ptrForInit());
	Util::throwIfError(hr);
}


#endif // 0

}