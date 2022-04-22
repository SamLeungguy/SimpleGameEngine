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

enum class PrimitiveType
{
	None = 0,
	Triangle,
	Triangle_Strip,
	Line,
	Line_Strip,
};

enum class VertexSematic 
{
	None = 0,
	Pos,
	Uv0,
	Uv1,
	Uv2,
	Uv3,
	Uv4,
	Uv5,
	Uv6,
	Uv7,
	Color,

	Count,
};

enum class RenderDataType
{
	None = 0,
	Float32,
	Float32x2,
	Float32x3,
	Float32x4,
	Int8,
	Int16,
	Int32,
	Int64,
};

enum class BufferUsage
{
	Static = 0,
	Dynamic,
	Default,
};

namespace Utils
{
inline static uint32_t getRenderDataTypeSize(RenderDataType type_)
{
	switch (type_)
	{
		//case RenderDataType::Int8:			return 4;
		//case RenderDataType::Int16:			return 4 * 2;
		//case RenderDataType::Int32:			return 4 * 3;
		//case RenderDataType::Int64:			return 4 * 4;
		case RenderDataType::Float32:		return 4;
		case RenderDataType::Float32x2:		return 4 * 2;
		case RenderDataType::Float32x3:		return 4 * 3;
		case RenderDataType::Float32x4:		return 4 * 4;
		//case RenderDataType::Mat3:		return 4 * 3 * 3;
		//case RenderDataType::Mat4:		return 4 * 4 * 4;
		//case RenderDataType::Bool:		return 4;
	}
	SGE_ASSERT(0, "Invalid render data type!");
	return 0;
}

inline static uint32_t getRenderDataTypeCount(RenderDataType type_)
{
	switch (type_)
	{
		case RenderDataType::Int8:			return 1;
		case RenderDataType::Int16:			return 2;
		case RenderDataType::Int32:			return 3;
		case RenderDataType::Int64:			return 4;
		case RenderDataType::Float32:		return 1;
		case RenderDataType::Float32x2:		return 2;
		case RenderDataType::Float32x3:		return 3;
		case RenderDataType::Float32x4:		return 4;
		//case RenderDataType::Mat3:			return 3;
		//case RenderDataType::Mat4:			return 4;
		//case RenderDataType::Bool:			return 1;
		default: break;
	}
	SGE_ASSERT(0, "Invalid render data type!");
	return 0;
}

inline static StrView getVertexSematicName(VertexSematic sematic_)
{
	switch (sematic_)
	{
		case VertexSematic::Pos:			return "POSITION";
		case VertexSematic::Uv0:			return "TEXCOORD0";
		case VertexSematic::Uv1:			return "TEXCOORD1;";
		case VertexSematic::Uv2:			return "TEXCOORD2;";
		case VertexSematic::Uv3:			return "TEXCOORD3;";
		case VertexSematic::Uv4:			return "TEXCOORD4;";
		case VertexSematic::Uv5:			return "TEXCOORD5;";
		case VertexSematic::Uv6:			return "TEXCOORD6;";
		case VertexSematic::Uv7:			return "TEXCOORD7;";
		case VertexSematic::Color:			return "COLOR;";
		default: break;
	}
	SGE_ASSERT(0, "Invalid render data type!");
	return 0;
}

}

}