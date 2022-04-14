#pragma once

#include <sge_core/base/sge_base.h>

#if SGE_OS_WINDOWS
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3DCompiler.lib")

#else

#endif