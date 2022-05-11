#pragma once

#include "../Render_Common.h"
#include "../RenderDataType.h"

namespace sge {

// posType			: 8 bit
// colorType		: 8 bit
// colorCount		: 2 bit
// uvType			: 8 bit
// uvCount			: 8 bit
// normalType		: 8 bit
// normalCount		: 2 bit
// tangentCount		: 2 bit
// binormalCount	: 2 bit
enum class VertexType : u64 { None = 0 };

struct VertexTypeUtil
{
	[[nodiscard]]static constexpr VertexType addPos(VertexType vertexType_, RenderDataType dataType_)
	{
		return static_cast<VertexType>(static_cast<u64>(vertexType_) 
										| static_cast<u64>(dataType_));
	}

	[[nodiscard]] static constexpr VertexType addColor(VertexType vertexType_, RenderDataType dataType_, u8 count_)
	{
		return static_cast<VertexType>(static_cast<u64>(vertexType_)
			| (static_cast<u64>(dataType_)	<< 8)
			| (static_cast<u64>(count_)		<< 16));
	}

	[[nodiscard]] static constexpr VertexType addUv(VertexType vertexType_, RenderDataType dataType_, u8 count_)
	{
		return static_cast<VertexType>(static_cast<u64>(vertexType_)
			| (static_cast<u64>(dataType_)	<< 18)
			| (static_cast<u64>(count_)		<< 26));
	}

	[[nodiscard]] static constexpr VertexType addNormal(VertexType vertexType_, RenderDataType dataType_, u8 count_)
	{
		return static_cast<VertexType>(static_cast<u64>(vertexType_)
			| (static_cast<u64>(dataType_)	<< 34)
			| (static_cast<u64>(count_)		<< 42));
	}

	[[nodiscard]] static constexpr VertexType addTangent(VertexType vertexType_, u8 count_)
	{
		return static_cast<VertexType>(static_cast<u64>(vertexType_)
			| (static_cast<u64>(count_) << 44));
	}

	[[nodiscard]] static constexpr VertexType addBinormal(VertexType vertexType_, u8 count_)
	{
		return static_cast<VertexType>(static_cast<u64>(vertexType_)
			| (static_cast<u64>(count_) << 46));
	}

	static VertexType make(RenderDataType pos_dataType_,
		RenderDataType color_dataType_, u8 color_count_,
		RenderDataType uv_dataType_, u8 uv_count_,
		RenderDataType normal_dataType_, u8 normal_count_, u8 tangent_count_, u8 binormal_count_)
	{
		VertexType vertexType = addPos(VertexType::None, pos_dataType_);
		if (color_count_)
			vertexType = addColor(vertexType, color_dataType_, color_count_);
		if (uv_count_)
			vertexType = addUv(vertexType, uv_dataType_, uv_count_);
		if (normal_count_)
		{
			vertexType = addNormal(vertexType, normal_dataType_, normal_count_);
			vertexType = addTangent(vertexType, tangent_count_);
			vertexType = addBinormal(vertexType, binormal_count_);
		}
		return vertexType;
	}
};

enum class Vertex_Semantic : u16;

enum class Vertex_SemanticType : u8
{
	None,
	Pos,
	Color,
	TexCoord,
	Normal,
	Tangent,
	Binormal,
};

SGE_ENUM_ALL_OPERATOR(Vertex_Semantic);

struct Vertex_SemanticUtil
{
	using Semantic = Vertex_Semantic;
	using Type = Vertex_SemanticType;
	using Index = u8;

	static constexpr Semantic make(Type type_, Index index_)
	{
		return static_cast<Semantic>((enumInt(type_) << 8) | index_);
	}
	static constexpr u16 _make(Type type, Index index) {
		return static_cast<u16>(make(type, index));
	};
	static constexpr Type getType(Semantic semantic_) { return static_cast<Type>(enumInt(semantic_) >> 8); }
	static constexpr Index getIndex(Semantic semantic_) { return static_cast<Index>(enumInt(semantic_)); }
};

enum class Vertex_Semantic : u16
{
	None = 0,
	Pos			= Vertex_SemanticUtil::_make(Vertex_SemanticType::Pos, 0),

	Color0		= Vertex_SemanticUtil::_make(Vertex_SemanticType::Color, 0),
	Color1		= Vertex_SemanticUtil::_make(Vertex_SemanticType::Color, 1),
	Color2		= Vertex_SemanticUtil::_make(Vertex_SemanticType::Color, 2),
	Color3		= Vertex_SemanticUtil::_make(Vertex_SemanticType::Color, 3),

