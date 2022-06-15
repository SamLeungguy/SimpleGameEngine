#pragma once

namespace sge {

#define SGE_RENDER_CMD_BIND_FN(fn_) [this](auto&&... args) -> decltype(auto) { return this->fn_(std::forward<decltype(args)>(args)...); }

struct RenderAdapterInfo
{
	String	adapterName;
	i64 memorySize = 0;

	bool isMultithread = false;
	bool hasComputeShader = false;
	bool isShaderHasFloat64 = false;
	int minThreadGroupSize = 1;
};

enum class RenderPrimitiveType
{
	None = 0,
	Points,
	Lines,
	Triangles,
};

}