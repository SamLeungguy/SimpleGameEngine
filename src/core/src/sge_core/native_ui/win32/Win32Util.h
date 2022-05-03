#pragma once
#include "sge_core/math/Rect2.h"

namespace sge {

struct Win32Util {
	Win32Util() = delete;

	static Rect2f toRect2f(const RECT& rc_) {
		return Rect2f(static_cast<float>(rc_.left),
			static_cast<float>(rc_.top),
			static_cast<float>(rc_.right - rc_.left),
			static_cast<float>(rc_.bottom - rc_.top));
	}

};

}