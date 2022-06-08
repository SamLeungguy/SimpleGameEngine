#if 0
#pragma once

#include "ShaderInfo.h"

namespace sge {

struct RenderShader_CreateDesc {
	StrView filename;
	SPtr<ShaderParsedInfo> spShaderParsedInfo;
	int passIndex = 0;
};

class RenderShader : public Object
{
public:
	using CreateDesc = RenderShader_CreateDesc;

	RenderShader(CreateDesc& desc_);
	virtual ~RenderShader() = default;

	Vector<SPtr<ShaderInfo>>& getShaderInfoSPtrs();
	const Vector<SPtr<ShaderInfo>>& getShaderInfoSPtrs() const;

protected:
	String _filename;
	Vector<SPtr<ShaderInfo>> _shaderInfoSPtrs;		// vs, ps, / gs/ ts / cs
};

inline Vector<SPtr<ShaderInfo>>& RenderShader::getShaderInfoSPtrs() { return _shaderInfoSPtrs; }
inline const Vector<SPtr<ShaderInfo>>& RenderShader::getShaderInfoSPtrs() const { return _shaderInfoSPtrs; }

}
#endif // 0
