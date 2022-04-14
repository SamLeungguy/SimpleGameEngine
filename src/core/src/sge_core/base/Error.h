#pragma once

#include "../string/String.h"
#include "../string/Fmt.h"

#include "sge_base.h"


#define SGE_ERROR(...) Error(SGE_SRC_LOC, Fmt(__VA_ARGS__))

namespace sge {

	class IError : public std::exception
	{
	public:

	};

	class Error : public IError
	{
	public:
		Error(const SrcLoc& loc_, StrView msg_);

	private:
		SrcLoc _loc;
		TempString _msg;
	};
}