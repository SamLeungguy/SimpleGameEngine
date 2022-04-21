#include "RenderCommandBuffer_DX11.h"


#include <sge_core/memory/Chunk.h>

namespace sge {

RenderCommandBuffer_DX11::~RenderCommandBuffer_DX11()
{

}

void RenderCommandBuffer_DX11::onExecute()
{
	Base::onExecute();

	auto& chunkList = _linearAllocator.getChunkList();

	RenderCommandDispatcher dispatcher;

	//SGE_LOG_WARN("==================chunkList size: {}", chunkList.size());

	int nCmd = 0;
	for (auto itList = chunkList.begin(); itList != _linearAllocator.getCurrentIterator().next(); itList++)
	{
		Chunk& chunk = *itList;
		u8* pChunkData = static_cast<u8*>(chunk.getData());
		u64 offset = 0;

		//SGE_LOG("chunk.getByteSize() {}", chunk.getByteSize());

		for (; offset < chunk.getByteSize();)
		{
			RenderCommand_Base* p = static_cast<RenderCommand_Base*>(static_cast<void*>(pChunkData + offset));
			dispatcher.set(p);
			dispatcher.dispatch<RenderCommand_Draw>(SGE_RENDER_CMD_BIND_FN(RenderCommandBuffer_DX11::_excuteDraw));
			dispatcher.dispatch<RenderCommand_SetViewport>(SGE_RENDER_CMD_BIND_FN(RenderCommandBuffer_DX11::_excuteSetViewport));
			offset += dispatcher.cmdSize;
			//SGE_LOG("dispatcher.cmdSize {}", dispatcher.cmdSize);

			nCmd++;
		}
		nCmd--;
	}

	//SGE_LOG_WARN("==================Cmd Count: {}", nCmd);

	_linearAllocator.clear();
}

u64 RenderCommandBuffer_DX11::_excuteDraw(RenderCommand_Draw* pCmd_)
{
	//SGE_LOG("_excuteDraw!");
	//SGE_LOG("IndexCount: {}", pCmd_->IndexCount);

	return sizeof(RenderCommand_Draw);
}

u64 RenderCommandBuffer_DX11::_excuteSetViewport(RenderCommand_SetViewport* pCmd_)
{
	//SGE_LOG("_excuteSetViewport!");
	//SGE_LOG("rect {} {} {} {}", pCmd_->rect.x, pCmd_->rect.y, pCmd_->rect.w, pCmd_->rect.h);

	return sizeof(RenderCommand_SetViewport);
}

}