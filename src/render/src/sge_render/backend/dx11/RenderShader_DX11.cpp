#include "RenderShader_DX11.h"
#include "Renderer_DX11.h"

#include <d3d11shader.h>

namespace sge {

RenderShader_DX11::RenderShader_DX11(CreateDesc& desc_)
	:
	Base(desc_)
{
	
}

void RenderShader_DX11::destroy()
{
	if (_cpVertexShader)			_cpVertexShader.reset(nullptr);
	if (_cpPixelShader)				_cpPixelShader.reset(nullptr);
	if (_cpVertexShaderBytecode)    _cpVertexShaderBytecode.reset(nullptr);

	_inputLayouts.clear();
}

void RenderShader_DX11::reload()
{
	destroy();

	HRESULT hr;
	const wchar_t* shaderFile = L"Assets/Shaders/test.hlsl";

	auto* _pRenderer = Renderer_DX11::current();

	auto* dev = _pRenderer->d3dDevice();
	//auto* ctx = _pRenderer->d3dDeviceContext();

	// use the byte code after the shader compiler done

	if (!_cpVertexShader) {
		ComPtr<ID3DBlob>	bytecode;
		ComPtr<ID3DBlob>	errorMsg;
		hr = D3DCompileFromFile(shaderFile, 0, 0, "vs_main", "vs_5_0", 0, 0, bytecode.ptrForInit(), errorMsg.ptrForInit());
		Util::throwIfError(hr);

		_cpVertexShaderBytecode = bytecode;

		hr = dev->CreateVertexShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, _cpVertexShader.ptrForInit());
		Util::throwIfError(hr);
	}

	if (!_cpPixelShader) {
		ComPtr<ID3DBlob>	bytecode;
		ComPtr<ID3DBlob>	errorMsg;
		hr = D3DCompileFromFile(shaderFile, 0, 0, "ps_main", "ps_5_0", 0, 0, bytecode.ptrForInit(), errorMsg.ptrForInit());
		Util::throwIfError(hr);

		dev->CreatePixelShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, _cpPixelShader.ptrForInit());
		Util::throwIfError(hr);
	}


	//ctx->VSSetShader(_testVertexShader, 0, 0);
	//ctx->PSSetShader(_testPixelShader, 0, 0);
}

void RenderShader_DX11::_reflect(Span<const u8> bytecode_)
{
	ComPtr<ID3D11ShaderReflection> cpReflection;
	auto hr = D3DReflect(bytecode_.data(), bytecode_.size(), IID_PPV_ARGS(cpReflection.ptrForInit()));
	Util::throwIfError(hr);

	D3D11_SHADER_DESC desc;
	cpReflection->GetDesc(&desc);

	desc.InputParameters;

	SGE_DUMP_VAR(desc.InputParameters);
		
}

void RenderShader_DX11::_reflect_inputLayout(ComPtr<ID3D11ShaderReflection> cpReflection_, ComPtr<ID3DBlob> cpBytecode_, D3D11_SHADER_DESC& desc_)
{
	auto inputCounts = desc_.InputParameters;

	// Read input layout description from shader info
	Vector_<D3D11_INPUT_ELEMENT_DESC, 12> inputLayoutDesc;
	for (u32 i = 0; i < inputCounts; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		cpReflection_->GetInputParameterDesc(i, &paramDesc);

		// fill out input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		// determine DXGI format
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		//save element desc
		inputLayoutDesc.push_back(elementDesc);
	}



	/*auto* dev = _pRenderer->d3dDevice();
	auto hr = dev->CreateInputLayout(inputDesc.data()
		, static_cast<UINT>(inputDesc.size())
		, _cpTestVertexShaderBytecode->GetBufferPointer()
		, _cpTestVertexShaderBytecode->GetBufferSize()
		, outLayout.ptrForInit());
	Util::throwIfError(hr);*/
}


}