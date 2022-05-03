#pragma once

#include "../detect_platform/sge_detect_platform.h"

#if SGE_OS_WINDOWS
#define NOMINMAX 1

#include <WinSock2.h> // WinSock2.h must include before windows.h to avoid winsock1 define
#include <ws2tcpip.h> // struct sockaddr_in6
#pragma comment(lib, "Ws2_32.lib")

#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h> // struct sockaddr_in
#endif

#include <cassert>
#include <exception>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <atomic>

#include <EASTL/vector.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/string.h>
#include <EASTL/fixed_string.h>
#include <EASTL/string_view.h>
#include <EASTL/span.h>
#include <EASTL/list.h>

#include <EASTL/optional.h>

#include <EASTL/map.h>
#include <EASTL/hash_map.h>
#include <EASTL/vector_map.h>
#include <EASTL/string_map.h>

#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/weak_ptr.h>

#include "sge_macro.h"

//==== EASTL ====

#if !EASTL_DLL // If building a regular library and not building EASTL as a DLL...
	// It is expected that the application define the following
	// versions of operator new for the application. Either that or the
	// user needs to override the implementation of the allocator class.
inline void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line) { return malloc(size); }
inline void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line) {
#if SGE_OS_WINDOWS
	return _aligned_malloc(size, alignment);
#else
	return std::aligned_alloc(alignment, size);
#endif
}
#endif

//===============

namespace sge {

template<class T> inline constexpr typename std::underlying_type<T>::type  enumInt(T  value)				{ return static_cast<typename std::underlying_type<T>::type>(value); }
template<class T> inline constexpr typename std::underlying_type<T>::type& enumIntRef(T& value)				{ return *reinterpret_cast<typename std::underlying_type<T>::type*>(&value); }
template<class T> inline constexpr typename std::underlying_type<T>::type const& enumIntRef(const T& value) { return *reinterpret_cast<const typename std::underlying_type<T>::type*>(&value); }

template<class T> inline bool constexpr enumHas(const T& a, const T& b) { return static_cast<T>(enumInt(a) & enumInt(b)) != static_cast<T>(0); }

template<class T> SGE_INLINE T* constCast(const T* v) { return const_cast<T*>(v); }
template<class T> SGE_INLINE T& constCast(const T& v) { return const_cast<T&>(v); }

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

// using f16 = half;
using f32 = float;
using f64 = double;
using f128 = long double;

template<class Obj, class Member> constexpr
intptr_t memberOffset(Member Obj::*ptrToMember_, size_t index_ = 0)
{
	Obj* p = nullptr;
	Member* pM = &(p->*ptrToMember_) + index_;
	return reinterpret_cast<intptr_t>(pM);
}

template<class T> using UPtr = eastl::unique_ptr<T>;
//template<class T> using SPtr = eastl::shared_ptr<T>;
template<class T> using WPtr = eastl::weak_ptr<T>;

template<class T, typename ... Args>
UPtr<T> createUPtr(Args&& ... args_)
{
	return eastl::unique_ptr<T>(eastl::forward<Args>(args_)...);
};

template<class T> using Span = eastl::span<T>;
template<class T, size_t N, bool bEnableOverflow = true> using Vector_ = eastl::fixed_vector<T, N, bEnableOverflow>;

template<class T> using Vector = eastl::vector<T>;
template<class KEY, class VALUE> using Map = eastl::map<KEY, VALUE>;
template<class KEY, class VALUE> using VectorMap = eastl::vector_map<KEY, VALUE>;

template<class T> using Opt = eastl::optional<T>;

template<class T> using List = eastl::list<T>;

template<class T> using StrViewT = eastl::basic_string_view<T>;
using StrViewA = StrViewT<char>;
using StrViewW = StrViewT<wchar_t>;

template<class T, size_t N, bool bEnableOverflow = true> // using FixedStringT = eastl::fixed_string<T, N, bEnableOverflow>;
class StringT : public eastl::fixed_string<T, N, bEnableOverflow> {
	using Base = eastl::fixed_string<T, N, bEnableOverflow>;
public:
	StringT() = default;
	StringT(StrViewT<T> view) : Base(view.data(), view.size()) {}
	StringT(StringT&& str) : Base(std::move(str)) {}