	TexCoord0	= Vertex_SemanticUtil::_make(Vertex_SemanticType::TexCoord, 0),
	TexCoord1	= Vertex_SemanticUtil::_make(Vertex_SemanticType::TexCoord, 1),
	TexCoord2	= Vertex_SemanticUtil::_make(Vertex_SemanticType::TexCoord, 2),
	TexCoord3	= Vertex_SemanticUtil::_make(Vertex_SemanticType::TexCoord, 3),
	TexCoord4	= Vertex_SemanticUtil::_make(Vertex_SemanticType::TexCoord, 4),
	TexCoord5	= Vertex_SemanticUtil::_make(Vertex_SemanticType::TexCoord, 5),
	TexCoord6	= Vertex_SemanticUtil::_make(Vertex_SemanticType::TexCoord, 6),
	TexCoord7	= Vertex_SemanticUtil::_make(Vertex_SemanticType::TexCoord, 7),

	Normal		= Vertex_SemanticUtil::_make(Vertex_SemanticType::Normal, 0),
	Tangent		= Vertex_SemanticUtil::_make(Vertex_SemanticType::Tangent, 0),
	Binormal	= Vertex_SemanticUtil::_make(Vertex_SemanticType::Binormal, 0),
};

struct VertexLayout
{
	using Semantic = Vertex_Semantic;
	using DataType = RenderDataType;

	struct Element
	{
		Semantic	semantic	= Semantic::None;
		DataType	dataType	= DataType::None;
		u16			offset		= 0;
	};

	VertexType type = VertexType::None;
	size_t stride = 0;
	Vector_<Element, 16> elements;

	template<class Vertex, class Attr>
	void addElement(Semantic semantic_, Attr Vertex::*attr_, size_t index_ = 0)
	{
		if (std::is_array<Attr>())
		{
			size_t n = std::extent<Attr>();
			for (size_t i = 0; i < n; i++)
			{
				_addElement(semantic_ + static_cast<int>(i), attr_, i);
			}
		}
		else
		{
			_addElement(semantic_, attr_, 0);
		}
	}

private:
	template<class Vertex, class Attr>
	void _addElement(Semantic semantic_, Attr Vertex::* attr_, size_t index_)
	{
		auto& o = elements.push_back();
		o.semantic = semantic_;
		using A = std::remove_extent<Attr>::type;
		o.dataType = RenderDataTypeUtil::get<A>();
		o.offset = static_cast<u16>(memberOffset(attr_) + sizeof(A) * index_);
	}
};

struct VertexBase 
{
	using Semantic = Vertex_Semantic;

	using PosType		= void;
	using ColorType		= void;
	using UvType		= void;
	using NormalType	= void;

	static const RenderDataType s_posType		= RenderDataType::None;
	static const RenderDataType s_colorType		= RenderDataType::None;
	static const RenderDataType s_uvType		= RenderDataType::None;
	static const RenderDataType s_normalType	= RenderDataType::None;

	static const u8 s_colorCount	= 0;
	static const u8 s_uvCount		= 0;
	static const u8 s_normalCount	= 0;
	static const u8 s_tangentCount	= 0;
	static const u8 s_binormalCount = 0;

	static const VertexType s_type = VertexType::None;
};

template<class Pos_Type>
struct VertexT_Pos : public VertexBase
{
	using PosType = Pos_Type;
	PosType pos;

	static const RenderDataType s_posType = RenderDataTypeUtil::get<Pos_Type>();
	static const VertexType s_type = VertexTypeUtil::addPos(VertexType::None, s_posType);

	static void onRegister(VertexLayout* pLayout_)
	{
		pLayout_->addElement(Semantic::Pos, &VertexT_Pos::pos);
	}

	static const VertexLayout* s_getLayout() {
		static const VertexLayout* s = VertexLayoutManager::current()->getLayout(s_type);
		return s;
	}
};

template<class Color_Type, u8 Color_Count, class Base>
struct VertexT_Color : public Base
{
	using ColorType = Color_Type;
	ColorType colors[Color_Count];

	static const RenderDataType s_colorType = RenderDataTypeUtil::get<ColorType>();
	static const u8 s_colorCount = Color_Count;
	static const VertexType s_type = VertexTypeUtil::addColor(Base::s_type, s_colorType, s_colorCount);

	static void onRegister(VertexLayout* pLayout_)
	{
		Base::onRegister(pLayout_);
		pLayout_->addElement(Semantic::Color0, &VertexT_Color::colors);
	}

	static const VertexLayout* s_getLayout() {
		static const VertexLayout* s = VertexLayoutManager::current()->getLayout(s_type);
		return s;
	}
};

template<class Uv_Type, u8 Uv_Count, class Base>
struct VertexT_Uv : public Base
{
	using UvType = Uv_Type;
	UvType uvs[Uv_Count];

