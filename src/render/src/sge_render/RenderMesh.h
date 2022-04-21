#pragma once
#include "VertexBuffer.h"
#include "VertexLayout.h"
#include "IndexBuffer.h"

namespace sge {

class RenderMesh
{
public:
	RenderMesh() = default;
	~RenderMesh() = default;

	VertexLayout* pVertexLayout = nullptr;
	UPtr<VertexBuffer>	upVertexBuffer;
	UPtr<IndexBuffer>	upIndexBuffer;
};

}