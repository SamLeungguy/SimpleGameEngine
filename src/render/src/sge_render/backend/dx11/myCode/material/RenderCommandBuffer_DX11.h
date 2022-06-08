//#pragma once
//
//#if SGE_RENDER_HAS_DX11
//
//#include "Render_Common_DX11.h"
//
//#include "../../command/RenderCommandBuffer.h"
//#include "../../command/RenderCommands.h"
//
//namespace sge {
//
//class RenderCommandBuffer_DX11 : public RenderCommandBuffer
//{
//	using Base = RenderCommandBuffer;
//	using Util = DX11Util;
//public:
//	RenderCommandBuffer_DX11() = default;
//	virtual ~RenderCommandBuffer_DX11();
//
//protected:
//	virtual void onExecute() override;
//
//private:
//	u64 _excuteDraw(RenderCommand_Draw* pCmd_);
//	u64 _excuteSetViewport(RenderCommand_SetViewport* pCmd_);
//
//};
//
//}
//#endif
//
