#pragma once

namespace sge {

template<class T>
class Color2 {
public:
	using ElementType = T;
	static const size_t kElementCount = 2;

	union {
		struct { T r, g; };
		T data[kElementCount];
	};

	Color2() = default;
	Color2(const T& r_, const T& g_) : r(r_), g(g_) {}

	Color2 operator+(const Color2& r) const { return Color2(r + r.r, g + r.g); }
	Color2 operator-(const Color2& r) const { return Color2(r - r.r, g - r.g); }
	Color2 operator*(const Color2& r) const { return Color2(r * r.r, g * r.g); }
	Color2 operator/(const Color2& r) const { return Color2(r / r.r, g / r.g); }

	Color2 operator+(const T& s) const { return Color2(r + s, g + s); }
	Color2 operator-(const T& s) const { return Color2(r - s, g - s); }
	Color2 operator*(const T& s) const { return Color2(r * s, g * s); }
	Color2 operator/(const T& s) const { return Color2(r / s, g / s); }
};

using Color2b = Color2<char>;
using Color2f = Color2<float>;

template<class T>
class Color4 {
public:
	using ElementType = T;
	static const size_t kElementCount = 4;

	union {
		struct { T r, g, b, a; };
		T data[kElementCount];
	};

	Color4() = default;
	Color4(const T& r_, const T& g_,const T& b_, const T& a_) : r(r_), g(g_), b(b_), a(a_) {}

	Color4 operator+(const Color4& r) const { return Color4(r + r.r, g + r.g); }
	Color4 operator-(const Color4& r) const { return Color4(r - r.r, g - r.g); }
	Color4 operator*(const Color4& r) const { return Color4(r * r.r, g * r.g); }
	Color4 operator/(const Color4& r) const { return Color4(r / r.r, g / r.g); }

	Color4 operator+(const T& s) const { return Color4(r + s, g + s); }
	Color4 operator-(const T& s) const { return Color4(r - s, g - s); }
	Color4 operator*(const T& s) const { return Color4(r * s, g * s); }
	Color4 operator/(const T& s) const { return Color4(r / s, g / s); }
};

using Color4b = Color4<char>;
using Color4f = Color4<float>;
}