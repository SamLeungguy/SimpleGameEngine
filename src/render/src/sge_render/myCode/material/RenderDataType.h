#if 0
#pragma once

#include <sge_core/base/Error.h>

namespace sge {

enum class RenderDataType : u8 {
	None,

	Int, UInt, Float, SNorm, UNorm,

	Int8, Int8x2, Int8x3, Int8x4,
	Int16, Int16x2, Int16x3, Int16x4,
	Int32, Int32x2, Int32x3, Int32x4,
	Int64, Int64x2, Int64x3, Int64x4,

	UInt8, UInt8x2, UInt8x3, UInt8x4,
	UInt16, UInt16x2, UInt16x3, UInt16x4,
	UInt32, UInt32x2, UInt32x3, UInt32x4,
	UInt64, UInt64x2, UInt64x3, UInt64x4,

	Float8, Float8x2, Float8x3, Float8x4,
	Float16, Float16x2, Float16x3, Float16x4,
	Float32, Float32x2, Float32x3, Float32x4,
	Float64, Float64x2, Float64x3, Float64x4,

	SNorm8, SNorm8x2, SNorm8x3, SNorm8x4,
	SNorm16, SNorm16x2, SNorm16x3, SNorm16x4,
	SNorm32, SNorm32x2, SNorm32x3, SNorm32x4,
	SNorm64, SNorm64x2, SNorm64x3, SNorm64x4,

	UNorm8, UNorm8x2, UNorm8x3, UNorm8x4,
	UNorm16, UNorm16x2, UNorm16x3, UNorm16x4,
	UNorm32, UNorm32x2, UNorm32x3, UNorm32x4,
	UNorm64, UNorm64x2, UNorm64x3, UNorm64x4,

	Float32x4x2, Float32x4x3, Float32x4x4,

};

struct RenderDataTypeUtil {
	using Type = RenderDataType;

	template<class T> static constexpr Type get();

	template<> static constexpr Type get<void>() { return Type::None; }

	template<> static constexpr Type get<i8 >() { return Type::Int8; }
	template<> static constexpr Type get<i16>() { return Type::Int16; }
	template<> static constexpr Type get<i32>() { return Type::Int32; }
	template<> static constexpr Type get<i64>() { return Type::Int64; }

	template<> static constexpr Type get<u8 >() { return Type::UInt8; }
	template<> static constexpr Type get<u16>() { return Type::UInt16; }
	template<> static constexpr Type get<u32>() { return Type::UInt32; }
	template<> static constexpr Type get<u64>() { return Type::UInt64; }

	template<> static constexpr Type get<f32>() { return Type::Float32; }
	template<> static constexpr Type get<f64>() { return Type::Float64; }

	template<> static constexpr Type get<Tuple2f>() { return Type::Float32x2; }
	template<> static constexpr Type get<Tuple2d>() { return Type::Float64x2; }

	template<> static constexpr Type get<Tuple3f>() { return Type::Float32x3; }
	template<> static constexpr Type get<Tuple3d>() { return Type::Float64x3; }

	template<> static constexpr Type get<Tuple4f>() { return Type::Float32x4; }
	template<> static constexpr Type get<Tuple4d>() { return Type::Float64x4; }

	template<> static constexpr Type get<Color4b>() { return Type::UNorm8x4; }

	static constexpr u16 getByteSize(RenderDataType type_)
	{
#define MY_TYPE_CASE_NOT_0(type, bitSize, count)	case Type::##type ##bitSize ##x ##count : return bitSize * count / 8; break

#define MY_TYPE_CASE_TYPE(type, bitSize)	\
		case Type::##type ##bitSize : return bitSize / 8; break;	\
		MY_TYPE_CASE_NOT_0(type, bitSize, 2);	\
		MY_TYPE_CASE_NOT_0(type, bitSize, 3);	\
		MY_TYPE_CASE_NOT_0(type, bitSize, 4)	\
//----------

#define MY_TYPE_CASE(type)	\
		MY_TYPE_CASE_TYPE(type, 8);		\
		MY_TYPE_CASE_TYPE(type, 16);	\
		MY_TYPE_CASE_TYPE(type, 32);	\
		MY_TYPE_CASE_TYPE(type, 64)		\
//----------------

		switch (type_)
		{
			MY_TYPE_CASE(Int);
			MY_TYPE_CASE(UInt);
			MY_TYPE_CASE(Float);
			MY_TYPE_CASE(SNorm);
			MY_TYPE_CASE(UNorm);
		default:	throw SGE_ERROR("{}", "invalid type");
		}

		return 0;
#undef MY_TYPE_CASE
#undef MY_TYPE_CASE_TYPE
#undef MY_TYPE_CASE_NOT_0
	}

