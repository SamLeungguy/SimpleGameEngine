#pragma once

#include "RenderShader.h"
#include "../buffer/RenderGpuBuffer.h"
#include "ShaderInfo.h"

namespace sge {

struct ShaderPass
{
	Vector<SPtr<ShaderInfo>> _shaderInfoSPtrs;		// vs, ps, / gs/ ts / cs
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