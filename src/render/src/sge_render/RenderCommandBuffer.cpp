#include "RenderCommandBuffer.h"
#include "Renderer.h"

#include "RenderCommands.h"

#include <new>

namespace sge {

RenderCommandBuffer::RenderCommandBuffer()
{
}

RenderCommandBuffer::~RenderCommandBuffer()
{
}

void RenderCommandBuffer::drawIndex(u32 indexCount_)
{
	void* p = Renderer::current()->getRenderCommnadBuffer()->_linearAllocator.allocate(sizeof(RenderCommand_Draw));

	//SGE_LOG("sizeof(RenderCommand_Draw): {}", sizeof(RenderCommand_Draw));

	RenderCommand_Draw cmd;
	cmd.IndexCount = indexCount_;
	new (p) RenderCommand_Draw(cmd);
}

void RenderCommandBuffer::setViewport(const Rect2i& rect_)
{
	void* p = Renderer::current()->getRenderCommnadBuffer()->_linearAllocator.allocate(sizeof(RenderCommand_SetViewport));
	//SGE_LOG("sizeof(RenderCommand_SetViewport): {}", sizeof(RenderCommand_SetViewport));

	RenderCommand_SetViewport cmd;
	cmd.rect.x = rect_.x;
	cmd.rect.y = rect_.y;
	cmd.rect.w = rect_.w;
	cmd.rect.h = rect_.h;
	//RenderCommand_Base* pBase = static_cast<RenderCommand_SetViewport*>(p);
	new (p) RenderCommand_SetViewport(cmd);
}

void RenderCommandBuffer::execute()
{
	onExecute();
}

}