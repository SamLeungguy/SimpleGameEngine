#pragma once

#include "sge_render/shader/Shader.h"

namespace sge {

struct Material_CreateDesc
{

};

class Material : public Object
{
public:
	Material(Material_CreateDesc& desc_);
private:
};

}