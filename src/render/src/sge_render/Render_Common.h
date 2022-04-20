#pragma once

namespace sge {

struct RenderAdapterInfo
{
	String	adapterName;
	i64 memorySize = 0;

	bool isMultithread = false;
	bool hasComputeShader = false;
	bool isShaderHasFloat64 = false;
	int minThreadGroupSize = 1;
};

}