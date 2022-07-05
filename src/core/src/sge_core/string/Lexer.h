#pragma once

#include <sge_core/base/Error.h>
#include <sge_core/string/StringUtil.h>

namespace sge {

class Lexer : public NonCopyable
{
public:
	enum class TokenType
	{
		None,
		Identifier,
		Number,
		String,
		Operator,
		Newline,
	};

	struct Token
	{
		TokenType type = TokenType::None;
		String str;

		bool isNone() const;

		bool isIdentifier() const;
		bool isIdentifier(StrView sv_) const;

		bool isNumber() const;
		bool isNumber(StrView sv_) const;

		bool isString() const;
		bool isString(StrView sv_) const;

		bool isOperator() const;
		bool isOperator(StrView sv_) const;

		bool isNewline() const;
		bool isNewline(StrView sv_) const;

		void setNone(); //  { type = TokenType::None; str.clear(); }

		void onFormat(fmt::format_context& ctx_) const;
	};

	void reset(ByteSpan source_, StrView filename_);
	void reset(StrView source_, StrView filename_);

	bool nextChar();
	bool nextToken();

	void trimSpaces();

	void skipNewlineTokens();

	bool isAlpha	(char c_);
	bool isLowerCase(char c_);
	bool isUpperCase(char c_);
	bool isDigit	(char c_);

	const Token& getToken() const;

	template<class... Args> void error(const Args &... args_);

	void errorUnexpectedChar();
	void errorUnexpectedToken();

	void expectOperator(StrView sv_);

	void readString(String& outStr_);
	void readIdentifier(String& outStr_);

	StrView getLastFewLines(size_t lineCount_);
	StrView getRemainSource() const;

	const char* getCur()	const;
	StrView		getSource()	const;
	size_t		getLine()	const;
	size_t		getCol()	const;

protected:
	void _error(StrView msg_);

	bool _nextToken();

	bool _parseIdentifier();
	bool _parseNumber();
	bool _parseString();
	void _parseCommentBlock();
	void _parseCommentSingleLine();

	Token		_token;
	String		_filename;
	StrView		_source;
	const char* _cur = nullptr;
	char		_ch = 0;
	size_t		_col = 0;
	size_t		_line = 0;
private:

};

#if 0
#pragma mark TokenType_Impl
#endif // 0
#if 1    // TokenType_Impl

inline bool Lexer::Token::isNone() const					{ return type == TokenType::None; }

inline bool Lexer::Token::isIdentifier() const				{ return type == TokenType::Identifier; }
inline bool Lexer::Token::isIdentifier(StrView sv_) const	{ return type == TokenType::Identifier && sv_.compare(str) == 0; }

inline bool Lexer::Token::isNumber() const					{ return type == TokenType::Number; }
inline bool Lexer::Token::isNumber(StrView sv_) const		{ return type == TokenType::Number && sv_.compare(str) == 0; }

inline bool Lexer::Token::isString() const					{ return type == TokenType::String; }
inline bool Lexer::Token::isString(StrView sv_) const		{ return type == TokenType::String && sv_.compare(str) == 0; }

inline bool Lexer::Token::isOperator() const				{ return type == TokenType::Operator; }
inline bool Lexer::Token::isOperator(StrView sv_) const		{ return type == TokenType::Operator && sv_.compare(str) == 0; }

inline bool Lexer::Token::isNewline() const					{ return type == TokenType::Newline; }
inline bool Lexer::Token::isNewline(StrView sv_) const		{ return type == TokenType::Newline && sv_.compare(str) == 0; }

inline void Lexer::Token::setNone()							{ type = TokenType::None; str.clear(); }

#endif // 1    // TokenType_Impl

#if 0
#pragma mark Lexer_Impl
#endif // 0
#if 1    // Lexer_Impl

inline bool Lexer::isAlpha		(char c_) { return isLowerCase(c_) || isUpperCase(c_); }
inline bool Lexer::isLowerCase	(char c_) { return c_ >= 'a' && c_ <= 'z'; }
inline bool Lexer::isUpperCase	(char c_) { return c_ >= 'A' && c_ <= 'Z'; }
inline bool Lexer::isDigit		(char c_) { return c_ >= '0' && c_ <= '9'; }

inline const Lexer::Token& Lexer::getToken() const { return _token; }

inline const char*	Lexer::getCur()		const { return _cur; }
inline StrView		Lexer::getSource()	const { return _source; }
inline size_t		Lexer::getLine()	const { return _line; }
inline size_t		Lexer::getCol()		const { return _col; }

template<class... Args> inline 
void Lexer::error(const Args &... args_)
{
	auto msg = Fmt(args_...);
	_error(msg);
}

#endif

inline
const char* enumStr(Lexer::TokenType v) {
	using E = Lexer::TokenType;
	switch (v) {
	case E::None:			return "None";
	case E::Identifier:		return "Identifier";
	case E::Number:			return "Number";
	case E::String:			return "String";
	case E::Operator:		return "Operator";
	case E::Newline:		return "Newline";
	default: {
		SGE_ASSERT(false);
		return "";
	}
	}
}

SGE_FORMATTER_ENUM(Lexer::TokenType)
SGE_FORMATTER(Lexer::Token);

}

