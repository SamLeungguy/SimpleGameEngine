#include "sge_render-pch.h"
#include "Terrain.h"

#include "sge_render/Renderer.h"

#include "sge_render/vertex/VertexLayoutManager.h"

namespace sge {

#if 0
struct TerrainSplit
{
	using IndexChunks = Vector_<VectorMap<u8, SPtr<RenderGpuBuffer>>, Terrain::kMaxLOD>;
	IndexChunks* pOut = nullptr;

	void split(IndexChunks& chunk_, Vec2i vertexSize_)
	{
		pOut = &chunk_;

		i32 lod = Math::ceil(log2(vertexSize_.x - 1)) - 1;
		_currentLod = lod;
		pOut->resize(lod - 1);
		if (lod - 1 == 0)
		{
			// insert the triangle to index buffer
		}
		_vertexCount = vertexSize_;
		//_halfCurrentSize = _currentSize / 2;

		_split({ 0, 0 }, { _vertexCount.x - 1, 0 }, { _vertexCount.x / 2, _vertexCount.y / 2}, 0);

	}

	void _split(Vec2i pt0_, Vec2i pt1_, Vec2i pt2_, int depth_)
	{

		auto line0 = pt1_ - pt0_;
		auto line1 = pt2_ - pt1_;
		auto line2 = pt0_ - pt2_;

		auto spt0 = line0 / 2;
		auto spt1 = line1 / 2;
		auto spt2 = line2 / 2;

		// check cannot split anymore
		/*if (spt0)
		{

		}*/

		if (_currentLod == depth_)
		{
			// lod - 1
			// insert the triangle to index buffer
		}

		//if (_currentLod + 1 == depth_)
		//{
		//	// max lod, add straight line to split
		//}

		else
		{
			auto nextDepth = depth_+ 1;

			// for top and left
			_split(pt0_, spt0, spt2, nextDepth);

			// for top
			_split(spt0, pt1_, spt1, nextDepth);

			// for left
			// split(spt2, spt1, pt2_);


			if (true)
			{

			}

			// recur
		}
	}

	void _spiltQuad(Vec2i pt0_, Vec2i pt1_, Vec2i pt2_, Vec2i pt3_)
	{
		Vec2i center = (pt0_ + pt1_ + pt2_ + pt3_) / 4;


	}

	int index(int x, int y) { return y * _vertexCount.x + x; }

private:
	u32 _currentLod = 0;
	Vec2i _vertexCount;

	Vector<u32> _lod0Indices;
	Vector<u32> _lod1Indices;


	//u32 _currentSize = 0;
	//u32 _halfCurrentSize = 0;
};
#endif // 0

#if 1

struct LODIndices
{
	using IndexType = Terrain::IndexType;

	void createEdgeIndices(int lod_, Vec2i vertexSize_)
	{
		_init(lod_, vertexSize_);
		_createTopBottonEdgeIndices();
		_createLeftEightEdgeIndices();
	}

	void next()
	{
		_clear();
		currentLod--;
		if (currentLod < 0)
			return;

		_update();
		_createTopBottonEdgeIndices();
		_createLeftEightEdgeIndices();
	}

	u32 index(int x, int y, int width_) { return y * width_ + x; }
#if 1
	u32 index(int x, int y) { return y * vertexSize.x + x; }
#else
	u32 index(int x, int y )			{ return (y + maxLod - currentLod) * vertexSize.x + x * (maxLod - currentLod + 1); }
#endif // 0

	u32 index(Vec2i coord_)				{ return coord_.y * vertexSize.x + coord_.x; }

	Vec2i indexToXY(u32 index_)			{ return { index_ % vertexSize.x, index_ / vertexSize.x }; }
	u32 topToBottom(u32 index_)			{ auto coord = indexToXY(index_); return index(vertexSize - coord - 1); }

	Span<const u32> getTopEdgeIndices_lod0()	{ return Span<const u32>(lod0Indices.begin() + stride0 * 0, lod0Indices.begin() + stride0 * 1); }
	Span<const u32> getBottomEdgeIndices_lod0() { return Span<const u32>(lod0Indices.begin() + stride0 * 1, lod0Indices.begin() + stride0 * 2); }
	Span<const u32> getLeftEdgeIndices_lod0()	{ return Span<const u32>(lod0Indices.begin() + stride0 * 2, lod0Indices.begin() + stride0 * 3); }
	Span<const u32> getRightEdgeIndices_lod0()	{ return Span<const u32>(lod0Indices.begin() + stride0 * 3, lod0Indices.begin() + stride0 * 4); }

