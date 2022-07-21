#pragma once
#include <sge_render/textures/Texture.h>
#include <sge_render/buffer/RenderGpuBuffer.h>
#include <sge_render/mesh/RenderMesh.h>

namespace sge {

class RenderMesh;

struct Terrain_CreateDesc
{
	Vec2i size{0, 0};

	Image heightMap;

	//SPtr<Texture2D> heightMap;
};

class Terrain_Patch
{
	u32 _indexOffset = 0;
	u32 _size = 17;
	Vec2f _widthHeightOffset;
	Vec3f _center;
};

class Terrain
{
public:
	static constexpr int kMaxLOD = 5;
	static constexpr int kMaxCombination = 16;

	using CreateDesc = Terrain_CreateDesc;
	using IndexChunks = Vector_<Vector_<SPtr<RenderGpuBuffer>, kMaxCombination>, Terrain::kMaxLOD>;
	using IndexType = u32;

	Terrain() = default;
	Terrain(CreateDesc& desc_);

	void create(CreateDesc& desc_);

	SGE_INLINE Vec2i size() const;

		  RenderMesh& getRenderMesh()		{ return _testRenderMesh; }
	const RenderMesh& getRenderMesh() const { return _testRenderMesh; }

		  RenderMesh& getRenderMesh2()		 { return _testRenderMesh2; }
	const RenderMesh& getRenderMesh2() const { return _testRenderMesh2; }

private:
	void _create(int width_, int height_) {};
	void _initMesh(const CreateDesc& desc_, RenderMesh& outMesh_, int lod_, Vec2i offset_);

	//void moveVertices()

private:
	Vec2i _size{ 0, 0 };

	SPtr<RenderGpuBuffer> _spVertexBuffer;
	Vector_<SPtr<RenderGpuBuffer>, 16> _indexBufferSPtrs;

	RenderMesh _testRenderMesh;
	RenderMesh _testRenderMesh2;

	IndexChunks _indexChunks;
};

#if 0
#pragma mark ===================== Terrain
#endif // 0
#if 1    // ImageInfo

SGE_INLINE Vec2i Terrain::size() const { return _size; }

#endif

}