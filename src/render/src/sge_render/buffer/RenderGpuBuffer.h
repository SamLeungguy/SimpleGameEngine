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

	void uploadToGpu(ByteSpan data_, size_t offset_ = 0) {
		if (data_.size() + offset_ > _desc.bufferSize) {
			throw SGE_ERROR("out of range");
		}
		onUploadToGpu(data_, offset_);
	}

	size_t stride()			{ return _desc.stride; }
	size_t bufferSize()		{ return _desc.bufferSize; }
	size_t elementCount()	{ return _desc.bufferSize / (_desc.stride / 8); }


protected:
	virtual void onUploadToGpu(ByteSpan data_, size_t offset_) = 0;
	CreateDesc _desc;
};

}