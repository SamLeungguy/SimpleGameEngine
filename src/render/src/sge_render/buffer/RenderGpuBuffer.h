#pragma once

namespace sge {

enum class RenderGpuBufferType {
	None,
	Vertex,
	Index,
	Const,
};

struct RenderGpuBuffer_CreateDesc {
	using Type = RenderGpuBufferType;

	Type	type = Type::None;
	size_t	bufferSize = 0;
	size_t	stride = 16;
};

class RenderGpuBuffer : public Object
{
public:
	using CreateDesc = RenderGpuBuffer_CreateDesc;
	using Type = RenderGpuBufferType;

	RenderGpuBuffer(CreateDesc& desc_);
	virtual ~RenderGpuBuffer() = default;

	void uploadToGpu(Span<const u8> data_, size_t offset_ = 0) {
		if (data_.size() + offset_ > _desc.bufferSize) {
			throw SGE_ERROR("out of range");
		}
		SGE_ASSERT(data_.size() == _desc.bufferSize);

		onUploadToGpu(data_, offset_);
	}

protected:
	virtual void onUploadToGpu(Span<const u8> data_, size_t offset_) = 0;
	CreateDesc _desc;
};

}