	Span<const u32> getTopEdgeIndices_lod1()	{ return Span<const u32>(lod1Indices.begin() + stride1 * 0, lod1Indices.begin() + stride1 * 1); }
	Span<const u32> getBottomEdgeIndices_lod1() { return Span<const u32>(lod1Indices.begin() + stride1 * 1, lod1Indices.begin() + stride1 * 2); }
	Span<const u32> getLeftEdgeIndices_lod1()	{ return Span<const u32>(lod1Indices.begin() + stride1 * 2, lod1Indices.begin() + stride1 * 3); }
	Span<const u32> getRightEdgeIndices_lod1()	{ return Span<const u32>(lod1Indices.begin() + stride1 * 3, lod1Indices.begin() + stride1 * 4); }

private:
	void _init(int lod_, Vec2i vertexSize_)
	{
		SGE_ASSERT(!_isCreated);
		SGE_ASSERT(vertexSize_.x == vertexSize_.y);
		_isCreated = true;

		vertexSize = vertexSize_;

		int lodIndex = Math::log2(vertexSize.x - 1) - 1;
		currentLod = lodIndex;
		maxLod = lodIndex;

		_update();

		lod0Indices.reserve(stride0 * 4);
		lod1Indices.reserve(stride1 * 4);

		tmpIndices.reserve(stride1);
	}


	void _emplace_back_TopBottom(Vector<u32>& dst_, int x0_, int y0_, int x1_, int y1_, int x2_, int y2_)
	{
		dst_.emplace_back(index(x0_, y0_)); dst_.emplace_back(index(x1_, y1_)); dst_.emplace_back(index(x2_, y2_)); 
		tmpIndices.emplace_back(topToBotton_Index(x0_, y0_)); tmpIndices.emplace_back(topToBotton_Index(x2_, y2_)); tmpIndices.emplace_back(topToBotton_Index(x1_, y1_));
	}
	void _emplace_back_LeftRight(Vector<u32>& dst_, int x0_, int y0_, int x1_, int y1_, int x2_, int y2_)
	{
		dst_.emplace_back(index(x0_, y0_)); dst_.emplace_back(index(x1_, y1_)); dst_.emplace_back(index(x2_, y2_)); 
		tmpIndices.emplace_back(leftToRight_Index(x0_, y0_)); tmpIndices.emplace_back(leftToRight_Index(x2_, y2_)); tmpIndices.emplace_back(leftToRight_Index(x1_, y1_));
	}

#if 0
	void _lod0_emplace_back_TopBottom(int x_, int y_)		{ lod0Indices.emplace_back(index(x_, y_)); tmpIndices.emplace_back(index(x_, vertexSize.y - 1 - y_)); }
	void _lod1_emplace_back_TopBottom(int x_, int y_)		{ lod1Indices.emplace_back(index(x_, y_)); tmpIndices.emplace_back(index(x_, vertexSize.y - 1 - y_)); }

	void _lod0_emplace_back_LeftRight(int x_, int y_)		{ lod0Indices.emplace_back(index(x_, y_)); tmpIndices.emplace_back(index(vertexSize.x - 1 - x_, y_)); }
	void _lod1_emplace_back_LeftRight(int x_, int y_)		{ lod1Indices.emplace_back(index(x_, y_)); tmpIndices.emplace_back(index(vertexSize.x - 1 - x_, y_)); }

#endif // 0

	void _mergeAndClearIndicesChunk(Vector<u32>& dst_, size_t offset_, Vector<u32>& src_)
	{
		SGE_ASSERT(dst_.capacity() - dst_.size() >= src_.size());
		if (src_.size() == 0)
			return;

		dst_.resize(dst_.size() + src_.size());
		auto* pDst = dst_.data() + offset_;
		memcpy(pDst, src_.data(), src_.size() * sizeof(u32));
		src_.clear();
	}

