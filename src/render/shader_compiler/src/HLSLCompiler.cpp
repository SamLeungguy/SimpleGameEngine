
#if SGE_RENDER_HAS_DX11
#include "HLSLCompiler.h"

#define HLSL_COMPILER_DEBUG 0

namespace sge {

void HLSLCompiler::compile(StrView filename_)
{
	// TODO: correct temp
#if 1
	_filename = filename_;
	_filepath = "Assets/Shaders/";
	_filepath.append(filename_.begin(), filename_.end());
	_outputFilepath = "LocalTemp/Imported/";
	_outputFilepath.append(filename_.begin(), filename_.end());
	//_outputFilepath.append("/dx11/");

	TempStringW filenameW;
	UtfUtil::convert(filenameW, filename_);
	TempStringW tempOutputFilepath = L"LocalTemp/";
	bool ret = FileStream::s_createDirectory(tempOutputFilepath);
	tempOutputFilepath.append(L"Imported/");
	ret = FileStream::s_createDirectory(tempOutputFilepath);
	tempOutputFilepath.append(filenameW);
	ret = FileStream::s_createDirectory(tempOutputFilepath);
	tempOutputFilepath.append(L"/dx11/");
	ret = FileStream::s_createDirectory(tempOutputFilepath);
#endif // 1

	_spParsedInfo.reset(new ShaderParsedInfo());

	ShaderParser::parse(*_spParsedInfo.ptr(), _filepath);
	ShaderParser::writeToJsonFile(_outputFilepath);
	_shaderInfoSPtrs.reserve(2);

	_outputFilepath.append("/dx11/");

	_compile();

	_writeToJsonFile(_outputFilepath);
}

void HLSLCompiler::_compile()
{
	auto srcCodeSize = _lexer._sourceRemain.size() * sizeof(char);
	Span<const u8> srcCode(reinterpret_cast<const u8*>(_lexer._sourceRemain.data()), srcCodeSize);

	int iPass = 0;
	for (auto& sp : _spParsedInfo->passInfoSPtrs)
	{

		_compile_shaders(sp, RenderShaderType::Vertex, srcCode, iPass);
		_compile_shaders(sp, RenderShaderType::Pixel, srcCode, iPass);
		//_compile_shaders(sp, ShaderType::Gemotry, srcCode, iPass);
		//_compile_shaders(sp, ShaderType::Tessellation, srcCode, iPass);
		//_compile_shaders(sp, ShaderType::Compute, srcCode, iPass);

		iPass++;
	}
}

void HLSLCompiler::_compile_shaders(SPtr<PassInfo>& spPassInfo_, RenderShaderType type_, Span<const u8> srcCode_, int passIndex_)
{
	auto type = enumInt(type_);
	const auto& targetStr = spPassInfo_->shaderFuncs[type];

	if (targetStr.empty())
		_error("no shader is found in pass", std::to_string(passIndex_).c_str());

	TempString tmpStr(s_shaderTypeStrs[type]);
	tmpStr.append(s_version);

	// _compile_shader
	auto hr = D3DCompile(srcCode_.data(), srcCode_.size(), nullptr, nullptr, nullptr, targetStr.c_str(), tmpStr.c_str(), 0, 0, _cpBytecode.ptrForInit(), _cpErrorMsg.ptrForInit());
	Util::throwIfError(hr);

	_toOutputShaderPath(tmpStr, type_, passIndex_);
	tmpStr.append(".bin");

	// writeBinToFile
	MemMapFile mm;
	mm.openWrite(tmpStr, false);
	Span<const u8> data(reinterpret_cast<const u8*>(_cpBytecode->GetBufferPointer()), _cpBytecode->GetBufferSize());
	mm.writeBytes(data);

	_shaderInfoSPtrs.emplace_back(new ShaderInfo());
	auto& spShaderInfo = _shaderInfoSPtrs.back();
	spShaderInfo->_passIndex = passIndex_;
	spShaderInfo->_spShaderParsedInfo.reset(_spParsedInfo);
	_reflect(type_, _cpReflection, _cpBytecode);

}

void HLSLCompiler::_reflect(RenderShaderType type_, ComPtr<ID3D11ShaderReflection>& cpReflection_, ComPtr<ID3DBlob>& cpBlob_)
{
	ComPtr<ID3D11ShaderReflection>& cpReflection = cpReflection_;

	auto hr = D3DReflect(cpBlob_->GetBufferPointer(), cpBlob_->GetBufferSize(), IID_PPV_ARGS(cpReflection.ptrForInit()));
	Util::throwIfError(hr);

	auto& spShaderInfo = _shaderInfoSPtrs.back();
	spShaderInfo->_type = type_;
	
	D3D11_SHADER_DESC desc;
	cpReflection->GetDesc(&desc);

	auto& threadSize = spShaderInfo->_csWorkGroupSize;
	cpReflection->GetThreadGroupSize(&threadSize.x, &threadSize.y, &threadSize.z);

	spShaderInfo->_version = Util::getShaderVersion(desc.Version);

	{
		_reflect_inputs				(cpBlob_, cpReflection, desc);
		_reflect_cBuffers		(cpBlob_, cpReflection, desc);	
		_reflect_textures			(cpBlob_, cpReflection, desc);
		_reflect_samplers			(cpBlob_, cpReflection, desc);
		_reflect_storageBuffers		(cpBlob_, cpReflection, desc);	
	}
}

void HLSLCompiler::_reflect_inputs(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_)
{
	auto& desc = desc_;
#if HLSL_COMPILER_DEBUG 
	SGE_DUMP_VAR(desc.InputParameters);
#endif // HLSL_COMPILER_DEBUG 

	auto input_count = desc.InputParameters;

	if (!input_count)
		_error("no input count");

	auto& spShaderInfo = _shaderInfoSPtrs.back();
	spShaderInfo->_inputs.reserve(input_count);

	for (UINT i = 0; i < input_count; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC input_desc;
		cpReflection_->GetInputParameterDesc(i, &input_desc);

		spShaderInfo->_inputs.emplace_back();
		auto& input = spShaderInfo->_inputs.back();

		//input.name = input_desc.SemanticName;
		//Vertex_Semantic semantic;
		auto semanticType = Util::getSemanticType(input_desc.SemanticName);
		auto semanticIndex = static_cast<u8>(input_desc.SemanticIndex);
		input.semantic = Vertex_SemanticUtil::make(semanticType, semanticIndex);
		input.dataType = Util::getRenderDataTypeBySemanticName(input_desc.SemanticName);

		// TODO: remove tmp
		input.semanticStr = input_desc.SemanticName;
	}

#if HLSL_COMPILER_DEBUG 
		const auto* data_type = RenderDataTypeUtil::toString(input.dataType);
		SGE_LOG("{}: {}", "input data", dataType);
#endif
}

void HLSLCompiler::_reflect_cBuffers(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_)
{
	auto& desc = desc_;

#if HLSL_COMPILER_DEBUG
	SGE_DUMP_VAR(desc.ConstantBuffers);
#endif // HLSL_COMPILER_DEBUG

	auto cb_count = desc.ConstantBuffers;

	if (!cb_count)
		return;

	auto& spShaderInfo = _shaderInfoSPtrs.back();
	spShaderInfo->_cBufferLayoutSPtrs.reserve(cb_count);

	for (UINT iCb = 0; iCb < cb_count; iCb++)
	{
		auto* pCb = cpReflection_->GetConstantBufferByIndex(iCb);
		_D3D11_SHADER_BUFFER_DESC cb_desc;
		auto hr = pCb->GetDesc(&cb_desc);
		Util::throwIfError(hr);

		D3D11_SHADER_INPUT_BIND_DESC res_bind_desc;
		cpReflection_->GetResourceBindingDescByName(cb_desc.Name, &res_bind_desc);

		spShaderInfo->_cBufferLayoutSPtrs.emplace_back(new CBufferLayout);
		auto& spCbufferLayout = spShaderInfo->_cBufferLayoutSPtrs.back();

		//SGE_DUMP_VAR(cb_desc.Size);
		spCbufferLayout->dataSize = Math::byteToBit(cb_desc.Size);
		spCbufferLayout->bindPoint = res_bind_desc.BindPoint;
		spCbufferLayout->bindCount = res_bind_desc.BindCount;

		auto var_count = cb_desc.Variables;
		spCbufferLayout->elements.reserve(var_count);

#if HLSL_COMPILER_DEBUG
		SGE_DUMP_VAR(spCbufferLayout->stride);
		SGE_DUMP_VAR(var_count);
#endif // HLSL_COMPILER_DEBUG

		for (UINT iVar = 0; iVar < var_count; iVar++)
		{
			auto* var = pCb->GetVariableByIndex(iVar);

			D3D11_SHADER_VARIABLE_DESC var_desc;
			hr = var->GetDesc(&var_desc);
			Util::throwIfError(hr);

			auto* var_type = var->GetType();
			D3D11_SHADER_TYPE_DESC type_desc;
			var_type->GetDesc(&type_desc);

			spCbufferLayout->elements.push_back();
			auto& element = spCbufferLayout->elements.back();

			element.hash	= Math::hashStr(var_desc.Name);
			element.name	= var_desc.Name;
			element.offset	= Math::byteToBit(static_cast<u16>(var_desc.StartOffset));
			element.size	= Math::byteToBit(static_cast<u16>(var_desc.Size));

			if (type_desc.Class != D3D_SVC_STRUCT)
			{
				element.dataType = Util::getRenderDataType(type_desc.Name);
				continue;
#if HLSL_COMPILER_DEBUG
				//SGE_DUMP_VAR(type_desc.Name, var_desc.Name, var_desc.Size, var_desc.StartOffset);
				const auto* dataTypeStr = RenderDataTypeUtil::toString(element.dataType);
				SGE_DUMP_VAR(element.name, dataTypeStr, element.offset, element.size);
#endif // _DEBUG
			}

			// check is struct
			_reflect_appendCBufferLayout_if_is_strcut(var_type, var_desc, type_desc, iVar);
		}
	}
}

void HLSLCompiler::_reflect_textures(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_)
{
}

void HLSLCompiler::_reflect_samplers(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_)
{
}

void HLSLCompiler::_reflect_storageBuffers(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_)
{
}

void HLSLCompiler::_reflect_appendCBufferLayout_if_is_strcut(ID3D11ShaderReflectionType* pVar_type_, D3D11_SHADER_VARIABLE_DESC& var_desc_, D3D11_SHADER_TYPE_DESC& type_desc_, int startIndex_)
{
	auto* var_type = pVar_type_;
	auto& var_desc = var_desc_;
	auto& type_desc = type_desc_;

	auto member_count = type_desc.Members;

	if (member_count == 0)
		return;

#if HLSL_COMPILER_DEBUG
	SGE_DUMP_VAR(type_desc.Elements, member_count);
#endif // HLSL_COMPILER_DEBUG

	if (member_count > 0 && type_desc.Elements > 0)
		_error("struct of array is not support");

	auto& spShaderInfo = _shaderInfoSPtrs.back();
	auto& spCbufferLayout = spShaderInfo->_cBufferLayoutSPtrs.back();
	spCbufferLayout->elements.reserve(spCbufferLayout->elements.size() + member_count - 1);
	int startIndex = startIndex_;
	const auto* structName = var_desc.Name;
	auto structStartOffset = Math::byteToBit(var_desc.StartOffset);

	TempString tmpString(structName);

	for (UINT iMember = 0; iMember < member_count; iMember++)		// is struct if count > 0
	{
		auto* member_type = var_type->GetMemberTypeByIndex(iMember);

		D3D11_SHADER_TYPE_DESC member_type_desc;
		member_type->GetDesc(&member_type_desc);

		auto& mElement = spCbufferLayout->elements[startIndex + iMember];

		// set name -> struct.dataName
		tmpString.clear();
		tmpString.append(structName);
		tmpString.append(".");
		tmpString.append(var_type->GetMemberTypeName(iMember));

		mElement.dataType = Util::getRenderDataType(member_type_desc.Name);

		mElement.hash = Math::hashStr(mElement.name.c_str());
		mElement.name = tmpString;
		mElement.offset = static_cast<u16>(structStartOffset + Math::byteToBit(member_type_desc.Offset));

		u32 array_element_count = member_type_desc.Elements ? member_type_desc.Elements : 1;
		mElement.size = array_element_count * RenderDataTypeUtil::getBitSize(mElement.dataType);

#if HLSL_COMPILER_DEBUG
		//SGE_DUMP_VAR(mElement.name, member_type_desc.Name, structStartOffset + member_type_desc.Offset, member_type_desc.Elements);
		const auto* dataTypeStr = RenderDataTypeUtil::toString(mElement.dataType);
		SGE_DUMP_VAR(mElement.name, dataTypeStr, mElement.offset, mElement.size);
#endif // HLSL_COMPILER_DEBUG

		if (iMember < member_count - 1)		// last element no need to emplace_back
			spCbufferLayout->elements.emplace_back();
	}
}

void HLSLCompiler::_writeToJsonFile(StrView outputPath_)
{
	TempString tmpString;

	// identity each pass index
	for (auto& spInfo : _shaderInfoSPtrs)		// vs / ps / gs / ts / cs
	{
		// same json file
		// this vs / ps info to json
		json j;
		auto type = enumInt(spInfo->_type);
		
		tmpString.clear();
		tmpString.append(s_shaderTypeStrs[type]);
		tmpString.append(s_version);
		j["profile"] = tmpString.c_str();

		j["csWorkgroupSize"] = { spInfo->_csWorkGroupSize.x, spInfo->_csWorkGroupSize.y, spInfo->_csWorkGroupSize.z };

		_writeToJsonFile_inputs(j, spInfo);
		_writeToJsonFile_params(j, spInfo);
		_writeToJsonFile_cBuffers(j, spInfo);
		_writeToJsonFile_textures(j, spInfo);
		_writeToJsonFile_samplers(j, spInfo);
		_writeToJsonFile_storageBuffers(j, spInfo);

		MemMapFile mm;

		tmpString.clear();
		tmpString.append(outputPath_.begin(), outputPath_.end());
		_toOutputShaderPath(tmpString, spInfo->_type, spInfo->_passIndex);
		tmpString.append(".json");
		mm.openWrite(tmpString, false);

		auto str = j.dump(4);
		Span<const u8> data(reinterpret_cast<const u8*>(str.data()), str.size());
		mm.writeBytes(data);
	}
}

void HLSLCompiler::_writeToJsonFile_inputs(json& out_, SPtr<ShaderInfo>& spInfo_)
{
	auto& spInfo = spInfo_;
	auto& j = out_;

	TempString tmpString;

	j["inputs"];
	for (auto& input : spInfo->_inputs)
	{
		auto& e = j["inputs"];
		// TODO: remove tmp, // shuold handle SV_ properly
		if (StringUtil::hasChar(input.semanticStr, '_'))
		{
			e.push_back(
				{
					{"name", ""},
					{"attrId",    input.semanticStr.c_str()},
					{"dataType", RenderDataTypeUtil::toString(input.dataType)}
				}
			);
		}
		else
		{
			auto vs_type = Vertex_SemanticUtil::getType(input.semantic);
			auto vs_index = Vertex_SemanticUtil::getIndex(input.semantic);

			auto semanticNameStr = Util::getDxSemanticName(vs_type);
			tmpString.clear();
			tmpString.append(semanticNameStr);
			tmpString.append(std::to_string(vs_index).c_str());

			e.push_back(
				{
					{"name", ""},
					{"attrId",  tmpString.c_str() },			// shuold handle SV_
					{"dataType", RenderDataTypeUtil::toString(input.dataType)}
				}
			);
		}
	}
}

void HLSLCompiler::_writeToJsonFile_params(json& out_, SPtr<ShaderInfo>& spInfo_)
{
	auto& j = out_;
	j["params"] = {};
}

void HLSLCompiler::_writeToJsonFile_cBuffers(json& out_, SPtr<ShaderInfo>& spInfo_)
{
	auto& spInfo = spInfo_;
	auto& j = out_;

	// write cbuffer layout
	j["cBuffers"] = { {} };
	auto& j_uniform = j["cBuffers"];

	int i = 0;
	for (auto& spCBufferLayout : spInfo->_cBufferLayoutSPtrs)
	{
		j_uniform[i] = {
			{"name",			"$Globals"},
			{"bindPoint",		spCBufferLayout->bindPoint},
			{"bindCount",		spCBufferLayout->bindCount},
			{"dataSize",		spCBufferLayout->dataSize},
			{"variableCount",	spCBufferLayout->elements.size()},
			{"variables", {}}
		};

		for (auto& element : spCBufferLayout->elements)
		{
			auto& value = j_uniform.back()["variables"];
			value.push_back({
				{"name",		element.name.c_str()},
				{"offset",		element.offset},
				{"dataType",	RenderDataTypeUtil::toString(element.dataType)},
				{"size",		element.size},
			});
		}
		i++;
	}
}

void HLSLCompiler::_writeToJsonFile_textures(json& out_, SPtr<ShaderInfo>& spInfo_)
{
	//auto& spInfo = spInfo_;
	auto& j = out_;

	j["textures"] = {
		/*{"name", ""},
		{"bindPoint", 0},
		{"bindCount", 0},
		{"dataSize", 0}*/
	};
}

void HLSLCompiler::_writeToJsonFile_samplers(json& out_, SPtr<ShaderInfo>& spInfo_)
{
	//auto& spInfo = spInfo_;
	auto& j = out_;

	j["samplers"] = {
		/*{"name", ""},
		{"bindPoint", 0},
		{"bindCount", 0},
		{"dataSize", 0}*/
	};
}

void HLSLCompiler::_writeToJsonFile_storageBuffers(json& out_, SPtr<ShaderInfo>& spInfo_)
{
	auto& j = out_;

	j["storageBuffers"] = {
		/*{"name", ""},
		{"bindPoint", 0},
		{"bindCount", 0},
		{"dataSize", 0}*/
	};
}

void HLSLCompiler::_toOutputShaderPath(TempString& out_, RenderShaderType type_, int passIndex_)
{
	out_.clear();
	out_.append(_outputFilepath.c_str());

	out_.append("pass");
	out_.append(std::to_string(passIndex_).c_str());
	out_.append("_");
	out_.append(s_shaderTypeStrs[enumInt(type_)]);
	// eg. pass0_vs
}

}

#endif
