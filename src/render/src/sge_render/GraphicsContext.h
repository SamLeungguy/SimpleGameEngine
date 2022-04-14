#pragma once

#include "sge_render-base.h"

#if SGE_OS_WINDOWS

#include "d3d11/GraphicsContext_D3d11.h"

namespace sge {
using GraphicsContext_Impl = GraphicsContext_D3d11;
}

#else
	#error "unsupported platform graphics api"

#endif

namespace sge {

class GraphicsContext : public GraphicsContext_Impl
{

public:


private:

};

}