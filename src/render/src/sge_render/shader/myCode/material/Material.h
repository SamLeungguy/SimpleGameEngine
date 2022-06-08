#if 0
#pragma once

#include "RenderShader.h"
#include "../buffer/RenderGpuBuffer.h"
#include "ShaderInfo.h"

namespace sge {

struct ShaderPass
{
	//Vector<SPtr<ShaderInfo>> _shaderInfoSPtrs;		// vs, ps, / gs/ ts / cs
	SPtr<RenderShader> _spRenderShader;
};

struct Material_CreateDesc {
	StrView filename;
};

class Material : public Object
{
public:
	using CreateDesc = Material_CreateDesc;

	Material(CreateDesc& desc_);
	virtual ~Material() = default;

	void setFloat(StrView name_, float value);

	const Vector<ShaderPass>& getShaderPasses() const;
	Vector<ShaderPass>& getShaderPasses();

protected:
	String _filename;
	Vector<ShaderPass>		_shaderPasses;
	SPtr<ShaderParsedInfo>	_spShaderParsedInfo;
};

inline const Vector<ShaderPass>& Material::getShaderPasses() const { return _shaderPasses; }
inline Vector<ShaderPass>& Material::getShaderPasses() { return _shaderPasses; }

}
#endif // 0
