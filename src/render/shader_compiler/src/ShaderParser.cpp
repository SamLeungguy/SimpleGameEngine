#include "ShaderParser.h"

namespace sge {

void ShaderParser::readFile(ShaderInfo& outInfo_, StrView filename_)
{
	_memMapFile.open(filename_);
	readMem(outInfo_, _memMapFile, filename_);
}

void ShaderParser::readMem(ShaderInfo& outInfo_, ByteSpan data_, StrView filename_)
{
	outInfo_.clear();
	_pOutInfo = &outInfo_;
	reset(data_, filename_);
	skipNewlineTokens();

	if (_token.isIdentifier("Shader")) {
		_readShader();
	}
	else {
		error("missing Shader tag");
	}
}

void ShaderParser::_readShader()
{
	nextToken();
	expectOperator("{");

	for (;;) {
		if (_token.isOperator("}"))		{ nextToken(); break; }
		if (_token.isNewline())			{ nextToken(); continue; }
		if (_token.isIdentifier("Properties"))	{ _readProperties(); continue; }
		if (_token.isIdentifier("Pass"))		{ _readPass(); continue; }
		return errorUnexpectedToken();
	}
}

void ShaderParser::_readProperties()
{
	nextToken();
	expectOperator("{");

	for (;;) {
		skipNewlineTokens();
		if (_token.isOperator("}")) { nextToken(); break; }
		_readProperty();
	}
}

void ShaderParser::_readProperty()
{
	auto& prop = _pOutInfo->props.emplace_back();

	if (_token.isOperator("["))
	{
		nextToken();
		while (!_token.isNone())
		{
			skipNewlineTokens();

			if (_token.isIdentifier("DisplayName"))
			{
				nextToken();
				expectOperator("=");
				readString(prop.displayName);
			}
			if (_token.isOperator("]")) { nextToken(); break; }

			expectOperator(",");
		}
	}

	skipNewlineTokens();

	{
		// prop type
		_readEnum(prop.propType);

		// prop name
		readIdentifier(prop.name);
	}

	// optional defaultValue
	if (_token.isOperator("=")) {
		nextToken();
		while (!_token.isNone()) {
			if (_token.isNewline()) { break; }
			prop.defaultValue += _token.str;
			nextToken();
		}
	}

	if (!_token.isNewline()) {
		errorUnexpectedToken();
	}
	nextToken();
}

void ShaderParser::_readPass()
{
	nextToken();
	auto& o = _pOutInfo->passes.emplace_back();

	if (_token.isString()) {
		readString(o.name);
	}
	expectOperator("{");

	for (;;) {
		if (_token.isOperator("}")) { nextToken(); break; }
		if (_token.isNewline())		{ nextToken(); continue; }

		if (_token.isIdentifier("VsFunc")) { nextToken(); readIdentifier(o.vsFunc); continue; }
		if (_token.isIdentifier("PsFunc")) { nextToken(); readIdentifier(o.psFunc); continue; }

		if (_token.isIdentifier("Cull"))    { nextToken(); readIdentifier(o.cull);    continue; }

		if (_token.isIdentifier("BlendRGB"))  
		{  
			nextToken(); 
			readIdentifier(o.blendRGB.op);    
			readIdentifier(o.blendRGB.src);    
			readIdentifier(o.blendRGB.dst);    
			continue; 
		}
		if (_token.isIdentifier("BlendAlpha"))  
		{  
			nextToken(); 
			readIdentifier(o.blendAlpha.op);    
			readIdentifier(o.blendAlpha.src);    
			readIdentifier(o.blendAlpha.dst);    
			continue; 
		}

		if (_token.isIdentifier("DepthTest"))		{ nextToken(); readIdentifier(o.depthTest);    continue; }
		if (_token.isIdentifier("DepthWrite"))		{ nextToken(); readBool(o.isDepthWrite);    continue; }

		return errorUnexpectedToken();
	}
}

}