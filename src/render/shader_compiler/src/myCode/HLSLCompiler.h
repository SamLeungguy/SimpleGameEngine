#if 0
#pragma once

#if SGE_RENDER_HAS_DX11
#include <sge_render/backend/dx11/Render_Common_DX11.h>

#include "ShaderParser.h"

#include <sge_render/material/ShaderInfo.h>

#include <nlohmann/json.hpp>

namespace sge {

class HLSLCompiler : public ShaderParser
{
	using Util = DX11Util;
	using PassInfo = ShaderParsedInfo::PassInfo;

	using json = nlohmann::ordered_json;
public:
	static constexpr char* s_shaderTypeStrs[] = { "vs", "ps", "gs", "ts", "cs" };
	static constexpr char* s_version = "_5_0";

public:
	void compile(StrView filename_);

private:
	void _compile();
	void _compile_shaders(SPtr<PassInfo>& spPassInfo_, RenderShaderType type_, Span<const u8> srcCode_, int passIndex_);

	void _reflect(RenderShaderType type_, ComPtr<ID3D11ShaderReflection>& cpReflection_, ComPtr<ID3DBlob>& cpBlob_);
	void _reflect_inputs(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_);
	void _reflect_cBuffers(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_);
	void _reflect_appendCBufferLayout_if_is_strcut(ID3D11ShaderReflectionType* pVar_type_, D3D11_SHADER_VARIABLE_DESC& var_desc_, D3D11_SHADER_TYPE_DESC& type_desc_, int startIndex_);
	void _reflect_textures(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_);
	void _reflect_samplers(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_);
	void _reflect_storageBuffers(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_);

	void _writeToJsonFile(StrView outputPath_);
	void _writeToJsonFile_inputs(json& out_, SPtr<ShaderInfo>& spInfo_);
	void _writeToJsonFile_params(json& out_, SPtr<ShaderInfo>& spInfo_);
	void _writeToJsonFile_cBuffers(json& out_, SPtr<ShaderInfo>& spInfo_);
	void _writeToJsonFile_textures(json& out_, SPtr<ShaderInfo>& spInfo_);
	void _writeToJsonFile_samplers(json& out_, SPtr<ShaderInfo>& spInfo_);
	void _writeToJsonFile_storageBuffers(json& out_, SPtr<ShaderInfo>& spInfo_);

	void _toOutputShaderPath(TempString& out_, RenderShaderType type_, int passIndex_);

	void _error(StrView msg_, StrView value_ = {});
private:
	String _filename;
	String _filepath;
	String _outputFilepath;

	ComPtr<ID3D11ShaderReflection> _cpReflection;

	ComPtr<ID3DBlob>	_cpBytecode;
	ComPtr<ID3DBlob>	_cpErrorMsg;

	SPtr<ShaderParsedInfo>_spParsedInfo;

	// should store RenderPass, then no need the passIndex 
	Vector<SPtr<ShaderInfo>> _shaderInfoSPtrs;
};

inline void HLSLCompiler::_error(StrView msg_, StrView value_) { throw SGE_ERROR("{} {}", msg_, value_); }

}
#endif // SGE_HAS_DX11

#endif // 0
