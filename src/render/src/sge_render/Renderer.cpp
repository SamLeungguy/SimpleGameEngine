#include "Renderer.h"

#include "backend/dx11/Renderer_DX11.h"
#include "backend/dx11/RenderCommandBuffer_DX11.h"

#include "backend/opengl/Renderer_gl.h"
#include "backend/opengl/RenderCommandBuffer_gl.h"

namespace sge {

Renderer* Renderer::_pCurrent = nullptr;

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
	SGE_ASSERT(!_pCurrent);
	_pCurrent = this;
	_isVsync = true;
}

Renderer::~Renderer()
{
	SGE_ASSERT(_pCurrent);
	_pCurrent = nullptr;
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

}