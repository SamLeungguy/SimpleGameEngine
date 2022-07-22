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

	void uploadVertices(ByteSpan data_, size_t offset_ = 0) { _spVertexBuffer->uploadToGpu(data_); }

	void setVertexBuffer(SPtr<RenderGpuBuffer>& sp_) { _spVertexBuffer.reset(sp_); _vertexCount = _spVertexBuffer->elementCount(); }
	void setIndexBuffer(SPtr<RenderGpuBuffer>& sp_)  
	{ 
		_spIndexBuffer.reset(sp_);   
#if 1
		_indexCount = _spIndexBuffer->elementCount(); 
		if (_spIndexBuffer->stride() == 16)			_indexType = RenderDataType::UInt16;
		else if (_spIndexBuffer->stride() == 32)	_indexType = RenderDataType::UInt32;
		else throw SGE_ERROR("unsupport index type");
#else
		_indexCount = _spIndexBuffer->bufferSize() / RenderDataTypeUtil::getByteSize(indexType_); _indexType = indexType_; 
#endif // 0

	}

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