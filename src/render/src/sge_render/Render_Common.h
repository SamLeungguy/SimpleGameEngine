#pragma once

#include <sge_core/base/sge_macro.h>

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

#if 0
#pragma mark -----------------------Shader Pass Enum
#endif // 0
enum class CullMode
{
	None = 0,
	Off,
	Front,
	Back,
};
#define CullMode_ENUM_LIST(E) \
  E(None)\
  E(Off)\
  E(Front)\
  E(Back)\
//----

SGE_ENUM_ALL_OPERATOR(CullMode);
SGE_ENUM_STR_UTIL(CullMode);

enum class FillMode
{
	None = 0,
	Wire,
	Solid,
};
#define FillMode_ENUM_LIST(E) \
  E(None)\
  E(Wire)\
  E(Solid)\
//----

SGE_ENUM_ALL_OPERATOR(FillMode);
SGE_ENUM_STR_UTIL(FillMode);

enum class DepthComp
{
	None = 0,
	Never,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always,
};
#define DepthComp_ENUM_LIST(E) \
	E(None)\
	E(Never)\
	E(Less)\
	E(Equal)\
	E(LessEqual)\
	E(Greater)\
	E(NotEqual)\
	E(GreaterEqual)\
	E(Always)\
//----

SGE_ENUM_ALL_OPERATOR(DepthComp);
SGE_ENUM_STR_UTIL(DepthComp);

enum class BlendOp
{
	None,
	Add        , // Add (s + d).
	Subtract    , // Subtract.
	ReverseSubtract  , // Reverse subtract.
	Min        , // Min.
	Max        , // Max.
	LogicalClear  , // Logical Clear (0).
};
#define BlendOp_ENUM_LIST(E) \
	E(None)          \
	E(Add)          \
	E(Subtract)        \
	E(ReverseSubtract)    \
	E(Min)          \
	E(Max)          \
	E(LogicalClear)      \
//----

SGE_ENUM_ALL_OPERATOR(BlendOp);
SGE_ENUM_STR_UTIL(BlendOp);

enum class BlendMode
{
	None = 0,
	Zero,				//Blend factor is (0, 0, 0, 0).
	One,				//Blend factor is (1, 1, 1, 1).
	SrcColor,			//Blend factor is (Rs, Gs, Bs, As).
	OneMinusSrcColor,	//Blend factor is (1 - Rs, 1 - Gs, 1 - Bs, 1 - As).
	SrcAlpha,			//Blend factor is (As, As, As, As).
	OneMinusSrcAlpha,	//Blend factor is (1 - As, 1 - As, 1 - As, 1 - As).
	DstAlpha,			//Blend factor is (Ad, Ad, Ad, Ad).
	OneMinusDstAlpha,	//Blend factor is (1 - Ad, 1 - Ad, 1 - Ad, 1 - Ad).
	DstColor,			//Blend factor is (Rd, Gd, Bd, Ad).
	OneMinusDstColor,	//Blend factor is (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad).
	SrcAlphaSaturate,	//Blend factor is (f, f, f, 1); where f = min(As, 1 - Ad).
};
#define BlendMode_ENUM_LIST(E) E(None)\
	E(Zero)\
	E(One)\
	E(SrcColor)\
	E(OneMinusSrcColor)\
	E(SrcAlpha)\
	E(OneMinusSrcAlpha)\
	E(DstAlpha)\
	E(OneMinusDstAlpha)\
	E(DstColor)\
	E(OneMinusDstColor)\
	E(SrcAlphaSaturate)\
//----
SGE_ENUM_ALL_OPERATOR(BlendMode)
SGE_ENUM_STR_UTIL(BlendMode)

#if 0
#pragma mark ------------------ RenderState
#endif // 0

#if 1    // RenderState

struct RasterizerState
{
	CullMode cullMode = CullMode::Back;
	FillMode fillMode = FillMode::Solid;

	EA_CPP14_CONSTEXPR bool operator<(const RasterizerState& b) const
	{ return cullMode < b.cullMode && fillMode < b.fillMode; }

	EA_CPP14_CONSTEXPR bool operator==(const RasterizerState& b) const
	{ return cullMode == b.cullMode && fillMode == b.fillMode; }
};

struct BlendState
{
	struct BlendOption
	{
		BlendOp op		= BlendOp::None;
		BlendMode src	= BlendMode::None;
		BlendMode dst	= BlendMode::None;
	};
	BlendOption blendRGB	= {BlendOp::None, BlendMode::One, BlendMode::OneMinusSrcColor};
	BlendOption blendAlpha	= {BlendOp::None, BlendMode::One, BlendMode::OneMinusDstAlpha};
};

struct DepthStencilState
{
	DepthComp depthTest = DepthComp::Always;
	bool isDepthWrite = false;

	EA_CPP14_CONSTEXPR bool operator<(const DepthStencilState& b) const
	{ return depthTest < b.depthTest && isDepthWrite < b.isDepthWrite; }

	EA_CPP14_CONSTEXPR bool operator==(const DepthStencilState& b) const
	{ return depthTest == b.depthTest && isDepthWrite == b.isDepthWrite; }
};

struct RenderState
{
	RasterizerState rasterizerState;
	BlendState blendState;
	DepthStencilState depthStencilState;
};

//enum class RasterizerType : u32 { None = 0 };

// 4 bits for BlendOp_alpha
// 6 bits for BlendMode_src_alpha
// 6 bits for BlendMode_dst_alpha
// 4 bits for BlendOp_rgb
// 6 bits for BlendMode_src_rgb
// 6 bits for BlendMode_dst_rgb
enum class BlendType : u32 { None = 0 };

struct RenderStateUtil
{
	RenderStateUtil() = delete;
#if 0
#pragma --------------- RasterizerType
#endif // 0

#if 0
#pragma --------------- BlendType
#endif // 0

#if 1 // BlendType
	static constexpr BlendType _add_BlendOp(BlendType type_, BlendOp op_, size_t bit_)
	{
		return static_cast<BlendType>(static_cast<u32>(type_)
			| (static_cast<u32>(op_) << bit_));
	}

	static constexpr BlendType _add_BlendMode(BlendType type_, BlendMode op_, size_t bit_)
	{
		return static_cast<BlendType>(static_cast<u32>(type_)
			| (static_cast<u32>(op_) << bit_));
	}

	static constexpr BlendType _make_Blend(BlendOp op_a_, BlendMode src_a_, BlendMode dst_a_,
		BlendOp op_r_, BlendMode src_r_, BlendMode dst_r_)
	{
		BlendType v = _add_BlendOp(BlendType::None, op_a_, 0);
		v = _add_BlendMode(v, src_a_,  4);
		v = _add_BlendMode(v, dst_a_, 10);

		v =	  _add_BlendOp(v, op_r_,  16);
		v = _add_BlendMode(v, dst_a_, 20);
		v = _add_BlendMode(v, dst_a_, 26);
		return v;
	}

	static constexpr BlendType make_Blend(BlendState s_)
	{
		return _make_Blend(s_.blendAlpha.op, s_.blendAlpha.src, s_.blendAlpha.dst,
			s_.blendRGB.op, s_.blendRGB.src, s_.blendRGB.dst);
	}
#endif // 1

};

#endif // 1    // RenderState


}