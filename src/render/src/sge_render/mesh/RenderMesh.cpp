#include "RenderMesh.h"
#include <sge_render/Renderer.h>
#include <sge_render/vertex/VertexLayoutManager.h>

namespace sge {

struct RenderMesh_InternalHelper {
	template<class DST, class T> inline
		static void copyVertexData(DST* dst
			, size_t vertexCount
			, const VertexLayout::Element& element
			, size_t stride
			, const T* src)
	{
		u8* p = dst + element.offset;

		for (size_t i = 0; i < vertexCount; i++) {
			*reinterpret_cast<T*>(p) = *src;
			src++;
			p += stride;
		}
	}

	static bool hasAttr(size_t arraySize, size_t vertexCount) {
		if (arraySize <= 0) return false;
		if (arraySize < vertexCount) {
			SGE_ASSERT(false);
			return false;
		}
		return true;
	}
};

void RenderMesh::create(const EditMesh& src) {
	using Helper = RenderMesh_InternalHelper;
	clear();

	u8 uvCount = 0;
	u8 colorCount = 0;
	u8 normalCount = 0;
	u8 tangentCount = 0;
	u8 binormalCount = 0;

	_primitive = RenderPrimitiveType::Triangles;
	size_t vertexCount = src.positions.size();
	if (vertexCount <= 0)
		return;

	if (Helper::hasAttr(src.colors.size(), vertexCount)) colorCount = 1;
	if (Helper::hasAttr(src.normals.size(), vertexCount)) normalCount = 1;
	if (Helper::hasAttr(src.tangents.size(), vertexCount)) tangentCount = 1;
	if (Helper::hasAttr(src.binormals.size(), vertexCount)) binormalCount = 1;

	for (u8 i = 0; i < EditMesh::s_uvCountMax; i++) {
		if (Helper::hasAttr(src.uvs[i].size(), vertexCount)) uvCount = i + 1;
	}

	auto vertexType = VertexTypeUtil::make(
		RenderDataTypeUtil::get<Tuple3f>(),
		RenderDataTypeUtil::get<Color4b>(), colorCount,
		RenderDataTypeUtil::get<Tuple2f>(), uvCount,
		RenderDataTypeUtil::get<Tuple3f>(), normalCount, tangentCount, binormalCount);

	_pVertexLayout = VertexLayoutManager::instance()->getLayout(vertexType);

	if (!_pVertexLayout) {
		throw SGE_ERROR("cannot find vertex Layout for mesh");
	}

	setSubMeshCount(1);
	_subMeshes[0].create(src);
}

void RenderMesh::clear() {
	_pVertexLayout = nullptr;
	_subMeshes.clear();
}

void RenderSubMesh::create(const EditMesh& src) {
	using Helper = RenderMesh_InternalHelper;

	clear();

	_vertexCount = src.positions.size();
	_indexCount = src.indices.size();

	if (_vertexCount <= 0)
		return;

	auto* pVertexLayout = _pMesh->getVertexLayout();

	Vector_<u8, 1024>	vertexData;
	vertexData.resize(pVertexLayout->stride * _vertexCount);

	auto* pData = vertexData.data();
	auto stride = pVertexLayout->stride;
	auto vc = _vertexCount;

	for (auto& e : pVertexLayout->elements) {
		using S = VertexSemantic;
		using ST = VertexSemanticType;
		using U = VertexSemanticUtil;

		auto semanticType = U::getType(e.semantic);
		auto semanticIndex = U::getIndex(e.semantic);

		switch (semanticType) {
		case ST::TEXCOORD: {
			if (semanticIndex < EditMesh::s_uvCountMax) {
				Helper::copyVertexData(pData, vc, e, stride, src.uvs[semanticIndex].data()); break;
			}
			continue;
		} break;
		}

		switch (e.semantic) {
		case S::POSITION:	Helper::copyVertexData(pData, vc, e, stride, src.positions.data());   break;
		case S::COLOR0:		Helper::copyVertexData(pData, vc, e, stride, src.colors.data()); break;
		case S::NORMAL:		Helper::copyVertexData(pData, vc, e, stride, src.normals.data()); break;
		case S::TANGENT:	Helper::copyVertexData(pData, vc, e, stride, src.tangents.data()); break;
		case S::BINORMAL:	Helper::copyVertexData(pData, vc, e, stride, src.binormals.data()); break;
		}
	}

	//------
	auto* renderer = Renderer::instance();
	{
		RenderGpuBuffer::CreateDesc desc;
		desc.type = RenderGpuBufferType::Vertex;
		desc.bufferSize = _vertexCount * pVertexLayout->stride;
		_spVertexBuffer = renderer->createGpuBuffer(desc);
		_spVertexBuffer->uploadToGpu(vertexData);
	}

	if (_indexCount > 0) {
		ByteSpan indexData;
		Vector_<u16, 1024> index16Data;

		if (_vertexCount > UINT16_MAX) {
			_indexType = RenderDataType::UInt32;
			indexData = spanCast<const u8, const u32>(src.indices);
		}
		else {
			_indexType = RenderDataType::UInt16;
			index16Data.resize(src.indices.size());
			for (size_t i = 0; i < src.indices.size(); i++) {
				u32 vi = src.indices[i];
				index16Data[i] = static_cast<u16>(vi);
			}
			indexData = spanCast<const u8, const u16>(index16Data);
		}

		RenderGpuBuffer::CreateDesc desc;
		desc.type = RenderGpuBufferType::Index;
		desc.bufferSize = indexData.size();
		_spIndexBuffer = renderer->createGpuBuffer(desc);
		_spIndexBuffer->uploadToGpu(indexData);
	}
}

void RenderSubMesh::clear() {
	_spVertexBuffer = nullptr;
	_spIndexBuffer = nullptr;
	_vertexCount = 0;
	_indexCount = 0;
}

void RenderMesh::setSubMeshCount(size_t newSize) {
	size_t oldSize = _subMeshes.size();
	_subMeshes.resize(newSize);
	for (size_t i = oldSize; i < newSize; i++) {
		_subMeshes[i]._pMesh = this;
	}
}

}