	template<class R> void operator=(R&& r) { Base::operator=(SGE_FORWARD(r)); }
};

using StringA = eastl::basic_string<char>;
using StringW = eastl::basic_string<wchar_t>;

template<size_t N, bool bEnableOverflow = true> using StringA_ = StringT<char, N, bEnableOverflow>;
template<size_t N, bool bEnableOverflow = true> using StringW_ = StringT<wchar_t, N, bEnableOverflow>;

using TempStringA = StringA_<220>;
using TempStringW = StringW_<220>;

using StrView = StrViewA;
using String = StringA;

template<size_t N> using String_ = StringA_<N>;
using TempString = TempStringA;

template<size_t N> struct CharBySize;
template<> struct CharBySize<1> { using Type = char; };
template<> struct CharBySize<2> { using Type = char16_t; };
template<> struct CharBySize<4> { using Type = char32_t; };

struct WCharUtil {
	using Char = typename CharBySize<sizeof(wchar_t)>::Type;
	Char    toChar(wchar_t c) { return static_cast<Char>(c); }
	wchar_t toWChar(Char    c) { return static_cast<wchar_t>(c); }
};


//! Source Location
class SrcLoc {
public:
	SrcLoc() = default;
	SrcLoc(const char* file_, int line_, const char* func_)
		: file(file_)
		, line(line_)
		, func(func_) {
	}

	const char* file = "";
	const char* func;
	int line = 0;
};

class NonCopyable {
public:
	NonCopyable() = default;

private:
	NonCopyable(NonCopyable&&) = delete;

	NonCopyable(const NonCopyable&) = delete;
	void operator=(const NonCopyable&) = delete;
};

template<class T> inline void sge_delete(T* p) { delete p; }

template<class T>
class ComPtr : public NonCopyable
{
public:
	ComPtr() = default;
	~ComPtr() noexcept { reset(nullptr); }
	ComPtr(const ComPtr& rhs_) { reset(rhs_._p); }
	void operator=(const ComPtr& rhs_) { if (_p == rhs_._p) return; reset(rhs_._p); }

	T* operator->() noexcept			{ return _p; }
	operator T* () noexcept				{ return _p; }

			T* ptr() noexcept			{ return _p; }
	const	T* ptr() const  noexcept	{ return _p; }

	void reset(T* p_)
	{
		if (_p == p_)
			return;

		if (_p)
		{
			_p->Release();
			_p = nullptr;
		}
		_p = p_;
		if (_p)
		{
			_p->AddRef();
		}
	}

	T** ptrForInit() noexcept { reset(nullptr); return &_p; }

	T* detach() { T* pOut = _p = nullptr; return o; }

private:
	T* _p = nullptr;
};

class RefCountBase : public NonCopyable
{
public:
	std::atomic_int _refCount = 0;
};

class Object : public RefCountBase {
public:
	virtual ~Object() = default;
};

template<class T>
class SPtr : public NonCopyable
{
public:
	SPtr() = default;
	~SPtr() noexcept { reset(nullptr); }
	SPtr(T* ptr_) { reset(ptr_); }
	void operator=(T* ptr_) { if (ptr_ == _ptr) return; reset(ptr_); }

	SPtr(SPtr&& rhs_) { reset(rhs_._ptr); rhs_._ptr = nullptr; }
	void operator=(SPtr&& rhs_) { if (_ptr == rhs_._ptr) return; reset(rhs_._ptr); rhs_._ptr = nullptr; }

	T* operator->() noexcept			{ return _ptr; }
	operator T* () noexcept				{ return _ptr; }

			T* ptr() noexcept			{ return _ptr; }
	const	T* ptr() const  noexcept	{ return _ptr; }

	void reset(T* ptr_)
	{
		static_assert(std::is_base_of<RefCountBase, T>::value, "");
		if (_ptr == ptr_)
			return;

		if (_ptr)
		{
			auto c = --_ptr->_refCount;
			if (c <= 0)
			{
				sge_delete(_ptr);
			}
			_ptr = nullptr;
		}

		_ptr = ptr_;
		if (ptr_)
		{
			_ptr->_refCount++;
		}
	}

	T* detach() { T* o = _ptr; _ptr = nullptr; return o; }

private:
	T* _ptr = nullptr;
};

} // namespace