	static constexpr u16 getBitSize(RenderDataType type_)
	{
#define MY_TYPE_CASE_NOT_0(type, bitSize, count)	case Type::##type ##bitSize ##x ##count : return bitSize * count; break

#define MY_TYPE_CASE_TYPE(type, bitSize)	\
		case Type::##type ##bitSize : return bitSize; break;	\
		MY_TYPE_CASE_NOT_0(type, bitSize, 2);	\
		MY_TYPE_CASE_NOT_0(type, bitSize, 3);	\
		MY_TYPE_CASE_NOT_0(type, bitSize, 4)	\
//----------

#define MY_TYPE_CASE(type)	\
		MY_TYPE_CASE_TYPE(type, 8);		\
		MY_TYPE_CASE_TYPE(type, 16);	\
		MY_TYPE_CASE_TYPE(type, 32);	\
		MY_TYPE_CASE_TYPE(type, 64)		\
//----------------

		switch (type_)
		{
			MY_TYPE_CASE(Int);
			MY_TYPE_CASE(UInt);
			MY_TYPE_CASE(Float);
			MY_TYPE_CASE(SNorm);
			MY_TYPE_CASE(UNorm);
		default:	throw SGE_ERROR("{}", "invalid type");
		}

		//return 0;

#undef MY_TYPE_CASE
#undef MY_TYPE_CASE_TYPE
#undef MY_TYPE_CASE_NOT_0
	}

	static constexpr u16 getCount(RenderDataType type_)
	{
#define MY_TYPE_CASE_NOT_MAT_AND_0(type, bitSize, count)	case Type::##type ##bitSize ##x ##count : return count; break

#define MY_TYPE_CASE_TYPE(type, bitSize)	\
		case Type::##type ##bitSize : return 1; break;	\
		MY_TYPE_CASE_NOT_MAT_AND_0(type, bitSize, 2);	\
		MY_TYPE_CASE_NOT_MAT_AND_0(type, bitSize, 3);	\
		MY_TYPE_CASE_NOT_MAT_AND_0(type, bitSize, 4)	\
//----------

#define MY_TYPE_CASE(type)	\
		MY_TYPE_CASE_TYPE(type, 8);		\
		MY_TYPE_CASE_TYPE(type, 16);	\
		MY_TYPE_CASE_TYPE(type, 32);	\
		MY_TYPE_CASE_TYPE(type, 64)		\
//----------------

		switch (type_)
		{
			MY_TYPE_CASE(Int);
			MY_TYPE_CASE(UInt);
			MY_TYPE_CASE(Float);
			MY_TYPE_CASE(SNorm);
			MY_TYPE_CASE(UNorm);
		default:	throw SGE_ERROR("{}", "invalid type");
		}

		//return 0;

#undef MY_TYPE_CASE
#undef MY_TYPE_CASE_TYPE
#undef MY_TYPE_CASE_NOT_0
	}

	static constexpr RenderDataType getBaseType(RenderDataType type_)
	{
#define MY_TYPE_CASE_NOT_0(type, bitSize, count)	case Type::##type ##bitSize ##x ##count : return Type::##type; break

#define MY_TYPE_CASE_TYPE(type, bitSize)	\
		case Type::##type ##bitSize : return Type::##type; break;	\
		MY_TYPE_CASE_NOT_0(type, bitSize, 2);	\
		MY_TYPE_CASE_NOT_0(type, bitSize, 3);	\
		MY_TYPE_CASE_NOT_0(type, bitSize, 4)	\
//----------

#define MY_TYPE_CASE(type)	\
		MY_TYPE_CASE_TYPE(type, 8);		\
		MY_TYPE_CASE_TYPE(type, 16);	\
		MY_TYPE_CASE_TYPE(type, 32);	\
		MY_TYPE_CASE_TYPE(type, 64)		\
//----------------

		switch (type_)
		{
			MY_TYPE_CASE(Int);
			MY_TYPE_CASE(UInt);
			MY_TYPE_CASE(Float);
			MY_TYPE_CASE(SNorm);
			MY_TYPE_CASE(UNorm);
		default:	throw SGE_ERROR("{}", "invalid type");
		}

		//return Type::None;
#undef MY_TYPE_CASE
#undef MY_TYPE_CASE_TYPE
#undef MY_TYPE_CASE_NOT_0
	}

