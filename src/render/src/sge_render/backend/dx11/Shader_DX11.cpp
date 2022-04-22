#include "Shader_DX11.h"
#include "Renderer_DX11.h"

#if SGE_RENDER_HAS_DX11

namespace sge {

Shader_DX11::Shader_DX11(CreateDesc& desc_)
	:
	Base(desc_)
{
	_filepath = desc_.filepath;

	const wchar_t* shaderFile = _filepath.data();

	auto* dev = Renderer_DX11::current()->d3dDevice();
	auto* ctx = Renderer_DX11::current()->d3dDeviceContext();

	HRESULT hr;

	if (!_cpVertexShader) {
		ComPtr<ID3DBlob>	bytecode;
		ComPtr<ID3DBlob>	errorMsg;
		hr = D3DCompileFromFile(shaderFile, 0, 0, "vs_main", "vs_4_0", 0, 0, bytecode.ptrForInit(), errorMsg.ptrForInit());
		Util::throwIfError(hr);

		hr = dev->CreateVertexShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, _cpVertexShader.ptrForInit());
		Util::throwIfError(hr);

		Vector<D3D11_INPUT_ELEMENT_DESC> inputDescs;

		if (desc_.pVertexLayout)
		{
			auto& vertexLayout = *desc_.pVertexLayout;

			inputDescs.reserve(vertexLayout.getCount());

			for (auto it = vertexLayout.begin(); it != vertexLayout.end(); it++)
			{
				D3D11_INPUT_ELEMENT_DESC inputDesc;
				inputDesc.SemanticName				= Util::toSemanticName_DX11(it->sematic).data();
				inputDesc.SemanticIndex				= 0;
				inputDesc.Format					= Util::toRenderDataFormat_DX11(it->type);
				inputDesc.InputSlot					= 0;
				inputDesc.AlignedByteOffset			= it->offset;
				inputDesc.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
				inputDesc.InstanceDataStepRate		= 0;

				//SGE_LOG("SemanticName: {}, offset: {}", inputDesc.SemanticName, inputDesc.AlignedByteOffset);

				inputDescs.push_back(inputDesc);
			}
		}
		else
			SGE_ASSERT(0, "");

		SGE_ASSERT(inputDescs.size() <= 0, "");

		//SGE_LOG("inputDescs.size(): {}", inputDescs.size());

		hr = dev->CreateInputLayout(inputDescs.data()
			//, sizeof(inputDesc) / sizeof(inputDesc[0])
			, inputDescs.size()
			, bytecode->GetBufferPointer()
			, bytecode->GetBufferSize()
			, _cpInputLayout.ptrForInit());
		Util::throwIfError(hr);
	}

	if (!_cpPixelShader) {
		ComPtr<ID3DBlob>	bytecode;
		ComPtr<ID3DBlob>	errorMsg;
		hr = D3DCompileFromFile(shaderFile, 0, 0, "ps_main", "ps_4_0", 0, 0, bytecode.ptrForInit(), errorMsg.ptrForInit());
		Util::throwIfError(hr);

		dev->CreatePixelShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, _cpPixelShader.ptrForInit());
		Util::throwIfError(hr);
	}

}

Shader_DX11::~Shader_DX11()
{

}

void Shader_DX11::bind()
{
	auto* ctx = Renderer_DX11::current()->d3dDeviceContext();

	ctx->IASetInputLayout(_cpInputLayout);
	ctx->VSSetShader(_cpVertexShader, 0, 0);
	ctx->PSSetShader(_cpPixelShader, 0, 0);
}

}


#endif // SGE_RENDER_HAS_DX11