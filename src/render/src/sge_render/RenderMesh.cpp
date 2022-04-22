#include "RenderMesh.h"

#include <new>

namespace sge {
RenderMesh::~RenderMesh()
{
}
RenderMesh* RenderMesh::create(const Mesh& mesh_)
{
	VertexLayout vertexLayout;
	vertexLayout.elements.reserve(static_cast<int>(VertexSematic::Count));
	VertexLayout::Element element;

	SGE_ASSERT(mesh_._positions.size() > 0);

	// TODO: remove temp
	Vector<const u8*> vertexArrayDataPtrs;
	vertexArrayDataPtrs.reserve(static_cast<int>(VertexSematic::Count));

	if (mesh_._positions.size() > 0)
	{
		element.sematic = VertexSematic::Pos;
		element.type = RenderDataType::Float32x3;

		vertexLayout.push_back(element);

		vertexArrayDataPtrs.push_back(reinterpret_cast<const u8*>(mesh_._positions.data()));
	}
	
	if (mesh_._uvs[0].size() > 0)
	{
		element.sematic = VertexSematic::Uv0;
		element.type = RenderDataType::Float32x2;

		vertexLayout.push_back(element);

		vertexArrayDataPtrs.push_back(reinterpret_cast<const u8*>(mesh_._uvs[0].data()));
	}

	if (mesh_._uvs[1].size() > 0)
	{
		SGE_ASSERT(0, "not yet handle!");

		element.sematic = VertexSematic::Uv1;
		element.type = RenderDataType::Float32x2;

		vertexLayout.push_back(element);

		vertexArrayDataPtrs.push_back(reinterpret_cast<const u8*>(mesh_._uvs[1].data()));
	}

	if (mesh_._colors.size() > 0)
	{
		element.sematic = VertexSematic::Color;
		element.type = RenderDataType::Float32x4;

		vertexLayout.push_back(element);

		vertexArrayDataPtrs.push_back(reinterpret_cast<const u8*>(mesh_._colors.data()));
	}

	// determine final vertex layout 
	{
		Vertex_PosColor final_layout;
	}

	// create vertex data as vertex layout
	// process to mesh to vertex layout like vector
	u64 layoutSize = vertexLayout.getTotalBytes();
	u64 vertexCount = mesh_._positions.size();
	u64 totalByteSize = layoutSize * vertexCount;

	/*SGE_ASSERT(mesh_._positions.size() == mesh_._uvs[0].size()
		&& mesh_._positions.size() == mesh_._colors.size());*/

	if (mesh_._colors.size())
	{
		SGE_ASSERT(mesh_._positions.size() == mesh_._colors.size());
	}
	if (mesh_._uvs[0].size())
	{
		SGE_ASSERT(mesh_._positions.size() == mesh_._uvs[0].size());
	}

	Vector<u8> verticesData;

	verticesData.resize(totalByteSize);
	memset(verticesData.data(), 0, totalByteSize);

	u64 vertexOffset = 0;
	for (size_t i = 0; i < vertexCount; i++)
	{
		//void* p = reinterpret_cast<void*>(vertexData.data() + vertexOffset);
		u8* p = verticesData.data() + vertexOffset;

		for (size_t i = 0; i < vertexLayout.getCount(); i++)
		{
			auto& element = vertexLayout.elements[i];
			//SGE_LOG("pMeshVertexArrayData: {}", vertexArrayDataPtrs[i] + element.size);

			memcpy(p + element.offset, vertexArrayDataPtrs[i], element.size);			// samm cpy size should use for loop

			vertexArrayDataPtrs[i] += element.size;
		}

		vertexOffset += layoutSize;
	}

	// set up output Render Mesh
	{
		// create Render Mesh
		auto* pRenderMesh = new RenderMesh();
		pRenderMesh->vertexLayout = vertexLayout;

		// create vertex buffer
		UPtr<VertexBuffer> upVertexBuffer;
		VertexBuffer_CreateDesc v_cd;
		v_cd.usageFlag = BufferUsage::Static;
		v_cd.vertexCount = vertexCount;
		v_cd.byteSize = verticesData.size();
		v_cd.pData = verticesData.data();
		v_cd.pVertexLayout = &pRenderMesh->vertexLayout;
		upVertexBuffer.reset(VertexBuffer::create(v_cd));

		SGE_ASSERT(mesh_._indices.size() > 0, "");

		// create index buffer
		UPtr<IndexBuffer> upIndexBuffer;
		IndexBuffer_CreateDesc i_cd;
		i_cd.usageFlag = BufferUsage::Default;
		i_cd.pData = mesh_._indices.data();
		i_cd.count = mesh_._indices.size();
		i_cd.byteSize = sizeof(u32) * mesh_._indices.size();
		upIndexBuffer.reset(IndexBuffer::create(i_cd));

		pRenderMesh->upVertexBuffer = eastl::move(upVertexBuffer);
		pRenderMesh->upIndexBuffer = eastl::move(upIndexBuffer);

		return pRenderMesh;
	}
}


}