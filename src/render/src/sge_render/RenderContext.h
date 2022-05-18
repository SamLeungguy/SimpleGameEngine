#pragma once

#include "command/RenderCommand.h"

namespace sge {

struct RenderContext_CreateDesc 
{
	NativeUIWindow* pWindow = nullptr;
};

class RenderContext : public Object
{
public:
	using CreateDesc = RenderContext_CreateDesc;

	RenderContext(CreateDesc& desc_);
	virtual ~RenderContext() = default;

	void beginRender()	{ onBeginRender(); }
	void endRender()	{ onEndRender(); }

	void setFrameBufferSize(Vec2f newSize);

	void commit(RenderCommandBuffer& cmdBuf_) { onCommit(cmdBuf_); }

protected:
	virtual void onBeginRender() {};
	virtual void onEndRender() {};
	virtual void onSetFrameBufferSize(Vec2f newSize_) {};
	virtual void onCommit(RenderCommandBuffer& cmdBuf_) {}

	Vec2f	_frameBufferSize = { 0,0 };

	template<class Impl>
	void _dispatch(Impl* pImpl_, RenderCommandBuffer& cmdBuf_)
	{
		using Cmd = RenderCommandType;

		#define CMD_CASE(E)	\
			case Cmd::E: \
			{	\
				auto* p = static_cast<RenderCommand_##E*>(cmd);	\
				pImpl_->onCmd_##E(*p);\
			} break;	\
		//---------------

		for (auto* cmd : cmdBuf_.commands())
		{
			switch (cmd->type())
			{
				CMD_CASE(ClearFrameBuffers)
				CMD_CASE(SwapBuffers);
				CMD_CASE(DrawCall);
			default:
				throw SGE_ERROR("unhandled command");
			}
		}

		#undef CMD_CASE
	}
};

}