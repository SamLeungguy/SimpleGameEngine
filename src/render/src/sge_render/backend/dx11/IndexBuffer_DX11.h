#pragma once

#if SGE_RENDER_HAS_DX11
#include "Render_Common_DX11.h"
#include <sge_render/Renderer.h>
#include "../../IndexBuffer.h"

namespace sge {

class IndexBuffer_DX11 : public IndexBuffer
{
	using Base = IndexBuffer;
	using Util = DX11Util;
public:
	IndexBuffer_DX11(CreateDesc& desc_);
	virtual ~IndexBuffer_DX11();

	virtual void bind() override;

	virtual u64 getCount() override;

protected:
	ComPtr<DX11_ID3DBuffer>				_cpIndexBuffer;

	u64 _count = 0;
};

inline u64 IndexBuffer_DX11::getCount() { return _count; }


}

#endif