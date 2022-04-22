#pragma once

#include "Render_Common.h"
#include "VertexLayout.h"

namespace sge {

struct VertexBuffer_CreateDesc
{
	BufferUsage usageFlag = BufferUsage::Static;

	void* pData = nullptr;
	u64 byteSize = 0;
	u64 vertexCount = 0;
	VertexLayout* pVertexLayout	 = nullptr;
};

class VertexBuffer : public NonCopyable
{
public:
	using CreateDesc = VertexBuffer_CreateDesc;

	VertexBuffer(CreateDesc& desc_);
	virtual ~VertexBuffer() = default;

	virtual void bind() = 0;
	virtual u64 getCount() = 0;

	static VertexBuffer* create(CreateDesc& desc_);
private:
};

}