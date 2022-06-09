#include "RenderMesh.h"

#include <sge_render/Renderer.h>
#include <sge_render/vertex/VertexLayoutManager.h>

#include <new>

namespace sge {

struct RenderMesh_InternalHelper
{
	template<class Dst, class T> inline
	static void copyVertexData(Dst* pDst_, size_t vertexCount_, const VertexLayout::Element& element_, size_t stride_, const T* pSrc_)
	{
		u8* p = pDst_ + element_.offset;
		for (size_t i = 0; i < vertexCount_; i++)
		{
			*reinterpret_cast<T*>(p) = *pSrc_;
			pSrc_++;
			p += stride_;
		}
	}

	static bool hasAttr(size_t arraySize_, size_t vertexCount_)
	{
		if (arraySize_ <= 0)
			return false;
		if (arraySize_ < vertexCount_)
		{
			SGE_ASSERT(false);
		}
		return true;
	}
};

void RenderSubMesh::clear() {
	_spVertexBuffer = nullptr;
	_spIndexBuffer = nullptr;
	_vertexCount = 0;
	_indexCount = 0;
}

void RenderSubMesh::create(const EditMesh& src_)
{
	using Helper = RenderMesh_InternalHelper;
	clear();

	_vertexCount = src_.positions.size();
	_indexCount = src_.indices.size();

	if (_vertexCount <= 0)
		return;

	auto* pVertexLayout = _pMesh->getVertexLayout();

	//------
	Vector_<u8, 1024>	vertexData;
	vertexData.resize(pVertexLayout->stride * _vertexCount);

	auto* pData = vertexData.data();
	auto stride = pVertexLayout->stride;

	for (auto& element : pVertexLayout->elements)
	{
		using S = Vertex_Semantic;
		using ST = Vertex_SemanticType;
		using U = Vertex_SemanticUtil;

		auto semanticType = U::getType(element.semantic);
		auto semanticIndex = U::getIndex(element.semantic);

		switch (semanticType)
		{
		case ST::TexCoord:
		{
			if (semanticIndex < EditMesh::s_uvCountMax)
			{
				Helper::copyVertexData(pData, _vertexCount, element, stride, src_.uvs[semanticIndex].data());
			}
			continue;
		}break;
		}

		switch (element.semantic)
		{
		case S::Pos:		Helper::copyVertexData(pData, _vertexCount, element, stride, src_.positions.data());	break;
		case S::Color0:		Helper::copyVertexData(pData, _vertexCount, element, stride, src_.colors.data());		break;
		case S::Normal:		Helper::copyVertexData(pData, _vertexCount, element, stride, src_.normals.data());		break;
		case S::Tangent:	Helper::copyVertexData(pData, _vertexCount, element, stride, src_.tangents.data());		break;
		case S::Binormal:	Helper::copyVertexData(pData, _vertexCount, element, stride, src_.binormals.data());	break;
		}
	}

	_indexCount = src_.indices.size();

	//-----
	auto* pRenderer = Renderer::current();
	{
		RenderGpuBuffer::CreateDesc desc;
		desc.type = RenderGpuBufferType::Vertex;
		desc.bufferSize = _vertexCount * pVertexLayout->stride;
		_spVertexBuffer = pRenderer->createGpuBuffer(desc);
		_spVertexBuffer->uploadToGpu(vertexData);
	}

	if (_indexCount > 0) {
		ByteSpan indexData;
		Vector_<u16, 1024> index16Data;

		if (_vertexCount > UINT16_MAX) {
			_indexType = RenderDataType::UInt32;
			indexData = spanCast<const u8, const u32>(src_.indices);
		}
		else {
			_indexType = RenderDataType::UInt16;
			index16Data.resize(src_.indices.size());
			for (size_t i = 0; i < src_.indices.size(); i++) {
				u32 vi = src_.indices[i];
				index16Data[i] = static_cast<u16>(vi);
			}
			indexData = spanCast<const u8, const u16>(index16Data);
		}

		RenderGpuBuffer::CreateDesc desc;
		desc.type = RenderGpuBufferType::Index;
		desc.bufferSize = indexData.size();
		_spIndexBuffer = pRenderer->createGpuBuffer(desc);
		_spIndexBuffer->uploadToGpu(indexData);
	}
}

void RenderMesh::create(const EditMesh& src_)
{
	using Helper = RenderMesh_InternalHelper;
	clear();

	u8 uvCount = 0;
	u8 colorCount = 0;
	u8 normalCount = 0;
	u8 tangentCount = 0;
	u8 binormalCount = 0;

	using Helper = RenderMesh_InternalHelper;

	_primitive = RenderPrimitiveType::Triangles;
	size_t vertexCount = src_.positions.size();

	if (vertexCount <= 0)
		return;

	if (Helper::hasAttr(src_.colors.size(), vertexCount))		colorCount = 1;
	if (Helper::hasAttr(src_.normals.size(), vertexCount))		normalCount = 1;
	if (Helper::hasAttr(src_.tangents.size(), vertexCount))		tangentCount = 1;
	if (Helper::hasAttr(src_.binormals.size(), vertexCount))	binormalCount = 1;

	for (u8 i = 0; i < EditMesh::s_uvCountMax; i++)
	{
		if (Helper::hasAttr(src_.uvs[i].size(), vertexCount))
			uvCount = i + 1;
	}

	auto vertexType = VertexTypeUtil::make(
		RenderDataTypeUtil::get<Tuple3f>(),
		RenderDataTypeUtil::get<Color4b>(), colorCount,
		RenderDataTypeUtil::get<Tuple2f>(), uvCount,
		RenderDataTypeUtil::get<Tuple3f>(), normalCount, tangentCount, binormalCount);

	_pVertexLayout = VertexLayoutManager::current()->getLayout(vertexType);

	if (!_pVertexLayout)
	{
		throw SGE_ERROR("cannot find vertex Layout for mesh");
	}

	setSubMeshCount(1);
	_subMeshes[0].create(src_);
}

void RenderMesh::clear() {
	_pVertexLayout = nullptr;
	_subMeshes.clear();
}

void RenderMesh::setSubMeshCount(size_t newSize) {
	size_t oldSize = _subMeshes.size();
	_subMeshes.resize(newSize);
	for (size_t i = oldSize; i < newSize; i++) {
		_subMeshes[i]._pMesh = this;
	}
}

}