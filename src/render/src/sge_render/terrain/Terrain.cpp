#include "sge_render-pch.h"
#include "Terrain.h"

#include "sge_render/Renderer.h"

#include "sge_render/vertex/VertexLayoutManager.h"
#include <sge_render/mesh/RenderMesh.h>

namespace sge {

struct TerrainHelper
{
	TerrainHelper(Terrain& v_) : pTerrain(&v_) {}

	template<typename FUNC>
	static void loopWidthHeight(Vec2i size_, FUNC func_)
	{
		for (int j = 0; j < size_.y; j++)
		{
			for (int i = 0; i < size_.x; i++)
			{
				func_(i, j);
			}
		}
	}


private:
	Terrain* pTerrain = nullptr;
};

Terrain::Terrain(CreateDesc& desc_)
{
	//desc_.heightMap.<Color4b>();

	_size = desc_.heightMap.size();

}

void Terrain::create(CreateDesc& desc_)
{
	auto mapSize = desc_.heightMap.size();
	_size = mapSize - 1;

	_initMesh(desc_, _testRenderMesh);
}

void Terrain::_initMesh(CreateDesc& desc_, RenderMesh& outMesh_)
{
	auto vertexCount = _size.x * _size.y;
	auto triangleCount = _size.x * _size.y;
	Vec2f size_f{ _size.x, _size.y };

	EditMesh editMesh;
	editMesh.positions.reserve(vertexCount);
	editMesh.uvs[0].reserve(vertexCount);
	editMesh.colors.resize(vertexCount);						// TODO: remove
	editMesh.normals.resize(vertexCount);						// TODO: remove
	editMesh.indices.reserve(triangleCount * 3);				// TODO: remove

	float normalizeFactor = 1.0f / std::numeric_limits<u16>::max(), yOffset = 400.0f;
	float heightFactor = 800.0f;

	for (int j = 0; j < _size.y; j++)
	{
		auto* row = desc_.heightMap.row<ColorLs>(j).data();
		for (int i = 0; i < _size.x; i++)
		{
			auto height = row->r * normalizeFactor * heightFactor - yOffset;
			//height = -1;
			editMesh.positions.emplace_back(-size_f.x / 2.0f + i, height, -size_f.y / 2.0f + j);
			row++;
		}
	}
	for (int j = 0; j < _size.y; j++)
	{
		for (int i = 0; i < _size.x; i++)
		{
			editMesh.uvs[0].emplace_back(i / size_f.x, j / size_f.y);
		}
	}
	for (int j = 0; j < _size.y - 1; j++)
	{
		for (int i = 0; i < _size.x - 1; i++)
		{
			editMesh.indices.emplace_back( j	  * _size.x + i + 0);
			editMesh.indices.emplace_back((j + 1) * _size.x + i + 0);
			editMesh.indices.emplace_back( j	  * _size.x + i + 1);

			editMesh.indices.emplace_back( j	  * _size.x + i + 1);
			editMesh.indices.emplace_back((j + 1) * _size.x + i + 0);
			editMesh.indices.emplace_back((j + 1) * _size.x + i + 1);
		}
	}

	outMesh_.create(editMesh);
}

}