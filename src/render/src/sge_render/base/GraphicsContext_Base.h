#pragma once

#include "../sge_render-base.h"

namespace sge {

static constexpr int s_temp_width = 800;
static constexpr int s_temp_height = 600;

class GraphicsContext_Base : public NonCopyable
{
public:
	GraphicsContext_Base() = default;
	virtual ~GraphicsContext_Base() = default;

	virtual void init(void* hWnd_) = 0;
	virtual void destroy() = 0;

	virtual void swapBuffers() = 0;
	
	//virtual void makeContextCurrent() = 0;
	//inline virtual void* getHandle() const = 0;
	//virtual void enableVsync(bool isVsync_) = 0;

private:

};

}