	void _createTopBottonEdgeIndices()
	{
		int end = Math::pow2(currentLod) - 1;

		auto factor0 = Math::pow2(inverseLod);		// 1, 2, 4, ...
		auto factor1 = Math::pow2(inverseLod + 1);	// 2, 4, 8, ...
		auto factor2 = Math::pow2(inverseLod + 2);	// 4, 8, 16, ...

		// LOD: lod - 1
		{
			if (end == 0)
			{
				auto start = 0 * factor1;
				_emplace_back_TopBottom(lod0Indices, start + 0, 0, start + factor1, 0, start + factor0, factor0);
			}

			// \/\ \/\ \/\ \/	eg. for lod == 2 (x vertex == 9)
			for (int i = 0; i < end; i++)
			{
				auto start = i * factor1;
				_emplace_back_TopBottom(lod0Indices,	  start + 0,  0,	 start + factor1,		0, start + factor0, factor0);
				_emplace_back_TopBottom(lod0Indices, start + factor1, 0, start + factor0 * 3, factor0, start + factor0, factor0);

				if (i == end - 1)
				{
					_emplace_back_TopBottom(lod0Indices, start + factor1, 0, start + factor2, 0, start + 3 * factor0, factor0);
				}
			}
			_mergeAndClearIndicesChunk(lod0Indices, lod0Indices.size(), tmpIndices);
		}

		// LOD: lod
		{
			if (end == 0)
			{
				auto start = 0 * factor1;
				_emplace_back_TopBottom(lod1Indices,	   start + 0, 0,	 start + factor0,		0, start + factor0,	factor0);	// \|
				_emplace_back_TopBottom(lod1Indices, start + factor0, 0,	 start + factor1,		0, start + factor0,	factor0);	//   |/
			}

			// \|/\ \|/|\ \|/\ \|/	eg. for lod == 2 (x vertex == 9)
			for (int i = 0; i < end; i++)
			{
				auto start = i * factor1;
				_emplace_back_TopBottom(lod1Indices,	   start + 0, 0,	 start + factor0,		0, start + factor0,	factor0);	// \|
				_emplace_back_TopBottom(lod1Indices, start + factor0, 0,	 start + factor1,		0, start + factor0,	factor0);	//   |/
				_emplace_back_TopBottom(lod1Indices, start + factor1, 0, start + factor0 * 3, factor0, start + factor0,	factor0);	//    /\

				if (i == end - 1)
				{
					_emplace_back_TopBottom(lod1Indices,	start + factor1,  0, start + factor0 * 3,	0, start + factor0 * 3, factor0);
					_emplace_back_TopBottom(lod1Indices, start + 3 * factor0, 0,	 start + factor2,	0, start + factor0 * 3, factor0);
				}
			}
			// merge bottom edge to the main indices
			_mergeAndClearIndicesChunk(lod1Indices, lod1Indices.size(), tmpIndices);
		}
	}
	void _createLeftEightEdgeIndices()
	{
		int end = static_cast<int>(Math::pow(2.0f, static_cast<float>(currentLod))) - 1;

		auto factor0 = Math::pow2(inverseLod);		// 1, 2, 4, ...
		auto factor1 = Math::pow2(inverseLod + 1);	// 2, 4, 8, ...
		auto factor2 = Math::pow2(inverseLod + 2);	// 4, 8, 16, ...

		// LOD: lod - 1
		{
			if (end == 0)
			{
				auto start = 0 * factor1;
				_emplace_back_TopBottom(lod0Indices,	   0,		start + 0, factor0,		start + factor0, 0, start + factor1);
			}

			// similar to top
			for (int i = 0; i < end; i++)
			{
				auto start = i * factor1;
				_emplace_back_LeftRight(lod0Indices,	   0,		start + 0, factor0,		start + factor0, 0, start + factor1);
				_emplace_back_LeftRight(lod0Indices, factor0, start + factor0, factor0, start + factor0 * 3, 0, start + factor1);

				//auto start = i * 2;
				//_emplace_back_LeftRight(lod0Indices, 0, start + 0,	1, start + 1, 0, start + 2);
				//_emplace_back_LeftRight(lod0Indices, 1, start + 1, 1, start + 3, 0, start + 2);

				if (i == end - 1)
				{
					_emplace_back_LeftRight(lod0Indices, 0, start + factor1, factor0, start + factor0 * 3, 0, start + factor2);

					//_emplace_back_LeftRight(lod0Indices, 0, start + 2, 1, start + 3, 0, start + 4);
				}
			}
			_mergeAndClearIndicesChunk(lod0Indices, lod0Indices.size(), tmpIndices);
		}

		// LOD: lod
		{
			if (end == 0)
			{
				auto start = factor1 * 0;
				_emplace_back_LeftRight(lod1Indices,	   0,		  start + 0, factor0,	  start + factor0, 0, start + factor1);
				_emplace_back_LeftRight(lod1Indices,	   0,	start + factor0, factor0,	  start + factor0, 0, start + factor1);
			}
			for (int i = 0; i < end; i++)
			{
				auto start = i * factor1;
				_emplace_back_LeftRight(lod1Indices,	   0,		  start + 0, factor0,	  start + factor0, 0, start + factor0);
				_emplace_back_LeftRight(lod1Indices,	   0,	start + factor0, factor0,	  start + factor0, 0, start + factor1);
				_emplace_back_LeftRight(lod1Indices, factor0,	start + factor0, factor0, start + factor0 * 3, 0, start + factor1);

				//auto start = i * 2;
				//_emplace_back_LeftRight(lod1Indices, 0, start + 0, 1, start + 1, 0, start + 1);
				//_emplace_back_LeftRight(lod1Indices, 0, start + 1, 1, start + 1, 0, start + 2);
				//_emplace_back_LeftRight(lod1Indices, 1, start + 1, 1, start + 3, 0, start + 2);


				if (i == end - 1)
				{
					_emplace_back_LeftRight(lod1Indices, 0,		start + factor1, factor0, start + factor0 * 3, 0, start + factor0 * 3);
					_emplace_back_LeftRight(lod1Indices, 0, start + factor0 * 3, factor0, start + factor0 * 3, 0, start + factor2);

					//_emplace_back_LeftRight(lod1Indices, 0, start + 2, 1, start + 3, 0, start + 3);
					//_emplace_back_LeftRight(lod1Indices, 0, start + 3, 1, start + 3, 0, start + 4);
				}
			}
			// merge right edge to the main indices
			_mergeAndClearIndicesChunk(lod1Indices, lod1Indices.size(), tmpIndices);
		}
	}

