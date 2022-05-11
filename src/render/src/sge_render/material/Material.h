#pragma once

#include "RenderShader.h"
#include "../buffer/RenderGpuBuffer.h"
#include "ShaderInfo.h"

namespace sge {

struct ShaderPass
{
	Vector<ShaderInfo> _shaderInfos;
	SPtr<RenderShader> _spShader;
};

class Material : public Object
{
public:
	Material();
	virtual ~Material() = default;

private:
	Vector<ShaderPass>		_shaderPasses;
	SPtr<ShaderParsedInfo>	_spShaderParsedInfo;
};

}