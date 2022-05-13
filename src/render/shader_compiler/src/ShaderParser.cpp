#include "ShaderParser.h"

#include <sge_render/backend/dx11/Render_Common_DX11.h>

#include <nlohmann/json.hpp>

#define _DEBUG_SHADER_PARSER 0

namespace sge {

bool ShaderParser::parse(ShaderParsedInfo& outInfo_, StrView filepath_)
{
	auto tokens = _lexer.parse(filepath_);
	_pTokens = &tokens;
	_pInfo = &outInfo_;

	_parse();

#if _DEBUG_SHADER_PARSER
	for (auto& e : _pInfo->passInfoSPtrs)
	{
		//SGE_DUMP_VAR(e->name, e->vsFunc, e->psFunc, e->queue);
	}
#endif // _DEBUG_SHADER_PARSER

	return _isSuccess;
}

void ShaderParser::writeToJsonFile(StrView outputPath_)
{
	if (!_pInfo)
		_error("not yet parsed");

	using json = nlohmann::ordered_json;
	json j;
	
	j["shaderName"] = _pInfo->shaderName.c_str();
	j["passCount"] = _pInfo->passCount;

	// serialize properties
	{
		auto& elements = _pInfo->spPropertiesLayout->elements;
		
		const auto* fieldName = "properties";

		j[fieldName] = {
				{"dataSize", _pInfo->spPropertiesLayout->stride}, 
				{"variables", {}}
		};

		for (int i = 0; i < elements.size(); ++i)
		{
			auto& e = j[fieldName]["variables"];
			e.push_back({
					{"name", elements[i].name.c_str()}, 
					{"offset", elements[i].offset}, 
					{"dataType", RenderDataTypeUtil::toString(elements[i].dataType)},
					{"defaultValue", reinterpret_cast<char*>(_pInfo->propertiesDefaultData.data() + elements[i].offset)}
			});
		}
	}

	// serialize premutation
	{
		auto& elements = _pInfo->spPremutationLayout->elements;
		const auto* fieldName = "premutation";

		j[fieldName] = {
				{"dataSize", _pInfo->spPremutationLayout->stride},
				{"variables", {}
			}};
	}
	
	// serialize pass
	{
		const auto* fieldName = "pass"; 

		j[fieldName] = {
				{"infos", {}}
		};

		for (size_t i = 0; i < _pInfo->passInfoSPtrs.size(); i++)
		{
			auto& spInfo = _pInfo->passInfoSPtrs[i];
			auto& e = j[fieldName]["infos"];

			e.push_back({
				  {"Queue", spInfo->queue.c_str()},
				  {"Cull", spInfo->cull.c_str()},

				  {"BlendRGB", spInfo->blendRGB.c_str()},
				  {"BlendAlpha", spInfo->blendAlpha.c_str()},

				  {"DepthTest", spInfo->depthTest.c_str()},
				  {"DepthWrite", spInfo->depthWrite},

				  {"VsFunc", spInfo->shaderFuncs[enumInt(ShaderType::Vertex)].c_str()},
				  {"PsFunc", spInfo->shaderFuncs[enumInt(ShaderType::Pixel)].c_str()},
			});
		}
	}

	{
		MemMapFile mm;

		TempString tmp_outPath = outputPath_;
		tmp_outPath.append("/shaderInfo.json");

		mm.openWrite(tmp_outPath, false);

		auto str = j.dump(4);
		Span<const u8> data(reinterpret_cast<const u8*>(str.data()), str.size());
		mm.writeBytes(data);
	}
}

void ShaderParser::_parse()
{
	_nextToken();
	_parse_shader_name();

	_nextToken();
	_check_start_parsing_keyword();

	while (!_checkEndOfParse())
	{
		_nextToken();
		_trim_newline();
		if (_checkEndOfParse())
			break;

		_parse_next_keyword();
	}

	if (_pInfo->passCount <= 0)
		_error("no pass found");
}

void ShaderParser::_parse_next_keyword()
{
	_trim_newline();

	if (!_checkTokenType(TokenType::Keyword))
		_error("no keyword found");

	static const size_t strCompares[] = { Math::hashStr("Properties"), Math::hashStr("Premutation"), Math::hashStr("Pass") };

	auto hs = Math::hashStr(_pToken->value);

	if (hs == strCompares[0])
		_parse_properties();
	else if (hs == strCompares[1])
		_parse_permutation();
	else if (hs == strCompares[2])
		_parse_pass();
}

void ShaderParser::_parse_shader_name()
{
	if (_isParsedShaderKeyword)
		_error("already parsed shader name");

	if (!_checkToken(TokenType::Keyword, "Shader"))
		_error("no keyword Shader is found");

	_isParsedShaderKeyword = true;

	_nextToken();
	_trim_newline();

	if (_checkToken(TokenType::Operator, "{"))
		return;

	if (!_checkTokenType(TokenType::String))
		_error("invalid type for shader name");

	_pInfo->shaderName = _pToken->value;
}

void ShaderParser::_parse_properties()
{
	if (_isParsedProperties)
		_error("already parsed properties");
	_isParsedProperties = true;

	_nextToken();
	_check_start_parsing_keyword();

	_pInfo->spPropertiesLayout.reset(new CBufferLayout());
	auto& spPropertiesLayout = _pInfo->spPropertiesLayout;

	u32 _leftCBracketCount = 1;

	u32 dataOffset = 0;

	while (_leftCBracketCount > 0)
	{
		if (_leftCBracketCount < 0)
			_error("invalid count of { or }");

		_nextToken();
		_trim_newline();

		if (_checkToken(TokenType::Operator, "}"))
		{
			_leftCBracketCount--;
			break;
		}

		if (!_checkTokenType(TokenType::Keyword))
			_error("TokenType is not keyword");

		spPropertiesLayout->elements.emplace_back();
		auto& element = spPropertiesLayout->elements.back();

		RenderDataType dataType = RenderDataType::None;
		dataType = DX11Util::getRenderDataType(_pToken->value.c_str());
		if (dataType == RenderDataType::None)
			_error("invalid data type");

		_nextToken();
		if (!_checkTokenType(TokenType::Identifier))
			_error("TokenType is not Identifier");
		
		element.dataType = dataType;
		element.name = _pToken->value;
		element.hash = Math::hashStr(element.name);
		element.offset = dataOffset;
		element.size = RenderDataTypeUtil::getBitSize(dataType);

		spPropertiesLayout->stride += element.size;
		dataOffset += element.size;

		// check is name exist, if then error
		for (size_t i = 0; i < spPropertiesLayout->elements.size() - 1; ++i)
		{
			if (spPropertiesLayout->elements[i].hash == element.hash)
				_error("data name repeated");
		}

#if _DEBUG_SHADER_PARSER
		const auto* dataTypeStr = RenderDataTypeUtil::toString(dataType);
		SGE_DUMP_VAR(dataTypeStr, element.name, element.offset, element.size);
#endif // _DEBUG_SHADER_PARSER

		_nextToken();
		
		_parse_properties_value();
	}

	_check_end_parsing_keyword();
}

void ShaderParser::_parse_permutation()
{
	if (_isParsedPremutation)
		_error("already parsed permutation");
	_isParsedPremutation = true;

	_nextToken();
	_check_start_parsing_keyword();

	u32 _leftCBracketCount = 1;

	u32 dataOffset = 0;

	while (_leftCBracketCount > 0)
	{
		if (_leftCBracketCount < 0)
			_error("invalid count of { or }");

		_nextToken();
		_trim_newline();

		if (_checkToken(TokenType::Operator, "}"))
		{
			_leftCBracketCount--;
			break;
		}


	}

	_pInfo->spPremutationLayout.reset(new CBufferLayout());
	auto& spPremutationLayout = _pInfo->spPremutationLayout;



	_check_end_parsing_keyword();
}

void ShaderParser::_parse_pass()
{
	_pInfo->passCount++;
	_pInfo->passInfoSPtrs.emplace_back(new ShaderParsedInfo::PassInfo);
	auto& spPassInfo = _pInfo->passInfoSPtrs.back();

	_nextToken();
	_trim_newline();

	if (_checkTokenType(TokenType::String))
	{
		spPassInfo->name = _pToken->value;
		_nextToken();
		_trim_newline();
	}
	_check_start_parsing_keyword();
	_nextToken();

	u32 _leftCBracketCount = 1;

	while (_leftCBracketCount > 0)
	{
		if (_leftCBracketCount < 0)
			_error("invalid count of { or }");

		_nextToken();
		_trim_newline();

		if (_checkToken(TokenType::Operator, "}"))
		{
			_leftCBracketCount--;
			break;
		}

		_parse_each_pass();

		_nextToken();
		if (!_checkToken(TokenType::Operator, "\n"))
			_error("no new line after value");

	}

	_check_end_parsing_keyword();
}

void ShaderParser::_check_start_parsing_keyword()
{
	_trim_newline();

	if (!_checkToken(TokenType::Operator, "{"))
		_error("properties no { found");
}

void ShaderParser::_check_end_parsing_keyword()
{
	_trim_newline();

	if (!_checkToken(TokenType::Operator, "}"))
		_error("properties no { found");
}

void ShaderParser::_parse_properties_value()
{
	// start parse value
	// do extrat stuff if is color / texture / float234...
	// if is curlt type , count: 1, 2, 3, 4

	auto& element = _pInfo->spPropertiesLayout->elements.back();
	_pInfo->propertiesDefaultData.resize(element.size + _pInfo->propertiesDefaultData.size());

	if (_checkToken(TokenType::Operator, "\n"))		// default value could be not set
		return;

	if (!_checkToken(TokenType::Operator, "="))
		_error("invalid syntax, no = is found");

	_nextToken();

	u8* pData = _pInfo->propertiesDefaultData.data() + element.offset;

	auto dataTypecount = RenderDataTypeUtil::getCount(element.dataType);
	auto baseType = RenderDataTypeUtil::getBaseType(element.dataType);

	auto parse_ret = false;

	switch (baseType)
	{
		using DataType = sge::RenderDataType;
		case DataType::Int:		parse_ret = _try_parseValue<int>(pData, dataTypecount);				break;
		//case DataType::UInt:	parse_ret = _try_parseValue<u32>(pData, dataTypecount);				break;
		case DataType::Float:	parse_ret = _try_parseValue<float>(pData, dataTypecount);			break;
		//case DataType::SNorm:	parse_ret = _try_parseValue<char>(pData, dataTypecount);			break;
		//case DataType::UNorm:	parse_ret = _try_parseValue<unsigned char>(pData, dataTypecount);	break;
		default: _error("invalid type");
	}
	if (!parse_ret)
		_error("cannot parse properties value");

#if _DEBUG_SHADER_PARSER
	switch (baseType)
	{
		using DataType = sge::RenderDataType;
		case DataType::Int:		SGE_DUMP_VAR(*reinterpret_cast<int*>(pData));	break;
		//case DataType::UInt:	SGE_DUMP_VAR(*reinterpret_cast<u32*>(pData));	break;
		case DataType::Float:	SGE_DUMP_VAR(*reinterpret_cast<float*>(pData));	break;
		//case DataType::SNorm:	SGE_DUMP_VAR(*reinterpret_cast<char*>(pData));	break;
		//case DataType::UNorm:	SGE_DUMP_VAR(*reinterpret_cast<unsigned char*>(pData));	break;
	default: _error("invalid type");
	}
#endif // _DEBUG_SHADER_PARSER

	_nextToken();
	if (!_checkToken(TokenType::Operator, "\n"))
		_error("no new line after value");
}

void ShaderParser::_parse_each_pass()
{
	auto& spPassInfo = _pInfo->passInfoSPtrs.back();

	if (!_checkTokenType(TokenType::Keyword))
		_error("invalid keyword in pass", _pToken->value);

	if (_checkTokenValue("Queue"))
	{
		_nextToken();
		if (!_checkTokenType(TokenType::String))
			_error("Queue error type", _pToken->value);
		spPassInfo->queue = _pToken->value;
	}
	else if (_checkTokenValue("Cull"))
	{
		_nextToken();
		if (!_checkTokenType(TokenType::String))
			_error("Cull error type", _pToken->value);
		spPassInfo->cull = _pToken->value;
	}
	else if (_checkTokenValue("BlendRGB"))
	{
		_nextToken();
		if (!_checkTokenType(TokenType::Keyword))
			_error("BlendRGB error type", _pToken->value);
		//spPassInfo->blendRGB

		_nextToken();
		if (!_checkTokenType(TokenType::Keyword))
			_error("BlendRGB error type", _pToken->value);
		//spPassInfo->blendRGB

		_nextToken();
		if (!_checkTokenType(TokenType::Keyword))
			_error("BlendRGB error type", _pToken->value);
		//spPassInfo->blendRGB
	}
	else if (_checkTokenValue("BlendAlpha"))
	{
		_nextToken();
		if (!_checkTokenType(TokenType::Keyword))
			_error("BlendAlpha error type", _pToken->value);
		//spPassInfo->blendAlpha

		_nextToken();
		if (!_checkTokenType(TokenType::Keyword))
			_error("BlendAlpha error type", _pToken->value);
		//spPassInfo->blendAlpha

		_nextToken();
		if (!_checkTokenType(TokenType::Keyword))
			_error("BlendAlpha error type", _pToken->value);
		//spPassInfo->blendAlpha
	}
	else if (_checkTokenValue("DepthTest"))
	{
		_nextToken();
		if (!_checkTokenType(TokenType::Keyword))
			_error("DepthTest error type", _pToken->value);
		spPassInfo->depthTest = _pToken->value;
	}
	else if (_checkTokenValue("DepthWrite"))
	{
		_nextToken();
		if		(_checkToken(TokenType::Keyword, "true"))		spPassInfo->depthWrite = true;
		else if (_checkToken(TokenType::Keyword, "false"))		spPassInfo->depthWrite = false;
		else
			_error("DepthWrite error value", _pToken->value);
	}
	else if (_checkTokenValue("VsFunc"))
	{
		_nextToken();
		if (!_checkTokenType(TokenType::Identifier))
			_error("VsFunc error type", _pToken->value);
		spPassInfo->shaderFuncs[enumInt(ShaderType::Vertex)] = _pToken->value;
	}
	else if (_checkTokenValue("PsFunc"))
	{
		_nextToken();
		if (!_checkTokenType(TokenType::Identifier))
			_error("PsFunc error type", _pToken->value);
		spPassInfo->shaderFuncs[enumInt(ShaderType::Pixel)] = _pToken->value;
	}
}

void ShaderParser::_trim_newline()
{
	while (_checkToken(TokenType::Operator, "\n"))
	{
		_nextToken();
	}
}

void ShaderParser::_nextToken()
{
	SGE_ASSERT(_tokenIndex < _pTokens->size());
	_pToken = _pTokens->begin() + _tokenIndex;
	_tokenIndex++;

#if _DEBUG_SHADER_PARSER
	auto typeStr = Lexer::s_printTokenType((*_pTokens)[_tokenIndex - 1].type);
	//SGE_DUMP_VAR(typeStr, (*_pTokens)[_tokenIndex - 1].value);
#endif
}

}