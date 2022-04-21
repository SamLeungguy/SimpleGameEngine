#pragma once

namespace sge {

template<class T>
class Vec4 {
public:
	using ElementType = T;
	static const size_t kElementCount = 4;

	union {
		struct { T x, y, z, w; };
		T data[kElementCount];
	};

	Vec4() = default;
	Vec4(const T& x_, const T& y_, const T& z_, const T& w_) : x(x_), y(y_), z(z_), w(w_) {}

	Vec4 operator+(const Vec4& r) const { return Vec4(x + r.x, y + r.y, z + r.z, w + r.w); }
	Vec4 operator-(const Vec4& r) const { return Vec4(x - r.x, y - r.y, z - r.z, w - r.w); }
	Vec4 operator*(const Vec4& r) const { return Vec4(x * r.x, y * r.y, z * r.z, w * r.w); }
	Vec4 operator/(const Vec4& r) const { return Vec4(x / r.x, y / r.y, z / r.z, w / r.w); }

	Vec4 operator+(const T& s) const { return Vec4(x + s, y + s, z + s, w + s); }
	Vec4 operator-(const T& s) const { return Vec4(x - s, y - s, z - s, w - s); }
	Vec4 operator*(const T& s) const { return Vec4(x * s, y * s, z * s, w * s); }
	Vec4 operator/(const T& s) const { return Vec4(x / s, y / s, z / s, w / s); }
};

using Vec4f = Vec4<float>;

}