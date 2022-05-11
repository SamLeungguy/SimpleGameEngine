#pragma once

namespace sge {

namespace Math {

template<class T> constexpr T max(const T& a, const T& b) { return a > b ? a : b; }
template<class T> constexpr T min(const T& a, const T& b) { return a < b ? a : b; }


template<class T> constexpr T byteToBit(const T& v) { return v * 8; }
template<class T> constexpr T byteToK(const T& v) { return v / 1024; }
template<class T> constexpr T byteToM(const T& v) { return v / (1024 * 1024); }
template<class T> constexpr T byteToG(const T& v) { return v / (1024 * 1024 * 1024); }

//template<class T> constexpr size_t hashValue(const T& value) { static eastl::hash<T> h; return h(T); }

inline size_t hashCStr(const char* value) { static eastl::hash<const char*> h; return h(value); }
inline size_t hashStr(const String& value) { return hashCStr(value.c_str()); }

struct _Helper {
	template<class T>
	static constexpr T alignTo_uint(T n, T a) {
		static_assert(std::is_unsigned<T>::value, "");
		T r = n % a;
		return r ? (n + a - r) : n;
		//	return (n + a - 1) / a * a;
	}
};

constexpr size_t alignTo(size_t n, size_t a) { return _Helper::alignTo_uint(n, a); }

}

}