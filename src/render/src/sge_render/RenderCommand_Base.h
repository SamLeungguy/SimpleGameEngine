#pragma once

#include "Render_Common.h"

#include "RenderMesh.h"
#include "Shader.h"

namespace sge {

enum class RenderCommnadType
{
	None = 0,
	Draw,
	SetViewport,
	SetClearColor,
	ClearBuffer,
};

class RenderCommand_Base
{
public:
	RenderCommand_Base(RenderCommnadType type_);
	/*virtual*/ ~RenderCommand_Base() = default;

	/*template<typename T, typename Func>
	static bool dispatch(T* pCmd_, Func func_)
	{
		switch (pCmd_->type)
		{
			case: RenderCommnadType::Draw:			{ func_(T); return true; }break;
			case: RenderCommnadType::SetViewport:	{ func_(T); return true; }break;
			case: RenderCommnadType::SetClearColor: { func_(T); return true; }break;
			case: RenderCommnadType::ClearBuffer:	{ func_(T); return true; }break;
		default:
			SGE_ASSERT(0);
			return false;
		}
		return false;
	}*/

	RenderCommnadType type = RenderCommnadType::None;
};

class RenderCommandDispatcher
{
	template<typename T>
	using ExecuteCmdFunc = u64(*)(T*);

public:
	RenderCommandDispatcher() = default;
	RenderCommandDispatcher(RenderCommand_Base* pBase_)
		:
		_pBase(pBase_)
	{
	}

	void set(RenderCommand_Base* pBase_) { _pBase = pBase_; }

	template<typename T, typename Func>
	u64 dispatch(const Func func_)
	{
		if (_pBase->type == T::getStaticType())
		{
			cmdSize = func_(static_cast<T*>(_pBase));
			return cmdSize;
		}
		return 0;
	}

	u64 cmdSize = 0;
private:
	RenderCommand_Base* _pBase = nullptr;
};
}