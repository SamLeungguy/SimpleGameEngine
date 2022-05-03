#pragma once

#include "EditMesh.h"
#include "../Render_Common.h"
#include <sge_render/buffer/RenderGpuBuffer.h>

namespace sge {

class RenderMesh {
public:
	void create(const EditMesh& src_);

	RenderPrimitiveType getPrimitive() const { return _primitive; }

	size_t getVertexCount() const { return _vertexCount; }
	RenderGpuBuffer* getVertexBuffer() { return _spVertexBuffer; }
	const VertexLayout* getVertexLayout() const { return _pVertexLayout; }

private:

	RenderPrimitiveType _primitive = RenderPrimitiveType::None;

	size_t _vertexCount = 0;
	const VertexLayout* _pVertexLayout = nullptr;
	SPtr<RenderGpuBuffer>	_spVertexBuffer;
};

}