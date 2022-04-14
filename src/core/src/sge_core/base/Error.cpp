#include "Error.h"

namespace sge {

	Error::Error(const SrcLoc& loc_, StrView msg_)
		:
		_loc(loc_), _msg(msg_)
	{
	}
}