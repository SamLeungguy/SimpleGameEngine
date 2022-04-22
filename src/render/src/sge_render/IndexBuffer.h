#pragma once

#include "Render_Common.h"

namespace sge {

struct IndexBuffer_CreateDesc
{
	BufferUsage usageFlag = BufferUsage::Default;

	const u32* pData = nullptr;		// shd be u32
	u64 byteSize = 0;
	u64 count = 0;
};

class IndexBuffer : public NonCopyable
{
public:
	using CreateDesc = IndexBuffer_CreateDesc;

	IndexBuffer(CreateDesc& desc_);
	virtual ~IndexBuffer() = default;

	virtual void bind() = 0;
	virtual u64 getCount() = 0;

	static IndexBuffer* create(CreateDesc& desc_);
private:
};

}