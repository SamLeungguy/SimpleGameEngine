#pragma once

#if SGE_RENDER_HAS_DX11
#include "Render_Common_DX11.h"
#include <sge_render/Renderer.h>
#include "../../VertexBuffer.h"

namespace sge {

class VertexBuffer_DX11 : public VertexBuffer
{
	using Base = VertexBuffer;
	using Util = DX11Util;
public:
	VertexBuffer_DX11(CreateDesc& desc_);
	virtual ~VertexBuffer_DX11();

	virtual void bind() override;
	virtual u64 getCount() override;


protected:
	ComPtr<DX11_ID3DBuffer>				_cpVertexBuffer;
	VertexLayout*						_pVertexLayout = nullptr;
	u64 _vertexCount = 0;
};

}

#endif