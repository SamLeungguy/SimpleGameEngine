#pragma once

#if SGE_RENDER_HAS_DX11

#include "sge_render/shader/Material.h"

namespace sge {

class Material_DX11 : public Material
{
	using Base = Material;
	using CreateDesc = Material_CreateDesc;
public:
	Material_DX11(CreateDesc desc_);

private:
};

}

#endif