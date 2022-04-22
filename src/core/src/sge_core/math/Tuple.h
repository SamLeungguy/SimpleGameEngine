#pragma once
#include "Vec2.h"
#include "Vec3.h"

namespace sge {

template<class T>
class Tuple2 {
public:
	using ElementType = T;
	static const size_t kElementCount = 2;

	union {
		struct { T x, y; };
		T data[kElementCount];
	};

	Tuple2() = default;
	Tuple2(const T& x_, const T& y_) : x(x_), y(y_) {}
	Tuple2(const Vec2<T>& v_) : x(v_.x), y(v_.y) {}

	Tuple2 operator+(const Tuple2& r) const { return Tuple2(x + r.x, y + r.y); }
	Tuple2 operator-(const Tuple2& r) const { return Tuple2(x - r.x, y - r.y); }
	Tuple2 operator*(const Tuple2& r) const { return Tuple2(x * r.x, y * r.y); }
	Tuple2 operator/(const Tuple2& r) const { return Tuple2(x / r.x, y / r.y); }

	Tuple2 operator+(const T& s) const { return Tuple2(x + s, y + s); }
	Tuple2 operator-(const T& s) const { return Tuple2(x - s, y - s); }
	Tuple2 operator*(const T& s) const { return Tuple2(x * s, y * s); }
	Tuple2 operator/(const T& s) const { return Tuple2(x / s, y / s); }
};
using Tuple2f = Tuple2<float>;

template<class T>
class Tuple3 {
public:
	using ElementType = T;
	static const size_t kElementCount = 3;

	union {
		struct { T x, y, z; };
		T data[kElementCount];
	};

	Tuple3() = default;
	Tuple3(const T& x_, const T& y_, const T& z_) : x(x_), y(y_), z(z_) {}
	Tuple3(const Vec3<T>& v_) : x(v_.x), y(v_.y), z(v_.z) {}

	Tuple3 operator+(const Tuple3& r) const { return Tuple3(x + r.x, y + r.y, z + r.z); }
	Tuple3 operator-(const Tuple3& r) const { return Tuple3(x - r.x, y - r.y, z - r.z); }
	Tuple3 operator*(const Tuple3& r) const { return Tuple3(x * r.x, y * r.y, z * r.z); }
	Tuple3 operator/(const Tuple3& r) const { return Tuple3(x / r.x, y / r.y, z / r.z); }

	Tuple3 operator+(const T& s) const { return Tuple3(x + s, y + s, z + s); }
	Tuple3 operator-(const T& s) const { return Tuple3(x - s, y - s, z - s); }
	Tuple3 operator*(const T& s) const { return Tuple3(x * s, y * s, z * s); }
	Tuple3 operator/(const T& s) const { return Tuple3(x / s, y / s, z / s); }
};

using Tuple3f = Tuple3<float>;

}