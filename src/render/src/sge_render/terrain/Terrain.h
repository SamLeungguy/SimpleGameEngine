#pragma once
#include <sge_render/textures/Texture.h>
#include <sge_render/buffer/RenderGpuBuffer.h>

namespace sge {

class RenderMesh;

struct Terrain_CreateDesc
{
	Vec2i size{0, 0};

	Image heightMap;

	//SPtr<Texture2D> heightMap;
};

class Terrain
{
public:
	using CreateDesc = Terrain_CreateDesc;
	Terrain() = default;
	Terrain(CreateDesc& desc_);

	void create(CreateDesc& desc_);

	SGE_INLINE Vec2i size() const;

		  RenderMesh& getRenderMesh()		{ return _testRenderMesh; }
	const RenderMesh& getRenderMesh() const { return _testRenderMesh; }

private:
	void _create(int width_, int height_) {};
	void _initMesh(CreateDesc& desc_, RenderMesh& outMesh_);

private:
	Vec2i _size{ 0, 0 };

	SPtr<RenderGpuBuffer> _spVertexBuffer;
	Vector_<SPtr<RenderGpuBuffer>, 16> _indexBufferSPtrs;

	RenderMesh _testRenderMesh;
};

#if 0
#pragma mark ===================== Terrain
#endif // 0
#if 1    // ImageInfo

SGE_INLINE Vec2i Terrain::size() const { return _size; }

#endif

}