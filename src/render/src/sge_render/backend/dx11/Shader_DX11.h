#pragma once

#if SGE_RENDER_HAS_DX11

#include "sge_render/shader/Shader.h"

namespace sge {

class Shader_DX11 : public Shader
{
	using Base = Shader;
	using CreateDesc = Shader_CreateDesc;
public:
	Shader_DX11(CreateDesc& desc_);

private:
};

}

#endif