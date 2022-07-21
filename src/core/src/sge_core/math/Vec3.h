#pragma once

#include "Vec2.h"
#include "Vec3_Basic.h"
#include "Vec3_SSE.h"

namespace sge {

#ifndef SGE_MATH_USE_SSE
#error "Please include sge_core-config.h"
#elif SGE_MATH_USE_SSE
template<class T> using Vec3 = Vec3_SSE<T>;
#else
template<class T> using Vec3 = Vec3_Basic<T>;
#endif

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;

using Vec3i = Vec3<int>;
using Vec3u = Vec3<u32>;

}