	static constexpr const char* toString(RenderDataType type_)
	{
#define MY_TYPE_CASE_NOT_0(type, bitSize, count)	case Type::##type ##bitSize ##x ##count : return #type #bitSize "x" #count; break

#define MY_TYPE_CASE_TYPE(type, bitSize)	\
		case Type::##type ##bitSize : return #type #bitSize; break;	\
		MY_TYPE_CASE_NOT_0(type, bitSize, 2);	\
		MY_TYPE_CASE_NOT_0(type, bitSize, 3);	\
		MY_TYPE_CASE_NOT_0(type, bitSize, 4)	\
//----------

#define MY_TYPE_CASE(type)	\
		MY_TYPE_CASE_TYPE(type, 8);		\
		MY_TYPE_CASE_TYPE(type, 16);	\
		MY_TYPE_CASE_TYPE(type, 32);	\
		MY_TYPE_CASE_TYPE(type, 64)		\
//----------------

		switch (type_)
		{
			MY_TYPE_CASE(Int);
			MY_TYPE_CASE(UInt);
			MY_TYPE_CASE(Float);
			MY_TYPE_CASE(SNorm);
			MY_TYPE_CASE(UNorm);
		default:	throw SGE_ERROR("{}", "invalid type");
		}
		//return nullptr;
	}

	static RenderDataType getRenderDataType(const char* name_)
	{
		size_t hs = Math::hashStr(name_);

#define MY_ARRAY_ELEMENTS_BY_TYPE(hash, type, biteSize) hash(#type #biteSize), hash(#type #biteSize "x" "2"), hash(#type  #biteSize "x" "3"), hash(#type  #biteSize "x" "4")

#define MY_ARRAY_ELEMENTS_TYPE_8_TO_64(hash, type) \
		MY_ARRAY_ELEMENTS_BY_TYPE(hash, type, 8),	\
		MY_ARRAY_ELEMENTS_BY_TYPE(hash, type, 16),	\
		MY_ARRAY_ELEMENTS_BY_TYPE(hash, type, 32),	\
		MY_ARRAY_ELEMENTS_BY_TYPE(hash, type, 64)	\
//-----------

#define MY_IF_TYPE_NOT_0(type, biteSize, count, i)	\
	if(hs == compares[(i)]) return Type::type ## biteSize ## x ## count	\
//---------------

#define MY_TYPE_IF(type, biteSize, col, offset)								\
	if(hs == compares[(col * 4 + 0 + offset)]) return Type::type ## biteSize;	\
	MY_IF_TYPE_NOT_0(type, biteSize, 2, ((col) * 4 + 1 + (offset)));					\
	MY_IF_TYPE_NOT_0(type, biteSize, 3, ((col) * 4 + 2 + (offset)));					\
	MY_IF_TYPE_NOT_0(type, biteSize, 4, ((col) * 4 + 3 + (offset)))					\
//---------------

#define MY_TYPE_IF_8_TO_64(type, row, offset)			\
	MY_TYPE_IF(type, 8,  (4 * (row) + 0), (offset));		\
	MY_TYPE_IF(type, 16, (4 * (row) + 1), (offset));		\
	MY_TYPE_IF(type, 32, (4 * (row) + 2), (offset));		\
	MY_TYPE_IF(type, 64, (4 * (row) + 3), (offset))		\
//-------------

		// Int, UInt, Float, SNorm, UNorm,
		static const size_t compares[] = {
			Math::hashStr(""),
			Math::hashStr("Int"), Math::hashStr("UInt"), Math::hashStr("Float"), Math::hashStr("SNorm"), Math::hashStr("UNorm"),
			MY_ARRAY_ELEMENTS_TYPE_8_TO_64(Math::hashStr, Int),
			MY_ARRAY_ELEMENTS_TYPE_8_TO_64(Math::hashStr, UInt),
			MY_ARRAY_ELEMENTS_TYPE_8_TO_64(Math::hashStr, Float),
			MY_ARRAY_ELEMENTS_TYPE_8_TO_64(Math::hashStr, SNorm),
			MY_ARRAY_ELEMENTS_TYPE_8_TO_64(Math::hashStr, UNorm),
		};

		if (hs == compares[enumInt(Type::Int)])		return Type::Int;
		if (hs == compares[enumInt(Type::UInt)])	return Type::UInt;
		if (hs == compares[enumInt(Type::Float)])	return Type::Float;
		if (hs == compares[enumInt(Type::SNorm)])	return Type::SNorm;
		if (hs == compares[enumInt(Type::UNorm)])	return Type::UNorm;

		MY_TYPE_IF_8_TO_64(Int, 0, enumInt(Type::Int8));
		MY_TYPE_IF_8_TO_64(UInt, 1, enumInt(Type::Int8));
		MY_TYPE_IF_8_TO_64(Float, 2, enumInt(Type::Int8));
		MY_TYPE_IF_8_TO_64(SNorm, 3, enumInt(Type::Int8));
		MY_TYPE_IF_8_TO_64(UNorm, 4, enumInt(Type::Int8));

		throw SGE_ERROR("{}", "invalid");

#undef MY_ARRAY_ELEMENTS_BY_TYPE
#undef MY_ARRAY_ELEMENTS_TYPE_8_TO_64
#undef MY_IF_TYPE_NOT_0
#undef MY_TYPE_IF
#undef MY_TYPE_IF_8_TO_64
	}
};

}
#endif // 0
