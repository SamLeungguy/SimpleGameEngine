#pragma once

#if SGE_RENDER_HAS_DX11
#include <sge_render/backend/dx11/Render_Common_DX11.h>

#include "ShaderParser.h"

#include <sge_render/material/ShaderInfo.h>

namespace sge {

class HLSLCompiler : public ShaderParser
{
	using Util = DX11Util;
public:

	void compile(StrView filename_);

private:
	void _compile();
	void _reflect(ComPtr<ID3DBlob>& cpBlob_);
	void _reflect_inputs		(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_);
	void _reflect_uniformBuffers(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_);
	void _reflect_textures		(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_);
	void _reflect_samplers		(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_);
	void _reflect_storageBuffers(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_);

	void _appendCBufferLayout_if_is_strcut(ID3D11ShaderReflectionType* pVar_type_, D3D11_SHADER_VARIABLE_DESC& var_desc_, D3D11_SHADER_TYPE_DESC& type_desc_, int startIndex_);

	void _error(StrView msg_);
private:
	String _filename;
	String _filepath;
	String _outputFilepath;

	ShaderParsedInfo _parsedInfo;

	Vector<SPtr<ShaderInfo>> _shaderInfoSPtrs;
};

inline void HLSLCompiler::_error(StrView msg_) { throw SGE_ERROR("{}", msg_); }

}
#endif // SGE_HAS_DX11
