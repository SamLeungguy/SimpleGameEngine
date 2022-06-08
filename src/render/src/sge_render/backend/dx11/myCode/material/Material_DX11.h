#if 0
#pragma once

#if SGE_RENDER_HAS_DX11

#include "Render_Common_DX11.h"
#include <sge_render/shader/Material.h>

namespace sge {

class Material_DX11 : public Material
{
	using Base = Material;
	using CreateDesc = Material_CreateDesc;
public:
	Material_DX11(CreateDesc& desc_);
	virtual ~Material_DX11() = default;

protected:

private:

};

}

#endif
#endif // 0
