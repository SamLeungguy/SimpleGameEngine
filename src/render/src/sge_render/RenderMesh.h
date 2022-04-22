#pragma once

#include "Render_Common.h"

#include "VertexLayout.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace sge {

class Mesh
{
	friend class RenderMesh;
public:
	static constexpr int s_kMaxUv = 8;

	void setPositions(const Vector<Tuple3f>& positions_)
	{
		_positions = positions_;
	}

	void setUVs(const Vector<Tuple2f>& uv_, int index_ = 0)
	{
		SGE_ASSERT(index_ < s_kMaxUv);
		_uvs[index_] = uv_;
	}

	void setColors(const Vector<Color4f>& colors_)
	{
		_colors = colors_;
	}

	void setIndices(const Vector<u32>& indices_)
	{
		SGE_ASSERT(indices_.size() % 3 == 0, "not triangle type indices!");
		_indices = indices_;
	}

private:
	Vector<Tuple3f> _positions;
	Vector<Tuple2f> _uvs[s_kMaxUv];
	Vector<Color4f> _colors;
	Vector<u32>		_indices;
};

class RenderMesh
{
public:
	RenderMesh() = default;
	~RenderMesh();

	static RenderMesh* create(const Mesh& mesh_);

	//VertexLayout* pVertexLayout = nullptr;
	VertexLayout vertexLayout;

	UPtr<VertexBuffer>	upVertexBuffer;
	UPtr<IndexBuffer>	upIndexBuffer;
};

}