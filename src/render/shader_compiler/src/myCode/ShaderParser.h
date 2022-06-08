#if 0
#pragma once
#include <sge_render/material/ShaderInfo.h>
#include "Lexer.h"

namespace sge {

class ShaderParser : public NonCopyable // Lexer
{
	using Token = Lexer::Token;
	using TokenType = Lexer::TokenType;

public:
	bool parse(ShaderParsedInfo& outInfo_, StrView filepath_);

public:
	bool _isSuccess = false;

protected:
	void writeToJsonFile(StrView outputPath_);

private:
	void _parse();

	void _parse_next_keyword();

	void _parse_shader_name();
	void _parse_properties();
	void _parse_permutation();
	void _parse_pass();

	void _parse_properties_value();

	void _parse_each_pass();

	void _check_start_parsing_keyword();
	void _check_end_parsing_keyword();

	void _trim_newline();

	bool _checkToken(TokenType type_, const char* value_);
	bool _checkTokenType(TokenType type_);
	bool _checkTokenValue(const char* value_);

	template<typename T>
	bool _try_parseValue(u8* pSrc_, u16 count_);

	void _nextToken();

	bool _checkEndOfParse();

	void _error(StrView msg_, StrView value_ = {});

protected:
	Lexer _lexer;

private:
	const Token* _pToken = nullptr;
	const Vector<Token>* _pTokens = nullptr;

	bool _isParsedShaderKeyword = false, _isParsedProperties = false, _isParsedPremutation = false;

	ShaderParsedInfo* _pInfo = nullptr;

	size_t _tokenIndex = 0;
};

inline void ShaderParser::_error(StrView msg_, StrView value_) { throw SGE_ERROR("{} {}", msg_, value_); }
inline bool ShaderParser::_checkEndOfParse() { return _tokenIndex >= _pTokens->size(); }

inline bool ShaderParser::_checkTokenType(TokenType type_) { return _pToken->type == type_; }
inline bool ShaderParser::_checkTokenValue(const char* value_) { return Math::hashStr(_pToken->value) == Math::hashStr(value_); }

inline bool ShaderParser::_checkToken(TokenType type_, const char* value_) { return _checkTokenType(type_) && _checkTokenValue(value_); }

template<typename T>
inline bool ShaderParser::_try_parseValue(u8* pSrc_, u16 count_)
{
	SGE_ASSERT(count_ > 0, "");
	if (count_ < 2)
		return StringUtil::tryParse(_pToken->value, *reinterpret_cast<T*>(pSrc_));

	bool ret = true;
	for (size_t i = 0; i < count_; i++)
	{
		if (i == 0)
		{
			if (!_checkToken(TokenType::Operator, "{"))
			{
				_error("invalid syntax, no { is found");
				return false;
			}
		}
		else
		{
			if (!_checkToken(TokenType::Operator, ","))
			{
				_error("invalid syntax, no , is found");
				return false;
			}
		}

		_nextToken();
		if (!_checkTokenType(TokenType::Number))
		{
			_error("no value is found");
			return false;
		}

		ret &= StringUtil::tryParse(_pToken->value, *reinterpret_cast<T*>(pSrc_));
		pSrc_ += sizeof(T) * 8;

		_nextToken();
	}
	if (!_checkToken(TokenType::Operator, "}"))
		_error("invalid syntax, no } is found");

	return ret;
}


}
#endif // 0
