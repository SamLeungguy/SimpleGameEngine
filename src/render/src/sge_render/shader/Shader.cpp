#include "Shader.h"
#include <sge_render/Renderer.h>

namespace sge {

Shader::Shader(StrView filename) 
	:_filename(filename)
{
	auto* proj = ProjectSettings::instance();
	auto infoFilename = Fmt("{}/{}/info.json", proj->importedPath(), filename);
	JsonUtil::readFile(infoFilename, _info);
}

Shader::~Shader()
{
	auto* renderer = Renderer::instance();
	renderer->onShaderDestory(this);
}

ShaderPass::ShaderPass(Shader* pShader_, ShaderInfo::Pass& info_)
	:
	_shader(pShader_),
	_info(&info_)
{

}

}