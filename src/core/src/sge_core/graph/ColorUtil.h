#pragma once

#include "Color.h"

namespace sge {

struct ColorUtil
{
	ColorUtil() = delete;

	static int pixelSizeInBytes(ColorType t_);
};

#if 0
#pragma mark ===================== ColorUtil
#endif // 0
#if 1
inline int ColorUtil::pixelSizeInBytes(ColorType t_)
{
	using SRC = ColorType;
	switch (t_)
	{
		//case SRC::None:		return sizeof();
		//case SRC::Rf:			return sizeof();	
		//case SRC::Rb:			return sizeof();	
		//case SRC::RGf:		return sizeof();	
		//case SRC::RGb:		return sizeof();	
		//case SRC::RGBf:		return sizeof();	
		//case SRC::RGBb:		return sizeof();	
		case SRC::RGBAf:	return sizeof(ColorRGBAf);	
		case SRC::RGBAb:	return sizeof(ColorRGBAb);	
		//case SRC::BC1:		return sizeof();	
		//case SRC::BC2:		return sizeof();	
		//case SRC::BC3:		return sizeof();	
		//case SRC::BC4:		return sizeof();	
		//case SRC::BC5:		return sizeof();	
		//case SRC::BC6h:		return sizeof();	
		//case SRC::BC7:		return sizeof();	
	}
	SGE_ASSERT(false);
	return 0;
}

#endif // 1


}