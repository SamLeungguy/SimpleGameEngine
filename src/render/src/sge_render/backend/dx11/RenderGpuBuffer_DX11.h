#pragma once

#include "Render_Common_DX11.h"
#include <sge_render/buffer/RenderGpuBuffer.h>

namespace sge {

class RenderGpuBuffer_DX11 : public RenderGpuBuffer {
	using Base = RenderGpuBuffer;
	using Util = DX11Util;
public:
	RenderGpuBuffer_DX11(CreateDesc& desc);
	~RenderGpuBuffer_DX11() = default;

	virtual void onUploadToGpu(Span<const u8> data_, size_t offset_) override;

	DX11_ID3DBuffer* getBuffer() { return _cpBuffer; }
private:
	ComPtr<DX11_ID3DBuffer> _cpBuffer;
};

}