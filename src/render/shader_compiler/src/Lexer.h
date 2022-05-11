#pragma once

namespace sge {

class Lexer : public NonCopyable
{
public:
	enum class TokenType
	{
		None = 0,
		Keyword,
		Identifier,
		Operator,
		Number,
		String,
	};

	struct Token
	{
		Token(TokenType type_, StrView value_)
			:
			type(type_), value(value_)
		{
		}
		TokenType type;
		String value;
	};

public:
	static StringMap<int> s_keywordMap;
	static StringMap<int> s_operatorMap;
	static bool s_isInited;
	static char* s_printTokenType(Lexer::TokenType type_)
	{
		switch (type_)
		{
		case sge::Lexer::TokenType::Keyword:		return "Keyword";
		case sge::Lexer::TokenType::Identifier:		return "Identifier";
		case sge::Lexer::TokenType::Operator:		return "Operator";
		case sge::Lexer::TokenType::Number:			return "Number";
		case sge::Lexer::TokenType::String:			return "String";
		}
		return "Error";
	}

	Lexer();
	
	Vector<Token> parse(StrView filename_);

private:

	void loadFile(StrView filename_);
	void loadMem(Span<const u8> src_);

	void _nextLine();
	void _nextToken();
	void _parseLine();

	void _parseToken();

	bool _try_emplaceToken();

	void _checkShouldEnd();

	void _error(StrView msg_);

	static TokenType _try_getTokenType(StrView token_);

	static bool _isKeyword(StrView token_);
	static bool _isIdentifier(StrView token_);
	static bool _isOperator(StrView token_);
	static bool _isNumber(StrView token_);
	static bool _isString(StrView token_);

public:
	u32 _lineNumber = 0;
	StrView _source;
	StrView _sourceRemain;

	StrView _currentLine;
	StrView _lineRemain;

	StrView _token;

	bool _isStartParsing = false;		// true if detected keyword-> Shader
	bool _isEnded = false;
	u32 _leftCBracketCount = 0;

	MemMapFile _memMap;
private:
	Vector<Token>* _pTokens = nullptr;
};

#if 1
inline void Lexer::_error(StrView msg_) { throw SGE_ERROR("{}: {}", _lineNumber, msg_); }

inline Lexer::TokenType Lexer::_try_getTokenType(StrView token_)
{ 
	using Type = Lexer::TokenType;
	if (token_.size() <= 0)
		return Type::None;

	if (_isOperator(token_))	{ return Type::Operator; }
	if (_isString(token_))		{ 
		return Type::String; }
	if (_isKeyword(token_))		{ return Type::Keyword; }
	if (_isNumber(token_))		{ return Type::Number; }
	if (_isIdentifier(token_))	{ return Type::Identifier; }

	return Type::None;
}

inline bool Lexer::_isKeyword(StrView token_)
{
	String_<20> tmp;
	tmp.append(token_.data(), token_.size());
	auto it = s_keywordMap.find(tmp.c_str());
	return it != s_keywordMap.end();
}
inline bool Lexer::_isIdentifier(StrView token_)	{ return !_isNumber(token_) && !_isKeyword(token_); }
inline bool Lexer::_isOperator(StrView token_)		
{
	String_<20> tmp;
	tmp.append(token_.data(), token_.size());
	auto it = s_operatorMap.find(tmp.c_str());
	return it != s_operatorMap.end();
}
inline bool Lexer::_isNumber(StrView token_)		
{ 
	auto pBeg = token_.begin();
	auto pEnd = token_.end();
	bool isFloat = false;
	bool hasPoint = false;

	for (;pBeg != pEnd && (std::isdigit(*pBeg) || *pBeg == '.' || *pBeg == 'f'); ++pBeg)
	{
	}
	return pBeg == pEnd;
}
inline bool Lexer::_isString(StrView token_)		
{ 
	if (token_.size() < 4)
		return false;
	auto last = token_.size() - 1; 
#if 0
	for (size_t i = 0; i < token_.size(); i++)
	{
		SGE_LOG("{} : {}", i, token_[i]);
	}
#endif // 0
	return token_[0] == '\"' && token_[last] == '\"'; 
}
#endif // 1


}