	static const RenderDataType s_uvType = RenderDataTypeUtil::get<UvType>();
	static const u8 s_uvCount = Uv_Count;
	static const VertexType s_type = VertexTypeUtil::addUv(Base::s_type, s_uvType, s_uvCount);

	static void onRegister(VertexLayout* pLayout_)
	{
		Base::onRegister(pLayout_);
		pLayout_->addElement(Semantic::TexCoord0, &VertexT_Uv::uvs);
	}

	static const VertexLayout* s_getLayout() {
		static const VertexLayout* s = VertexLayoutManager::current()->getLayout(s_type);
		return s;
	}
};

template<class Normal_Type, u8 Normal_Count, class Base>
struct VertexT_Normal : public Base
{
	using NormalType = Normal_Type;
	NormalType normals[Normal_Count];

	static const RenderDataType s_normalType = RenderDataTypeUtil::get<NormalType>();
	static const u8 s_normalCount = Normal_Count;
	static const VertexType s_type = VertexTypeUtil::addNormal(Base::s_type, s_normalType, s_normalCount);

	static void onRegister(VertexLayout* pLayout_)
	{
		Base::onRegister(pLayout_);
		pLayout_->addElement(Semantic::Normal, &VertexT_Normal::normals);
	}

	static const VertexLayout* s_getLayout() {
		static const VertexLayout* s = VertexLayoutManager::current()->getLayout(s_type);
		return s;
	}
};

template<class Tangent_Type, u8 Tangent_Count, class Base>
struct VertexT_Tangent : public Base
{
	using TangentType = Tangent_Type;
	TangentType tangents[Tangent_Count];

	static const RenderDataType s_tangentType = RenderDataTypeUtil::get<TangentType>();
	static const u8 s_tangentCount = Tangent_Count;
	static const VertexType s_type = VertexTypeUtil::addTangent(Base::s_type, s_tangentCount);

	static void onRegister(VertexLayout* pLayout_)
	{
		static_assert(std::is_same<TangentType, NormalType>::value, "");

		Base::onRegister(pLayout_);
		pLayout_->addElement(Semantic::Tangent, &VertexT_Tangent::tangents);
	}

	static const VertexLayout* s_getLayout() {
		static const VertexLayout* s = VertexLayoutManager::current()->getLayout(s_type);
		return s;
	}
};

template<class Binormal_Type, u8 Binormal_Count, class Base>
struct VertexT_Binormal : public Base
{
	using BinormalType = Binormal_Type;
	BinormalType binormals[Binormal_Count];

	static const RenderDataType s_binormalType = RenderDataTypeUtil::get<BinormalType>();
	static const u8 s_binormalCount = Binormal_Count;
	static const VertexType s_type = VertexTypeUtil::addBinormal(Base::s_type, s_binormalCount);

	static void onRegister(VertexLayout* pLayout_)
	{
		static_assert(std::is_same<BinormalType, NormalType>::value, "");

		Base::onRegister(pLayout_);
		pLayout_->addElement(Semantic::Binormal, &VertexT_Binormal::binormals);
	}

	static const VertexLayout* s_getLayout() {
		static const VertexLayout* s = VertexLayoutManager::current()->getLayout(s_type);
		return s;
	}
};

using Vertex_Pos		= VertexT_Pos<Tuple3f>;
using Vertex_PosColor	= VertexT_Color<Color4b, 1, Vertex_Pos>;

template<u8 Uv_Count> using Vertex_PosUv				= VertexT_Uv<Tuple2f, Uv_Count, Vertex_Pos>;
template<u8 Uv_Count> using Vertex_PosColorUv			= VertexT_Uv<Tuple2f, Uv_Count, Vertex_PosColor>;

template<u8 Uv_Count> using Vertex_PosNormalUv			= VertexT_Normal<Tuple3f, 1, Vertex_PosUv<Uv_Count>>;
template<u8 Uv_Count> using Vertex_PosColorNormalUv		= VertexT_Normal<Tuple3f, 1, Vertex_PosColorUv<Uv_Count>>;

template<u8 Uv_Count> using Vertex_PosTangentUv			= VertexT_Tangent<Tuple3f, 1, Vertex_PosNormalUv<Uv_Count>>;
template<u8 Uv_Count> using Vertex_PosColorTangentUv	= VertexT_Tangent<Tuple3f, 1, Vertex_PosColorNormalUv<Uv_Count>>;

template<u8 Uv_Count> using Vertex_PosBinormalUv		= VertexT_Binormal<Tuple3f, 1, Vertex_PosTangentUv<Uv_Count>>;
template<u8 Uv_Count> using Vertex_PosColorBinormalUv	= VertexT_Binormal<Tuple3f, 1, Vertex_PosColorTangentUv<Uv_Count>>;
}