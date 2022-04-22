#include "VertexBuffer.h"
#include "Renderer.h"

namespace sge {

VertexBuffer* VertexBuffer::create(CreateDesc& desc_)
{
	return  Renderer::current()->onCreateVertexBuffer(desc_);
}


VertexBuffer::VertexBuffer(CreateDesc& desc_)
{
}

}