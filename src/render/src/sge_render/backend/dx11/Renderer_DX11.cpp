#include "Renderer_DX11.h"
#include "RenderContext_DX11.h"
#include "RenderGpuBuffer_DX11.h"

#if SGE_RENDER_HAS_DX11

namespace sge{

Renderer_DX11::Renderer_DX11(CreateDesc& desc_)
{
	D3D_FEATURE_LEVEL featureLevel;
	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	ComPtr<ID3D11Device>			cpD3dDevice;
	ComPtr<ID3D11DeviceContext>		cpD3dDeviceContext;

	HRESULT hr;
	hr = D3D11CreateDevice( nullptr,
							D3D_DRIVER_TYPE_HARDWARE,
							nullptr,
							createDeviceFlags,
							nullptr,
							0,
							D3D11_SDK_VERSION,
							cpD3dDevice.ptrForInit(),
							&featureLevel,
							cpD3dDeviceContext.ptrForInit());
	Util::throwIfError(hr);

	hr = cpD3dDevice->QueryInterface(IID_PPV_ARGS(_cpD3dDevice.ptrForInit()));
	Util::throwIfError(hr);

	hr = cpD3dDeviceContext->QueryInterface(IID_PPV_ARGS(_cpD3dDeviceContext.ptrForInit()));
	Util::throwIfError(hr);

	hr = _cpD3dDevice->QueryInterface(IID_PPV_ARGS(_cpDxgiDevice.ptrForInit()));
	Util::throwIfError(hr);

	{
		ComPtr<IDXGIAdapter> cpAdapter;

		hr = _cpDxgiDevice->GetAdapter(cpAdapter.ptrForInit());
		Util::throwIfError(hr);

		hr = cpAdapter->QueryInterface(IID_PPV_ARGS(_cpDxgiAdapter.ptrForInit()));
		Util::throwIfError(hr);
	}

	{
		D3D11_FEATURE_DATA_THREADING feature = {0};
		hr = cpD3dDevice->CheckFeatureSupport(D3D11_FEATURE_THREADING, &feature, sizeof(feature));
		Util::throwIfError(hr);

		_adapterInfo.isMultithread = feature.DriverConcurrentCreates && feature.DriverCommandLists;
		if (!_adapterInfo.isMultithread)
			desc_.isMultithread = false;
	}

	//{
	//	D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS feature = {};
	//	hr = d3dDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &feature, sizeof(feature));
	//	Util::throwIfError(hr);
	//	_adapterInfo.hasComputeShader = feature.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x;
	//}
	_adapterInfo.hasComputeShader = true;

	{
		D3D11_FEATURE_DATA_DOUBLES feature = {};
		hr = cpD3dDevice->CheckFeatureSupport(D3D11_FEATURE_DOUBLES, &feature, sizeof(feature));
		Util::throwIfError(hr);
		_adapterInfo.isShaderHasFloat64 = feature.DoublePrecisionFloatShaderOps;
	}

	{
		DXGI_ADAPTER_DESC desc;
		hr = _cpDxgiAdapter->GetDesc(&desc);
		Util::throwIfError(hr);

		_adapterInfo.adapterName = UtfUtil::toString(desc.Description);
		_adapterInfo.memorySize = desc.DedicatedVideoMemory;

		SGE_LOG("Render Adapter\n  name={}\n  mem={}G"
			, _adapterInfo.adapterName
			, Math::byteToG(_adapterInfo.memorySize));
	}

	hr = _cpDxgiAdapter->GetParent(IID_PPV_ARGS(_cpDxgiFactory.ptrForInit()));
	Util::throwIfError(hr);

	if (createDeviceFlags & D3D11_CREATE_DEVICE_DEBUG) {
		hr = _cpD3dDevice->QueryInterface(_cpD3dDebug.ptrForInit());
		Util::throwIfError(hr);
	}
}

RenderContext* Renderer_DX11::onCreateContext(RenderContext_CreateDesc& desc_)
{
	return new RenderContext_DX11(desc_);
}

RenderGpuBuffer* Renderer_DX11::onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc_)
{
	return new RenderGpuBuffer_DX11(desc_);
}

RenderShader* Renderer_DX11::onCreateShader(RenderShader_CreateDesc& desc_)
{
	return nullptr;
}

}

#endif // SGE_RENDER_HAS_DX11
