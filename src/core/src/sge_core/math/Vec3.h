#pragma once

namespace sge {

template<class T>
class Vec3 {
public:
	using ElementType = T;
	static const size_t kElementCount = 3;

	union {
		struct { T x, y, z; };
		T data[kElementCount];
	};

	Vec3() = default;
	Vec3(const T& x_, const T& y_, const T& z_) : x(x_), y(y_), z(z_) {}

	Vec3 operator+(const Vec3& r) const { return Vec3(x + r.x, y + r.y, z + r.z); }
	Vec3 operator-(const Vec3& r) const { return Vec3(x - r.x, y - r.y, z - r.z); }
	Vec3 operator*(const Vec3& r) const { return Vec3(x * r.x, y * r.y, z * r.z); }
	Vec3 operator/(const Vec3& r) const { return Vec3(x / r.x, y / r.y, z / r.z); }

	Vec3 operator+(const T& s) const { return Vec3(x + s, y + s, z + s); }
	Vec3 operator-(const T& s) const { return Vec3(x - s, y - s, z - s); }
	Vec3 operator*(const T& s) const { return Vec3(x * s, y * s, z * s); }
	Vec3 operator/(const T& s) const { return Vec3(x / s, y / s, z / s); }
};

using Vec3f = Vec3<float>;

}