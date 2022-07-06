#include "Lexer.h"
#include <sge_core/log/Log.h>
#include <sge_core/file/MemMapFile.h>

namespace sge {

#if 0
#pragma mark TokenType_Impl
#endif // 0
#if 1    // TokenType_Impl

void Lexer::Token::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "({}, {})", type, str);
}

#endif

#if 0
#pragma mark Lexer_Impl
#endif // 0
#if 1    // Lexer_Impl

 void Lexer::reset(ByteSpan source_, StrView filename_)
{
	 reset(StrView_make(source_), filename_);
}

 void Lexer::reset(StrView source_, StrView filename_)
{
	 _filename = filename_;
	 _source = source_;
	 _cur = _source.data();
	 _ch = 0;
	 _col = 0;
	 _line = 1;

	 nextChar();
	 nextToken();
}

 bool Lexer::nextChar()
{
	 _ch = 0;
	 if (!_cur) return false;
	 if (_cur >= _source.end()) return false;

	 _ch = *_cur;
	 _cur++;
	 _col++;

	 if (_ch == '\n')
	 {
		 _line++;
		 _col = 0;
	 }

	return true;
}

 bool Lexer::nextToken()
{
	 if (!_nextToken()) return false;
	 SGE_DUMP_VAR(_token);
	 return true;
}

 void Lexer::trimSpaces()
{
	 for (;;)
	 {
		 if (_ch == ' ' || _ch == '\t' || _ch == '\r')
		 {
			 nextChar();
		 }
		 else
			 break;
	 }
}

 void Lexer::skipNewlineTokens()
{
	 while (_token.isNewline())
	 {
		 _nextToken();
	 }
}

 void Lexer::errorUnexpectedChar()
 {
	 error("Unexpected character [{}]", _ch);
 }

 void Lexer::errorUnexpectedToken()
 {
	 error("Unexpected token [{}]", _token.str);
 }

 void Lexer::expectOperator(StrView sv_)
 {
	 if (_token.isOperator(sv_)) {
		 nextToken();
		 return;
	 }
	 error("expected token {}", sv_);
 }

 void Lexer::readString(String& outStr_)
 {
	 if (!_token.isString()) errorUnexpectedToken();
	 outStr_ = _token.str;
	 nextToken();
 }

 void Lexer::readIdentifier(String& outStr_)
 {
	 if (!_token.isIdentifier()) errorUnexpectedToken();
	 outStr_ = _token.str;
	 nextToken();
 }

 void Lexer::readBool(bool& outBool_)
 {
	 if (!_token.isIdentifier()) errorUnexpectedToken();
	 if     (_token.str.compare("true") == 0)  { outBool_ = true; }
	 else if (_token.str.compare("false") == 0)  { outBool_ = false; }
	 else                      { errorUnexpectedToken(); }

	 nextToken();
 }

 StrView Lexer::getLastFewLines(size_t lineCount_)
 {
	 if (!_cur) return StrView();

	 auto n = lineCount_;

	 auto* start = _source.data();

	 auto* p = _cur;
	 for (; p >= start && n > 0; p--) {
		 if (*p == '\n') {
			 n--;
		 }
	 }
	 p++;

	 return StrView(p, _cur - p);
 }

 StrView Lexer::getRemainSource() const {
	 if (!_cur) return StrView();
	 auto* s = _cur - 1;
	 return StrView(s, _source.end() - s);
 }

 void Lexer::_error(StrView msg_)
 {
	 TempString tmp = msg_;
	 FmtTo(tmp, "\n{}\n", getLastFewLines(3));

	 {
		 TempString lastLine = getLastFewLines(1);
		 size_t i = 0;
		 for (auto& c : lastLine)
		 {
			 if (i >= _col) break;
			 if (c == ' ')	{ tmp += '-';		continue; }
			 if (c == '\n') { tmp += c;			continue; }
			 if (c == '\t') { tmp += "----";	continue; }
			 tmp += '-';
			 ++i;
		 }
		 tmp += "^^^\n";
	 }

	 FmtTo(tmp, "  token={}\n  file={}:{}:{}\n", _filename, _line, _col);
	 throw SGE_ERROR("{}", tmp);
 }

 bool Lexer::_nextToken()
 {
	 _token.setNone();

	 for (;;)
	 {
		 trimSpaces();
		 if (!_ch) return false;

		 if (_ch == '#') {
			 _parseCommentSingleLine();
			 continue;
		 }

		 if (_ch == '_' || isAlpha(_ch))
		 {
			 return _parseIdentifier();
		 }

		 if (isDigit(_ch))	{ return _parseNumber(); }
		 if (_ch == '\"')	{ return _parseString(); }

		 if (_ch == '\n')
		 {
			 _token.type = TokenType::Newline;
			 _token.str += "<newline>";
			 nextChar();
			 return true;
		 }

		 if (_ch == '/')
		 {
			 nextChar();
			 if (_ch == '*')
			 {
				 _parseCommentBlock();
				 continue;
			 }

			 if (_ch == '/')
			 {
				 _parseCommentSingleLine();
				 continue;
			 }

			 _token.type = TokenType::Operator;
			 _token.str = '/';
			 return true;
		 }

		 _token.type = TokenType::Operator;
		 _token.str += _ch;
		 nextChar();
		 return true;
	 }
 }

 bool Lexer::_parseIdentifier()
 {
	 _token.type = TokenType::Identifier;

	 _token.str += _ch;
	 nextChar();

	 while (_ch)
	 {
		 if (_ch == '_' || isAlpha(_ch) || isDigit(_ch))
		 {
			 _token.str += _ch;
			 nextChar();
		 }
		 else
			 break;
	 }

	 return false;
 }

 bool Lexer::_parseNumber()
 {
	 _token.type = TokenType::Number;

	 _token.str += _ch;
	 nextChar();

	 bool hasDot = false;

	 while (_ch)
	 {
		 if (_ch == '.')
		 {
			 if (hasDot)
			 {
				 errorUnexpectedChar();
			 }

			 hasDot = true;
			 _token.str += _ch;
			 nextChar();
		 }
		 else if (isDigit(_ch))
		 {
			 _token.str += _ch;
			 nextChar();
		 }
		 else
			 break;
	 }

	 return true;
 }

 bool Lexer::_parseString()
 {
	 _token.type = TokenType::String;

	 for (;;)
	 {
		 nextChar();

		 if (_ch == '\\')
		 {
			 nextChar();
			 switch (_ch)
			 {
				 case '\\':
				 case '/':
				 case '"':
					 _token.str += _ch;
					 break;
				 case 'b': _token.str += '\b'; break;
				 case 'f': _token.str += '\f'; break;
				 case 'n': _token.str += '\n'; break;
				 case 'r': _token.str += '\r'; break;
				 case 't': _token.str += '\t'; break;
			 default:
				 error("unknown escape character [{}]", _ch);
			 }
		 }
		 else if (_ch == '\"')
		 {
			 nextChar();
			 break;
		 }
		 else
		 {
			 _token.str += _ch;
		 }
	 }

	 return true;
 }

 void Lexer::_parseCommentBlock()
 {
	 nextChar();

	 for (;;)
	 {
		 if (!_ch) return;
		
		 if (_ch == '*')
		 {
			 nextChar();
			 if (_ch == '/') {
				 nextChar();
				 return;
			 }
		 }
		 else
		 {
			 nextChar();
		 }
	 }
 }

 void Lexer::_parseCommentSingleLine()
 {
	 nextChar();
	 for (;;)
	 {
		 if (!_ch) return;
		 if (_ch == '\n') {
			 nextChar();
			 return;
		 }
		 else {
			 nextChar();
		 }
	 }
 }


#endif    // Lexer_Impl

}