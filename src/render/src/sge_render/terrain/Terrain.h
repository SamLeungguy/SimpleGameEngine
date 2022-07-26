#pragma once
#include <sge_render/textures/Texture.h>
#include <sge_render/buffer/RenderGpuBuffer.h>
#include <sge_render/mesh/RenderMesh.h>

#include <sge_core/math/MathCamera3.h>

namespace sge {

class RenderMesh;

struct Terrain_CreateDesc
{
	Vec2i size{0, 0};
	Vec2i patchCount{1, 1};

	Image heightMap;

	//SPtr<Texture2D> heightMap;
};

class _Terrain_Patch : public RefCountBase
{
public:
	virtual ~_Terrain_Patch() = default;

	u8 tblr = 0b0000;
	int lod = 0;

	Vec2i offset;
	Mat4f modelMatrix;

	//RenderMesh* pMesh = nullptr;

	SPtr<RenderGpuBuffer> spVertexBuffer;
	SPtr<RenderGpuBuffer> spIndexBuffer;
};

class Terrain : public NonCopyable
{
public:
	static constexpr int kMaxLOD = 10;
	static constexpr int kMaxCombination = 16;

	using CreateDesc = Terrain_CreateDesc;
	using Patch = _Terrain_Patch;
	using IndexChunks = Vector_<Vector_<SPtr<RenderGpuBuffer>, kMaxCombination>, Terrain::kMaxLOD>;
	using IndexType = u32;

	Terrain() = default;
	Terrain(CreateDesc& desc_);

	void create(CreateDesc& desc_);

	void update(const Math::Camera3f& camera_);

	Vec2i size() const;
	int patchIndex(Vec2i iPatch_) const;

	SPtr<Patch>& getPatch(Vec2i iPatch_);
	SPtr<Patch>& getPatch_unsafe(Vec2i iPatch_);
	SPtr<Patch>& getPatch_clamp(Vec2i iPatch_);
	
	bool checkBoundary(Vec2i iPatch_) const;
	void clampBoundary(Vec2i& iPatch_) const;

	Span<      SPtr<Patch>>	getPatches()		{ return _patches; }
	Span<const SPtr<Patch>>	getPatches() const	{ return _patches; }

		  RenderMesh& getRenderMesh()		{ return _testRenderMesh; }
	const RenderMesh& getRenderMesh() const { return _testRenderMesh; }

	Vec2i getPatchCoord(const Math::Camera3f& camera_);

private:
	void _create(int width_, int height_) {};
	void _init(const CreateDesc& desc_, RenderMesh& outMesh_, int lod_, Vec2i offset_);
	void _initMesh(EditMesh& editMesh_, Vec2i iPatch_);

	void _init();

	void _updatePatchLOD(Vec2i startPatch_);

	void _error(StrView msg_);

private:
	Vec2i _size{ 0, 0 };
	Vec2i _patchCount;
	Vec2i _patchSize;
	int _maxLodIndex = 0;

	Vector_<SPtr<RenderGpuBuffer>, 16> _indexBufferSPtrs;

	RenderMesh _testRenderMesh;

	Vector<SPtr<Patch>> _patches;

	IndexChunks _indexChunks;
	int _squareToTriangleRatio = 2;

	float _normalizeFactor = 1.0f / std::numeric_limits<u16>::max();
	float _heightOffset = 400.0f;
	float _heightFactor = 800.0f;

	Image _heightMapImage;
};

#if 0
#pragma mark ===================== Terrain
#endif // 0
#if 1    // ImageInfo

SGE_INLINE Vec2i Terrain::size() const							{ return _size; }
SGE_INLINE int Terrain::patchIndex(Vec2i iPatch_) const			{ return _patchCount.x * iPatch_.y + iPatch_.x; }

SGE_INLINE SPtr<Terrain::Patch>& Terrain::getPatch(Vec2i iPatch_)			{ if (!checkBoundary(iPatch_)) _error("out of boundary"); return getPatch_unsafe(iPatch_); }
SGE_INLINE SPtr<Terrain::Patch>& Terrain::getPatch_unsafe(Vec2i iPatch_) 	{ return _patches[patchIndex(iPatch_)]; }
SGE_INLINE SPtr<Terrain::Patch>& Terrain::getPatch_clamp(Vec2i iPatch_) { clampBoundary(iPatch_); return _patches[patchIndex(iPatch_)]; }

SGE_INLINE bool Terrain::checkBoundary(Vec2i iPatch_) const		{ return iPatch_.x >= 0 && iPatch_.x < _patchCount.x&& iPatch_.y >= 0 && iPatch_.y < _patchCount.y; }
SGE_INLINE void Terrain::clampBoundary(Vec2i& iPatch_) const	{ iPatch_.x = Math::clamp(iPatch_.x, 0, _patchCount.x - 1); iPatch_.y = Math::clamp(iPatch_.y, 0, _patchCount.y - 1); }


SGE_INLINE void Terrain::_error(StrView msg_) { throw SGE_ERROR("{}", msg_); }

#endif

}