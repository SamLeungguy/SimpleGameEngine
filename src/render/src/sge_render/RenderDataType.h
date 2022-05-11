#pragma once

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

	Float32x4x2, Float32x4x3, Float32x4x4,

	SNorm8, SNorm8x2, SNorm8x3, SNorm8x4,
	SNorm16, SNorm16x2, SNorm16x3, SNorm16x4,
	SNorm32, SNorm32x2, SNorm32x3, SNorm32x4,
	SNorm64, SNorm64x2, SNorm64x3, SNorm64x4,

	UNorm8, UNorm8x2, UNorm8x3, UNorm8x4,
	UNorm16, UNorm16x2, UNorm16x3, UNorm16x4,
	UNorm32, UNorm32x2, UNorm32x3, UNorm32x4,
	UNorm64, UNorm64x2, UNorm64x3, UNorm64x4,
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

		return 0;
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

		return 0;
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

		return Type::None;
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
		return nullptr;
	}

};

}