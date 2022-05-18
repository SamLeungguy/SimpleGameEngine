#include "Shader.h"
#include "../Renderer.h"

namespace sge {

Material* Shader::s_find(StrView filename_)
{
	Material_CreateDesc desc;
	desc.filename = filename_;
	return Renderer::current()->createMaterial(desc);
}

}