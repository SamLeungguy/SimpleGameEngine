#include "RenderCommand.h"
#include "../mesh/RenderMesh.h"


namespace sge {
RenderCommandBuffer::RenderCommandBuffer()
{
	_allocator.init();
}

void RenderCommandBuffer::drawMesh(const SrcLoc& debugLoc_, const RenderMesh& mesh_, Material* pMaterial_)
{
	for (auto& sm : mesh_.subMeshes()) {
		drawSubMesh(debugLoc_, sm, pMaterial_);
	}
}

void RenderCommandBuffer::drawSubMesh(const SrcLoc& debugLoc_, const RenderSubMesh& subMesh_, Material* pMaterial_)
{
	if (!pMaterial_) { SGE_ASSERT(false); return; }

	auto passes = pMaterial_->passes();

	for (size_t i = 0; i < passes.size(); i++)
	{
		auto* cmd = newCommand<RenderCommand_DrawCall>();

		#if _DEBUG
				cmd->debugLoc = debugLoc_;
		#endif // _DEBUG
		cmd->spMaterial			= pMaterial_;
		cmd->materialPassIndex	= i;

		cmd->primitive		= subMesh_.getPrimitive();
		cmd->pVertexLayout	= subMesh_.getVertexLayout();
		cmd->spVertexBuffer	= subMesh_.getVertexBuffer();
		cmd->vertexCount	= subMesh_.getVertexCount();
		cmd->spIndexBuffer	= subMesh_.getIndexBuffer();
		cmd->indexType		= subMesh_.getIndexType();
		cmd->indexCount		= subMesh_.getIndexCount();
	}
}

void RenderCommandBuffer::test_drawMesh(const SrcLoc& debugLoc_, const RenderMesh& mesh_, SPtr<Material>& spMaterial_)
{
	//auto* cmd = newCommand<RenderCommand_DrawCall>();

#if _DEBUG
	//cmd->debugLoc = debugLoc_;
#endif // _DEBUG

	/*for (auto& pass : spMaterial_->getShaderPasses())
	{
		for (auto& sm : mesh_.subMeshes()) {
			cmd->primitive = sm.getPrimitive();
			cmd->pVertexLayout = sm.getVertexLayout();
			cmd->spVertexBuffer = sm.getVertexBuffer();
			cmd->vertexCount = sm.getVertexCount();
			cmd->spIndexBuffer = sm.getIndexBuffer();
			cmd->indexType = sm.getIndexType();
			cmd->indexCount = sm.getIndexCount();
			cmd->spRenderShader.reset(pass._spRenderShader);
		}
	}*/

	/*for (auto& sm : mesh_.subMeshes()) {
		cmd->primitive = sm.getPrimitive();
		cmd->pVertexLayout = sm.getVertexLayout();
		cmd->spVertexBuffer = sm.getVertexBuffer();
		cmd->vertexCount = sm.getVertexCount();
		cmd->spIndexBuffer = sm.getIndexBuffer();
		cmd->indexType = sm.getIndexType();
		cmd->indexCount = sm.getIndexCount();
		cmd->spRenderShader.reset(spMaterial_->getShaderPasses()[0]._spRenderShader);
	}*/
}

void RenderCommandBuffer::test_drawTerrain(const SrcLoc& debugLoc_, Terrain& terrain_,  Material* pMaterial_)
{
	//terrain_.update();

#if 1
	auto& renderMesh = terrain_.getRenderMesh();
	auto& subMesh = renderMesh.subMeshes()[0];

	for (auto& patch : terrain_.getPatches())
	{
		auto* cmd = newCommand<RenderCommand_DrawCall>();

#if _DEBUG
		cmd->debugLoc = debugLoc_;
#endif // _DEBUG
		cmd->spMaterial			= pMaterial_;
		cmd->materialPassIndex	= 0;

		cmd->primitive		= RenderPrimitiveType::Triangles;
		cmd->pVertexLayout	= subMesh.getVertexLayout();
		cmd->spVertexBuffer.reset(patch->spVertexBuffer);
		cmd->vertexCount	= patch->spVertexBuffer->elementCount();
		cmd->spIndexBuffer.reset(patch->spIndexBuffer);
		cmd->indexType		= RenderDataTypeUtil::get<Terrain::IndexType>();
		cmd->indexCount		= patch->spIndexBuffer->elementCount();

		cmd->_spTerrainPatch.reset(patch);
	}
#else
	for (auto& sm : terrain_.getRenderMesh().subMeshes()) {
		drawSubMesh(debugLoc_, sm, pMaterial_);
	}
	
	for (auto& sm : terrain_.getRenderMesh2().subMeshes()) {
		drawSubMesh(debugLoc_, sm, pMaterial_);
	}
#endif // 0
	
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
