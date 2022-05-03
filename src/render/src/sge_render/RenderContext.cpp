#include "RenderContext.h"
#include "Renderer.h"

namespace sge {

RenderContext::RenderContext(CreateDesc& desc_)
{

}

void RenderContext::setFrameBufferSize(Vec2f newSize_)
{
	if (_frameBufferSize == newSize_)
		return;

	_frameBufferSize = newSize_;
	onSetFrameBufferSize(newSize_);
}

}