#pragma once

#include "Material.h"

namespace sge {

class Shader : public NonCopyable
{
public:
	static Material* s_find(StrView filename_);

private:
	VectorMap<size_t, Material*> materialMap;
};

}