	u32 topToBotton_Index(int x_, int y_) { return index(					x_, vertexSize.y - 1 - y_); }
	u32 leftToRight_Index(int x_, int y_) { return index(vertexSize.x - 1 - x_, y_); }

	void _clear()
	{
		lod0Indices.clear();
		lod1Indices.clear();
		tmpIndices.clear();
		//stride0 = 0, stride1 = 0;
	}

	void _update()
	{
		inverseLod = maxLod - currentLod;
		auto halfWidth = Math::pow2(currentLod + 1) / 2;
		stride0 = (halfWidth * 1 + halfWidth - 1) * 3;
		stride1 = (halfWidth * 2 + halfWidth - 1) * 3;
	}

public:
	bool _isCreated = false;
	Vec2i vertexSize;
	Vector<u32> lod0Indices;
	Vector<u32> lod1Indices;
	Vector<u32> tmpIndices;

	int currentLod = 0;
	int maxLod = 0;
	int inverseLod = 0;
	u32 stride0 = 0, stride1 = 0;
};

class ChunkIndexGenerator
{
public:
	using IndexChunks	= Terrain::IndexChunks;
	using IndexType = Terrain::IndexType;

	void generate(IndexChunks& indexChunks_, Vec2i vertexSize_)
	{
		pOut = &indexChunks_;

		int lod = Math::log2(vertexSize_.x - 1);
		pOut->resize(lod);
		_maxLod = lod - 1;

		LODIndices lodIndices;
		lodIndices.createEdgeIndices(_maxLod, vertexSize_);

#if 1
		auto* renderer = Renderer::instance();
		for (int j = _maxLod; j >= 0; j--)
		{
			int tblr = 0b00000000;
			auto& indicesChunks = (*pOut)[j];
			indicesChunks.resize(Terrain::kMaxCombination);
			for (size_t i = 0; i < Terrain::kMaxCombination; i++)
			{
				// Top
				if (BitUtil::hasBit(tblr, 3))		_copyTo(indices, lodIndices.getTopEdgeIndices_lod1());
				else										_copyTo(indices, lodIndices.getTopEdgeIndices_lod0());

				// Bottom
				if (BitUtil::hasBit(tblr, 2))		_copyTo(indices, lodIndices.getBottomEdgeIndices_lod1());
				else										_copyTo(indices, lodIndices.getBottomEdgeIndices_lod0());

				// Left
				if (BitUtil::hasBit(tblr, 1))		_copyTo(indices, lodIndices.getLeftEdgeIndices_lod1());
				else										_copyTo(indices, lodIndices.getLeftEdgeIndices_lod0());

				// Right
				if (BitUtil::hasBit(tblr, 0))		_copyTo(indices, lodIndices.getRightEdgeIndices_lod1());
				else										_copyTo(indices, lodIndices.getRightEdgeIndices_lod0());

				auto& dst = indicesChunks[tblr];

				RenderGpuBuffer::CreateDesc desc;
				desc.type = RenderGpuBufferType::Index;
				ByteSpan indexData = spanCast<const u8, const u32>(indices);
				desc.bufferSize = indexData.size();
				desc.stride = RenderDataTypeUtil::getByteSize(RenderDataTypeUtil::get<IndexType>()) * 8;

				auto sp = renderer->createGpuBuffer(desc);
				dst = std::move(sp);
				dst->uploadToGpu(indexData);

				indices.clear();
				tblr++;
			}

			lodIndices.next();
		}
		
#endif // 0

	}

private:
	void _copyTo(Vector<u32>& dst_, Span<const u32> src_)
	{
		auto oldSize = dst_.size();
		dst_.resize(dst_.size() + src_.size());
		memcpy(dst_.data() + oldSize, src_.data(), src_.size() * sizeof(Terrain::IndexType));
	}

