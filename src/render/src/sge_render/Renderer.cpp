#include "Renderer.h"

#include "backend/dx11/Renderer_DX11.h"

#include "backend/opengl/Renderer_gl.h"
#include "backend/opengl/RenderCommandBuffer_gl.h"

namespace sge {

Renderer* Renderer::s_pInstance = nullptr;

Renderer::CreateDesc::CreateDesc()
{
#if SGE_OS_WINDOWS
	apiType = ApiType::DX11;
#else
	apiType = ApiType::None;
#endif // SGE_OS_WINDOWS

	isMultithread = false;
}

Renderer::Renderer()
{
	SGE_ASSERT(!s_pInstance);
	s_pInstance = this;
	_isVsync = true;
}

Renderer::~Renderer()
{
	SGE_ASSERT(s_pInstance);
	s_pInstance = nullptr;
}

VertexLayout* Renderer::createVertexLayout()
{
	return nullptr;
}

Renderer* Renderer::create(CreateDesc& desc_)
{
	Renderer* p = nullptr;

	switch (desc_.apiType)
	{
		case ApiType::DX11:		p = new Renderer_DX11(desc_);	break;
		case ApiType::OpenGL:	p = new Renderer_gl(desc_);	break;

		default: throw SGE_ERROR("unsupported graphic api!");
	}

	return p;
}

SPtr<Shader> Renderer::createShader(StrView filename_)
{
	TempString tmpName = filename_;

	auto it = _shaders.find(tmpName.c_str());
	if (it != _shaders.end()) {
		return it->second;
	}

	auto s = onCreateShader(tmpName);
	_shaders[tmpName.c_str()] = s.ptr();
	return s;
}

void Renderer::onShaderDestory(Shader* pShader_)
{
	_shaders.erase(pShader_->filename().c_str());
}

}