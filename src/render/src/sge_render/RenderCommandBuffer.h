#pragma once

#include <sge_core/memory/LinearAllocator.h>

namespace sge {

class RenderCommandBuffer : public NonCopyable
{
	friend class Renderer;
public:
	RenderCommandBuffer();
	virtual ~RenderCommandBuffer();

	void execute();

	static void drawIndex(u32 indexCount_);
	static void setViewport(const Rect2i& rect_);


protected:
	virtual void onExecute() {};

protected:

	LinearAllocator _linearAllocator;
};

}