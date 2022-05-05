#if SGE_RENDER_HAS_GL

#include "RenderGpuBuffer_gl.h"

namespace sge {

RenderGpuBuffer_gl::RenderGpuBuffer_gl(CreateDesc desc_)
	:
	Base(desc_)
{
	if (desc_.bufferSize <= 0)
		throw SGE_ERROR("buffer size = 0");
	if (desc_.stride <= 0)
		throw SGE_ERROR("stride == 0");

	glDebugCall(glCreateBuffers(1, &_rendererID));

	GLenum bufferType = 0;
	switch (_desc.type)
	{
		case Type::Vertex:
		{
			bufferType = GL_ARRAY_BUFFER;
			glDebugCall(glBindBuffer(bufferType, _rendererID));
			glDebugCall(glBufferData(bufferType, desc_.bufferSize, nullptr, GL_DYNAMIC_DRAW));		// GL_STATIC_DRAW, GL_DYNAMIC_DRAW
		} break;

		case Type::Index:
		{
			bufferType = GL_ELEMENT_ARRAY_BUFFER;
			glDebugCall(glBindBuffer(bufferType, _rendererID));
			glDebugCall(glBufferData(bufferType, desc_.bufferSize, nullptr, GL_DYNAMIC_DRAW));		// GL_STATIC_DRAW, GL_DYNAMIC_DRAW
		} break;

		case Type::Const:
		{
			bufferType = GL_UNIFORM_BUFFER;
			// define the range of the buffer that links to a uniform binding point
			glDebugCall(glNamedBufferData(_rendererID, desc_.bufferSize, nullptr, GL_DYNAMIC_DRAW));
			glDebugCall(glBindBufferRange(bufferType, 0 /*binding*/, _rendererID, 0, desc_.bufferSize));
			SGE_ASSERT(0, "not yet!");
		} break;
		default: throw SGE_ERROR("invalid buffer type!");
	}
}

RenderGpuBuffer_gl::~RenderGpuBuffer_gl()
{
	if (_rendererID)
	{
		SGE_ASSERT(_rendererID, "RenderGpuBuffer_gl delete Failed, no valid _rendererID");
		glDebugCall(glDeleteBuffers(1, &_rendererID));
		_rendererID = 0;
	}
}

void RenderGpuBuffer_gl::onUploadToGpu(Span<const u8> data_, size_t offset_)
{
	GLenum bufferType = 0;
	switch (_desc.type)
	{
		case Type::Vertex:
		{
			bufferType = GL_ARRAY_BUFFER;
			glDebugCall(glBindBuffer(bufferType, _rendererID));
			//glDebugCall(glBufferData(bufferType, data_.size(), reinterpret_cast<const void*>(data_.data()), GL_DYNAMIC_DRAW));		// GL_STATIC_DRAW, GL_DYNAMIC_DRAW

			glDebugCall(glBufferSubData(bufferType, offset_, data_.size(), reinterpret_cast<const void*>(data_.data())));
		} break;

		case Type::Index:
		{
			bufferType = GL_ELEMENT_ARRAY_BUFFER;
			glDebugCall(glBindBuffer(bufferType, _rendererID));
			//glDebugCall(glBufferData(bufferType, data_.size(), reinterpret_cast<const void*>(data_.data()), GL_DYNAMIC_DRAW));		// GL_STATIC_DRAW, GL_DYNAMIC_DRAW

			glDebugCall(glBufferSubData(bufferType, offset_, data_.size(), reinterpret_cast<const void*>(data_.data())));
		} break;

		case Type::Const:
		{
			bufferType = GL_UNIFORM_BUFFER;
			//glDebugCall(glBindBufferRange(bufferType, 0 /*binding*/, _rendererID, offset_, data_.size()));
			glDebugCall(glNamedBufferSubData(_rendererID, offset_, data_.size(), reinterpret_cast<const void*>(data_.data())));
			SGE_ASSERT(0, "not yet!");
		} break;

		default: throw SGE_ERROR("invalid buffer type!");
	}
}

}

#endif