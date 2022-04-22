#pragma once

#include "Render_Common.h"
#include "RenderCommand_Base.h"

namespace sge {

//template<typename T>
//static T* castRenderCommand(RenderCommand_Base* pBase_);

class RenderCommand_Draw : public RenderCommand_Base
{
	using Base = RenderCommand_Base;
public:
	RenderCommand_Draw()
		:
		Base(RenderCommnadType::Draw)
	{
	}
	~RenderCommand_Draw() = default;

	static RenderCommnadType getStaticType() { return RenderCommnadType::Draw; }

	PrimitiveType primitiveType = PrimitiveType::Triangle;

	Shader*			pShader = nullptr;

	VertexLayout*	pVertexLayout = nullptr;
	VertexBuffer*	pVertexBuffer = nullptr;
	IndexBuffer*	pIndexBuffer = nullptr;

	u32 vertexCount = 0;
	u32 IndexCount = 0;
};

class RenderCommand_SetViewport : public RenderCommand_Base
{
	using Base = RenderCommand_Base;
public:
	RenderCommand_SetViewport()
		:
		Base(RenderCommnadType::SetViewport)
	{
	}
	~RenderCommand_SetViewport() = default;

	static RenderCommnadType getStaticType() { return RenderCommnadType::SetViewport; }

	Rect2i rect;

	//int x, y = 0;
	//u32 width, height = 0;
};

//auto* castRenderCommand(RenderCommand_Base* pBase_)
//{
//	switch (pBase_->type)
//	{
//		case RenderCommnadType::Draw:			return static_cast<RenderCommand_Draw*>(pBase_);
//		case RenderCommnadType::SetViewport:	return static_cast<RenderCommand_SetViewport*>(pBase_);
//
//	default: SGE_ERROR("invalid render commnad type!");
//	}
//}



}