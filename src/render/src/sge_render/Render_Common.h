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

enum class BufferUsage
{
	Static = 0,
	Dynamic,
	Default,
};


enum class RenderShaderType
{
	Vertex = 0,
	Pixel,
	Gemotry,
	Tessellation,
	Compute,

	Count
};

namespace Utils
{

//inline static StrView getVertexSematicName(VertexSematic sematic_)
//{
//	switch (sematic_)
//	{
//		case VertexSematic::Pos:			return "POSITION";
//		case VertexSematic::Uv0:			return "TEXCOORD0";
//		case VertexSematic::Uv1:			return "TEXCOORD1;";
//		case VertexSematic::Uv2:			return "TEXCOORD2;";
//		case VertexSematic::Uv3:			return "TEXCOORD3;";
//		case VertexSematic::Uv4:			return "TEXCOORD4;";
//		case VertexSematic::Uv5:			return "TEXCOORD5;";
//		case VertexSematic::Uv6:			return "TEXCOORD6;";
//		case VertexSematic::Uv7:			return "TEXCOORD7;";
//		case VertexSematic::Color:			return "COLOR;";
//		default: break;
//	}
//	SGE_ASSERT(0, "Invalid render data type!");
//	return 0;
//}

}

}