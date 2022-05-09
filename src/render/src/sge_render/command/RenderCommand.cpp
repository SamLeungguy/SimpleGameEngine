#include "RenderCommand.h"
#include "../mesh/RenderMesh.h"

namespace sge {
RenderCommandBuffer::RenderCommandBuffer()
{
	_allocator.init();
}

void RenderCommandBuffer::drawMesh(const SrcLoc& debugLoc_, const RenderMesh& mesh_)
{
	for (auto& sm : mesh_.subMeshes()) {
		drawSubMesh(debugLoc_, sm);
	}
}

void RenderCommandBuffer::drawSubMesh(const SrcLoc& debugLoc_, const RenderSubMesh& subMesh_)
{
	auto* cmd = newCommand<RenderCommand_DrawCall>();

#if _DEBUG
	cmd->debugLoc = debugLoc_;
#endif // _DEBUG


	cmd->primitive		= subMesh_.getPrimitive();
	cmd->pVertexLayout	= subMesh_.getVertexLayout();
	cmd->spVertexBuffer	= subMesh_.getVertexBuffer();
	cmd->vertexCount	= subMesh_.getVertexCount();
	cmd->spIndexBuffer	= subMesh_.getIndexBuffer();
	cmd->indexType		= subMesh_.getIndexType();
	cmd->indexCount		= subMesh_.getIndexCount();
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
