#pragma once

namespace sge {

class ShaderCompiler_DX11 : public NonCopyable
{
public:
	void compile(RenderShaderType shaderType_, StrView filename_);

private:

};

}