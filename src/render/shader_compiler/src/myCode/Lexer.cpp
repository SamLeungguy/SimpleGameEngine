#if 0
#include "Lexer.h"
#include "sge_core/file/MemMapFile.h"

#define _DEBUG_LEXER 0

namespace sge {

StringMap<int> Lexer::s_keywordMap;
StringMap<int> Lexer::s_operatorMap;
bool Lexer::s_isInited = false;

Lexer::Lexer()
{
	if (!s_isInited)
	{
		{
			s_keywordMap.emplace("Shader", 0);
			s_keywordMap.emplace("Properties", 0);
			s_keywordMap.emplace("Premutation", 0);
			s_keywordMap.emplace("Pass", 0);

			s_keywordMap.emplace("Color4f", 0);
			s_keywordMap.emplace("float", 0);
			s_keywordMap.emplace("float2", 0);
			s_keywordMap.emplace("float3", 0);
			s_keywordMap.emplace("float4", 0);
			s_keywordMap.emplace("int", 0);
			s_keywordMap.emplace("bool", 0);
			s_keywordMap.emplace("float4x4", 0);
			s_keywordMap.emplace("float3x3", 0);

			s_keywordMap.emplace("Queue", 0);
			s_keywordMap.emplace("Cull", 0);

			s_keywordMap.emplace("BlendRGB", 0);
			s_keywordMap.emplace("BlendAlpha", 0);
			s_keywordMap.emplace("Add", 0);
			s_keywordMap.emplace("One", 0);
			s_keywordMap.emplace("OneMinusSrcAlpha", 0);

			s_keywordMap.emplace("DepthTest", 0);
			s_keywordMap.emplace("Always", 0);
			s_keywordMap.emplace("DepthWrite", 0);

			s_keywordMap.emplace("VsFunc", 0);
			s_keywordMap.emplace("PsFunc", 0);

			s_keywordMap.emplace("true", 0);
			s_keywordMap.emplace("false", 0);
		}

		{
			s_operatorMap.emplace("+", 0);
			s_operatorMap.emplace("-", 0);
			s_operatorMap.emplace("*", 0);
			s_operatorMap.emplace("/", 0);
			s_operatorMap.emplace("=", 0);
			s_operatorMap.emplace("==", 0);
			s_operatorMap.emplace("+=", 0);
			s_operatorMap.emplace("-=", 0);
			s_operatorMap.emplace("*=", 0);
			s_operatorMap.emplace("/=", 0);
			s_operatorMap.emplace("++", 0);
			s_operatorMap.emplace("--", 0);

			s_operatorMap.emplace("{", 0);
			s_operatorMap.emplace("}", 0);

			s_operatorMap.emplace(",", 0);
			s_operatorMap.emplace("\n", 0);
		}
	}
#if _DEBUG_LEXER
	SGE_DUMP_VAR(Lexer::s_keywordMap.size());
#endif
}

Vector<Lexer::Token> Lexer::parse(StrView filename_)
{
	//Lexer lexer;
	Vector<Token> tokens;
	_pTokens = &tokens;

	loadFile(filename_);

#if _DEBUG_LEXER
	SGE_LOG("==============================================parse end");

	for (size_t i = 0; i < tokens.size(); i++)
	{
		auto typeStr = s_printTokenType(tokens[i].type);
		SGE_DUMP_VAR(typeStr, tokens[i].value);
	}
#endif // _DEBUG_LEXER

	return tokens;
}

void Lexer::loadFile(StrView filename_)
{
	_memMap.openRead(filename_);
	loadMem(_memMap);
}

void Lexer::loadMem(Span<const u8> src_)
{
	_source = StrView(reinterpret_cast<const char*>(src_.data()), src_.size() * sizeof(char));
	_sourceRemain = _source;

	while (_sourceRemain.size() > 0 && (!_isEnded || !_isStartParsing))
	{
		_nextLine();
		if (_currentLine.size() > 0)
		{
			_lineRemain = _currentLine;
			while (_lineRemain.size() > 0)
			{
				_nextToken();
				_parseToken();
			}
		}
	}
}

void Lexer::_nextLine()
{
	auto pair = StringUtil::splitByChar_Inclusive(_sourceRemain, "\n");
	_currentLine = pair.first;
	_sourceRemain = pair.second;
	++_lineNumber;
}

void Lexer::_nextToken()
{
	_lineRemain = StringUtil::trimChar(_lineRemain, " \t\r");
	//auto pair = StringUtil::splitByChar_Inclusive(_lineRemain, " {}\t\r");
	auto pair = StringUtil::splitByChar(_lineRemain, " \t\r");
	_token = StringUtil::trimChar(pair.first, " \t\r");
	_lineRemain = StringUtil::trimChar(pair.second, " \t\r");

}

void Lexer::_parseLine()
{

}

void Lexer::_parseToken()
{
	if (_token.size() <= 0)
		return;

	if (_token.size() >= 2 && _token[0] == '/' && _token[1] == '/')
	{
		_lineRemain = StrView();
		return;
	}

	// shd have own func call
	auto pair = StringUtil::splitByChar_Inclusive(_token, "=,{}\n+-*/");		// check is token with concat able token eg. {}+-*/
	if (pair.first.size() > 0 && pair.first[0] == '/')
	{
		if (pair.second.size() > 0 && pair.second[0] == '/')
		{
			_lineRemain = StrView();
			return;
		}
	}

	_token = pair.first;
	if (!_try_emplaceToken())
		_error("_try_emplaceToken");

	while (pair.second.size() > 0)
	{
		// shd have own func call
		pair = StringUtil::splitByChar_Inclusive(pair.second, "=,{}\n+-*/");		// check is token with concat able token eg. {}+-*/
		if (pair.first.size() > 0 && pair.first[0] == '/')
		{
			if (pair.second.size() > 0 && pair.second[0] == '/')
			{
				_lineRemain = StrView();
				return;
			}
		}

		_token = pair.first;

		if (!_try_emplaceToken())
			_error("_try_emplaceToken");
	}
}

bool Lexer::_try_emplaceToken()
{
	auto type = _try_getTokenType(_token);
	if (type == TokenType::None)
		return false;

	if (type == TokenType::String)
	{
		auto pair = StringUtil::splitByChar(_token, "\"");
		pair = StringUtil::splitByChar(pair.second, "\"");
		_pTokens->emplace_back(type, pair.first);
	}
	else
	{
		//SGE_DUMP_VAR(_token);
		_pTokens->emplace_back(type, _token);
	}


	_checkShouldEnd();
	return true;
}

void Lexer::_checkShouldEnd()
{
	const auto& last = _pTokens->back();
	if (!_isStartParsing)
	{
		_isStartParsing = Math::hashStr(last.value) == Math::hashCStr("Shader");
		if (!_isStartParsing)
			_error("Invalid Start Keyword");
	}
	if (last.type != TokenType::Operator)
		return;
	if (last.value == "{")
		_leftCBracketCount++;
	else if (last.value == "}")
		_leftCBracketCount--;

	//SGE_DUMP_VAR(_LCBracketCount);

	if (_pTokens->size() > 3 && _leftCBracketCount == 0 && _isStartParsing)
	{
		_isEnded = true;
	}
}

}
#endif // 0
