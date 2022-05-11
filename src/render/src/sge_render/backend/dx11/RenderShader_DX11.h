#pragma once

#include "Render_Common_DX11.h"
#include <sge_render/material/RenderShader.h>

namespace sge {

class RenderShader_DX11 : public RenderShader {
	using Base = RenderShader;
	using Util = DX11Util;
public:
	RenderShader_DX11(CreateDesc& desc_);
	~RenderShader_DX11() = default;

	void destroy();

	void reload();

protected:
	void _setInputLayout();

	void _reflect(Span<const u8> bytecode_);
	void _reflect_inputLayout(ComPtr<ID3D11ShaderReflection> cpReflection_, ComPtr<ID3DBlob> cpBytecode_, D3D11_SHADER_DESC& desc_);

private:
	ComPtr<DX11_ID3DVertexShader>		_cpVertexShader;
	ComPtr<DX11_ID3DBlob>				_cpVertexShaderBytecode;
	ComPtr<DX11_ID3DPixelShader>		_cpPixelShader;

	VectorMap<const VertexLayout*, ComPtr<DX11_ID3DInputLayout>> _inputLayouts;
};

}