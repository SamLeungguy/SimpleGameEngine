
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
}

void HLSLCompiler::_compile()
{
	auto srcCodeSize = _lexer._sourceRemain.size() * sizeof(char);
	Span<const u8> srcCode(reinterpret_cast<const u8*>(_lexer._sourceRemain.data()), srcCodeSize);

	int iPass = 0;
	for (auto& sp : _spParsedInfo->passInfoSPtrs)
	{
		_compile_shaders(sp, ShaderType::Vertex, srcCode, iPass);
		_compile_shaders(sp, ShaderType::Pixel, srcCode, iPass);
		//_compile_shaders(sp, ShaderType::Gemotry, srcCode, iPass);
		//_compile_shaders(sp, ShaderType::Tessellation, srcCode, iPass);
		//_compile_shaders(sp, ShaderType::Compute, srcCode, iPass);

		iPass++;
	}
}

void HLSLCompiler::_compile_shaders(SPtr<PassInfo>& spPassInfo_, ShaderType type_, Span<const u8> srcCode_, int passIndex_)
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

	// writeBinToFile
	MemMapFile mm;
	mm.openWrite(tmpStr, false);
	Span<const u8> data(reinterpret_cast<const u8*>(_cpBytecode->GetBufferPointer()), _cpBytecode->GetBufferSize());
	mm.writeBytes(data);

	_shaderInfoSPtrs.emplace_back(new ShaderInfo());
	_reflect(_cpReflection, _cpBytecode);
}

void HLSLCompiler::_reflect(ComPtr<ID3D11ShaderReflection>& cpReflection_, ComPtr<ID3DBlob>& cpBlob_)
{
	ComPtr<ID3D11ShaderReflection>& cpReflection = cpReflection_;

	auto hr = D3DReflect(cpBlob_->GetBufferPointer(), cpBlob_->GetBufferSize(), IID_PPV_ARGS(cpReflection.ptrForInit()));
	Util::throwIfError(hr);

	auto& spShaderInfo = _shaderInfoSPtrs.back();

	D3D11_SHADER_DESC desc;
	cpReflection->GetDesc(&desc);

	auto& threadSize = spShaderInfo->_csWorkGroupSize;
	cpReflection->GetThreadGroupSize(&threadSize.x, &threadSize.y, &threadSize.z);

	spShaderInfo->_version = desc.Version;

	{
		_reflect_inputs				(cpBlob_, cpReflection, desc);
		_reflect_uniformBuffers		(cpBlob_, cpReflection, desc);	
		_reflect_textures			(cpBlob_, cpReflection, desc);
		_reflect_samplers			(cpBlob_, cpReflection, desc);
		_reflect_storageBuffers		(cpBlob_, cpReflection, desc);	
	}

#if 0
	{
		auto res_count = desc.BoundResources;
		if (res_count)
		{
			_D3D11_SHADER_INPUT_BIND_DESC bind_desc;
			cpReflection->GetResourceBindingDesc(0, &bind_desc);
			SGE_DUMP_VAR(bind_desc.Name);
		}
		//GetResourceBindingDesc
		//cb_desc
	}
#endif // 0

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

		InputAttribute input;
		//input.name = input_desc.SemanticName;
		Vertex_Semantic semantic;
		auto semanticType = getSemanticType(input_desc.SemanticName);
		UINT semanticIndex = input_desc.SemanticIndex;
		input.semantic_type = Vertex_SemanticUtil::make(semanticType, semanticIndex);
		input.data_type = Util::getRenderDataTypeBySemanticName(input_desc.SemanticName);

#if HLSL_COMPILER_DEBUG 
		const auto* data_type = RenderDataTypeUtil::toString(input.data_type);
		SGE_LOG("{}: {}", "input data", data_type);
#endif
	}
}

void HLSLCompiler::_reflect_uniformBuffers(ComPtr<ID3DBlob>& cpBlob_, ComPtr<ID3D11ShaderReflection>& cpReflection_, D3D11_SHADER_DESC& desc_)
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

		spShaderInfo->_cBufferLayoutSPtrs.emplace_back(new CBufferLayout);
		auto& spCbufferLayout = spShaderInfo->_cBufferLayoutSPtrs.back();

		//SGE_DUMP_VAR(cb_desc.Size);
		spCbufferLayout->stride = Math::byteToBit(cb_desc.Size);

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
			element.offset	= Math::byteToBit(var_desc.StartOffset);
			element.size	= Math::byteToBit(var_desc.Size);
			
			if (type_desc.Class != D3D_SVC_STRUCT)
			{
				element.dataType = Util::getRenderDataType(type_desc.Name);
#if HLSL_COMPILER_DEBUG
				//SGE_DUMP_VAR(type_desc.Name, var_desc.Name, var_desc.Size, var_desc.StartOffset);
				const auto* dataTypeStr = RenderDataTypeUtil::toString(element.dataType);
				SGE_DUMP_VAR(element.name, dataTypeStr, element.offset, element.size);
#endif // _DEBUG
			}

			// check is struct
			_appendCBufferLayout_if_is_strcut(var_type, var_desc, type_desc, iVar);
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

void HLSLCompiler::_appendCBufferLayout_if_is_strcut(ID3D11ShaderReflectionType* pVar_type_, D3D11_SHADER_VARIABLE_DESC& var_desc_, D3D11_SHADER_TYPE_DESC& type_desc_, int startIndex_)
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
		mElement.offset = structStartOffset + Math::byteToBit(member_type_desc.Offset);

		auto array_element_count = member_type_desc.Elements ? member_type_desc.Elements : 1;
		mElement.size = array_element_count * RenderDataTypeUtil::getBitSize(mElement.dataType);

#if HLSL_COMPILER_DEBUG
		//SGE_DUMP_VAR(mElement.name, member_type_desc.Name, structStartOffset + member_type_desc.Offset, member_type_desc.Elements);
		const auto* dataTypeStr = RenderDataTypeUtil::toString(mElement.dataType);
		SGE_DUMP_VAR(mElement.name, dataTypeStr, mElement.offset, mElement.size);
#endif // HLSL_COMPILER_DEBUG

		spCbufferLayout->elements.emplace_back();
	}
}

void HLSLCompiler::_toOutputShaderPath(TempString& out_, ShaderType type_, int passIndex_)
{
	out_.clear();
	out_.append(_outputFilepath.c_str());

	out_.append("pass");
	out_.append(std::to_string(passIndex_).c_str());
	out_.append("_");
	out_.append(s_shaderTypeStrs[enumInt(type_)]);
	out_.append(".bin");
}

}

#endif
