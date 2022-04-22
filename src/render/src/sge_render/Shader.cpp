#include "Shader.h"
#include "Renderer.h"

namespace sge {
Shader::Shader(CreateDesc& desc_)
{
}

Shader* Shader::create(CreateDesc& desc_)
{
	return  Renderer::current()->onCreateShader(desc_);
}
}