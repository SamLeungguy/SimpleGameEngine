#pragma once

#include "EditMesh.h"
#include "../Render_Common.h"
#include <sge_render/buffer/RenderGpuBuffer.h>

namespace sge {

class RenderMesh;

class RenderSubMesh {
public:
	void create(const EditMesh& src_);
	void clear();

	RenderGpuBuffer* getVertexBuffer() const	{ return constCast(_spVertexBuffer); }
	RenderGpuBuffer* getIndexBuffer() const		{ return constCast(_spIndexBuffer); }

	size_t getIndexCount() const		{ return _indexCount; }
	size_t getVertexCount() const { return _vertexCount; }
	RenderDataType getIndexType() const { return _indexType; }

	RenderPrimitiveType getPrimitive() const;
	const VertexLayout* getVertexLayout() const;

	void setIndexBuffer(SPtr<RenderGpuBuffer>& sp_, RenderDataType indexType_) { _spIndexBuffer.reset(sp_); _indexCount = _spIndexBuffer->bufferSize() / RenderDataTypeUtil::getByteSize(indexType_); _indexType = indexType_; }

friend class RenderMesh;
protected:
	RenderMesh* _pMesh = nullptr;
	RenderDataType _indexType = RenderDataType::None;

	SPtr<RenderGpuBuffer>	_spVertexBuffer;
	SPtr<RenderGpuBuffer>	_spIndexBuffer;

	size_t _vertexCount = 0;
	size_t _indexCount = 0;
};

class RenderMesh {
public:
	using SubMesh = RenderSubMesh;
	void create(const EditMesh& src);
	void clear();

	RenderPrimitiveType getPrimitive() const	{ return _primitive; }
	const VertexLayout* getVertexLayout() const { return _pVertexLayout; }

	Span<      SubMesh>	subMeshes()			{ return _subMeshes; }
	Span<const SubMesh>	subMeshes() const	{ return _subMeshes; }

	void setSubMeshCount(size_t newSize);

private:
	RenderPrimitiveType _primitive = RenderPrimitiveType::Triangles;
	const VertexLayout* _pVertexLayout = nullptr;
	Vector_<SubMesh, 1>	_subMeshes;
};

SGE_INLINE RenderPrimitiveType RenderSubMesh::getPrimitive() const {
	return _pMesh->getPrimitive();
}

SGE_INLINE const VertexLayout* RenderSubMesh::getVertexLayout() const {
	return _pMesh->getVertexLayout();
}

}