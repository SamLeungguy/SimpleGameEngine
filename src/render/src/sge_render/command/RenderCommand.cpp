#include "RenderCommand.h"
#include "../mesh/RenderMesh.h"

namespace sge {
RenderCommandBuffer::RenderCommandBuffer()
{
	_allocator.init();
}

void RenderCommandBuffer::drawMesh(RenderMesh& mesh_)
{
	auto* cmd = newCommand<RenderCommand_DrawCall>();
	cmd->primitive = mesh_.getPrimitive();
	cmd->pVertexLayout = mesh_.getVertexLayout();
	cmd->spVertexBuffer = mesh_.getVertexBuffer();
	cmd->spIndexBuffer = mesh_.getIndexBuffer();
	cmd->vertexCount = mesh_.getVertexCount();
	cmd->indexCount = mesh_.getIndexCount();
}

void RenderCommandBuffer::reset()
{
	for (auto* cmd : _commands) {
		cmd->~RenderCommand();
	}
	_allocator.clear();
	_commands.clear();
}

}
