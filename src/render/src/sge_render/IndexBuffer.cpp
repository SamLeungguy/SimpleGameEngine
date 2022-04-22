#include "IndexBuffer.h"
#include "Renderer.h"

namespace sge {

IndexBuffer* IndexBuffer::create(CreateDesc& desc_)
{
	return  Renderer::current()->onCreateIndexBuffer(desc_);
}

IndexBuffer::IndexBuffer(CreateDesc& desc_)
{
}

}