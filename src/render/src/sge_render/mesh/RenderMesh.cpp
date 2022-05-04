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

void RenderMesh::create(const EditMesh& src_)
{
	u8 uvCount = 0;
	u8 colorCount = 0;
	u8 normalCount = 0;
	u8 tangentCount = 0;
	u8 binormalCount = 0;

	using Helper = RenderMesh_InternalHelper;

	_primitive = RenderPrimitiveType::Triangles;
	_vertexCount = src_.positions.size();

	if (_vertexCount <= 0)
		return;

	if (Helper::hasAttr(src_.colors.size(), _vertexCount))
		colorCount = 1;
	if (Helper::hasAttr(src_.normals.size(), _vertexCount))
		normalCount = 1;
	if (Helper::hasAttr(src_.tangents.size(), _vertexCount))
		tangentCount = 1;
	if (Helper::hasAttr(src_.binormals.size(), _vertexCount))
		binormalCount = 1;

	for (u8 i = 0; i < EditMesh::s_uvCountMax; i++)
	{
		if (Helper::hasAttr(src_.uvs[i].size(), _vertexCount))
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

	//------
	Vector_<u8, 1024>	vertexData;
	vertexData.resize(_pVertexLayout->stride * _vertexCount);

	auto* pData = vertexData.data();
	auto stride = _pVertexLayout->stride;

	for (auto& element : _pVertexLayout->elements)
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
		desc.bufferSize = _vertexCount * _pVertexLayout->stride;
		_spVertexBuffer = pRenderer->createGpuBuffer(desc);
		
		_spVertexBuffer->uploadToGpu(vertexData);
	}

	if(_indexCount)
	{
		RenderGpuBuffer::CreateDesc desc;
		desc.type = RenderGpuBufferType::Index;
		desc.bufferSize = _indexCount * sizeof(EditMesh::IndexType);
		_spIndexBuffer = pRenderer->createGpuBuffer(desc);

		Span<const u8> indexData(reinterpret_cast<const u8*>(src_.indices.data()), desc.bufferSize);
		
		_spIndexBuffer->uploadToGpu(indexData);
	}
}

}