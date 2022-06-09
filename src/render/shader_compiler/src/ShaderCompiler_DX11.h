#pragma once
#include <sge_render.h>
#include <sge_render/backend/dx11/Render_Common_DX11.h>

namespace sge {

class ShaderCompiler_DX11 : public NonCopyable
{
	using Util = DX11Util;
public:
	void compile(StrView outFilename_, ShaderStage shaderStage_, StrView srcFilename_, StrView entryFunc_);

private:

};

}