	void _clear()
	{
	}

private:
	IndexChunks* pOut = nullptr;
	int _maxLod = 0;
	Vec2i _vertexCount;

	Vector<u32> indices;
};

#endif // 0

Terrain::Terrain(CreateDesc& desc_)
{
	//desc_.heightMap.<Color4b>();

	_size = desc_.heightMap.size();

}

void Terrain::create(CreateDesc& desc_)
{
	auto mapSize = desc_.heightMap.size();
	_size = mapSize;
	//_size = { 5, 5 };
	_size = { 17, 17 };

	int lodIndex = Math::log2(_size.x - 1) - 1;

	_initMesh(desc_, _testRenderMesh, lodIndex, {0, 0});
	_initMesh(desc_, _testRenderMesh2, lodIndex - 1, {_size.x - 1, 0});
}

void Terrain::_initMesh(const CreateDesc& desc_, RenderMesh& outMesh_, int lod_, Vec2i offset_)
{
	auto cellSize = _size - 1;
	auto vertexCount = _size.x * _size.y;
	auto triangleCount = cellSize.x * cellSize.y * 3;
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
			height = -1;
			editMesh.positions.emplace_back(-size_f.x / 2.0f + i + offset_.x, height, -size_f.y / 2.0f + j + offset_.y);
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

#if 0
	for (int j = 0; j < cellSize.y; j++)
	{
		for (int i = 0; i < cellSize.x; i++)
		{
			editMesh.indices.emplace_back( j	  * _size.x + i + 0);
			editMesh.indices.emplace_back((j + 1) * _size.x + i + 0);
			editMesh.indices.emplace_back( j	  * _size.x + i + 1);

			editMesh.indices.emplace_back( j	  * _size.x + i + 1);
			editMesh.indices.emplace_back((j + 1) * _size.x + i + 0);
			editMesh.indices.emplace_back((j + 1) * _size.x + i + 1);
		}
	}
#else
	ChunkIndexGenerator generator;
	if (_indexChunks.size() == 0)
	{
		generator.generate(_indexChunks, _size);
	}

#endif // 0

	outMesh_.create(editMesh);

	u8 tblr = 0b00000000;
	outMesh_.subMeshes()[0].setIndexBuffer(_indexChunks[lod_][tblr + 12], RenderDataTypeUtil::get<IndexType>());

}

}