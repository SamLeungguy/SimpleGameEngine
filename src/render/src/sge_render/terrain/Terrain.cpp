#include "sge_render-pch.h"
#include "Terrain.h"

#include "sge_render/Renderer.h"

#include "sge_render/vertex/VertexLayoutManager.h"

#define _SGE_TERRRAIN_DEBUG 0

namespace sge {

#if 1

struct LODIndices
{
	using IndexType = Terrain::IndexType;

	void create(int lod_, Vec2i vertexSize_, int squareToTriangleRatio_ = 4)
	{
		_init(lod_, vertexSize_, squareToTriangleRatio_);
		_createIndices();
	}

	void next()
	{
		_clear();
		currentLod--;
		if (currentLod < 0)
			return;

		_update();
		_createIndices();
	}

	void setSquareToTriangleRatio(int squareToTriangleRatio_)
	{
		squareToTriangleRatio = squareToTriangleRatio_;
	}

	u32 index(int x, int y, int width_) { return y * width_ + x; }
	u32 index(int x, int y)				{ return y * vertexSize.x + x; }

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

	Span<const u32> getInsideIndices()			{ return Span<const u32>(tmpIndices.begin(), tmpIndices.end()); }

private:
	void _init(int lod_, Vec2i vertexSize_, int squareToTriangleRatio_)
	{
		SGE_ASSERT(!_isCreated);
		SGE_ASSERT(vertexSize_.x == vertexSize_.y);
		_isCreated = true;

		vertexSize = vertexSize_;

		int lodIndex = Math::log2(vertexSize.x - 1) - 1;
		currentLod = lodIndex;
		maxLod = lodIndex;

		squareToTriangleRatio = squareToTriangleRatio_;

		_update();

		lod0Indices.reserve(stride0 * 4);
		lod1Indices.reserve(stride1 * 4);

		tmpIndices.reserve(maxLod * squareToTriangleRatio * 3);
	}

	void _emplace_back(Vector<u32>& dst_, int x0_, int y0_, int x1_, int y1_, int x2_, int y2_)
	{
		dst_.emplace_back(index(x0_, y0_)); dst_.emplace_back(index(x1_, y1_)); dst_.emplace_back(index(x2_, y2_)); 
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

	void _createIndices()
	{
		int end = static_cast<int>(Math::pow(2.0f, static_cast<float>(currentLod))) - 1;

		auto factor0 = Math::pow2(inverseLod);		// 1, 2, 4, ...
		auto factor1 = Math::pow2(inverseLod + 1);	// 2, 4, 8, ...
		auto factor2 = Math::pow2(inverseLod + 2);	// 4, 8, 16, ...

		_createTopBottonEdgeIndices(end, factor0, factor1, factor2);
		_createLeftRightEdgeIndices(end, factor0, factor1, factor2);
		_createInsideIndices(end, factor0, factor1, factor2);
	}

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

	void _mergeIndicesChunk(Vector<u32>& dst_, size_t offset_, Vector<u32>& src_)
	{
		if (src_.size() == 0)
			return;

		dst_.resize(dst_.size() + src_.size());
		auto* pDst = dst_.data() + offset_;
		memcpy(pDst, src_.data(), src_.size() * sizeof(u32));
	}

	void _createTopBottonEdgeIndices(int end_, int factor0_, int factor1_, int factor2_)
	{
		auto end = end_, factor0 = factor0_, factor1 = factor1_, factor2 = factor2_;

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
	void _createLeftRightEdgeIndices(int end_, int factor0_, int factor1_, int factor2_)
	{
		auto end = end_, factor0 = factor0_, factor1 = factor1_, factor2 = factor2_;

		// LOD: lod - 1
		{
			if (end == 0)
			{
				auto start = 0 * factor1;
				_emplace_back_LeftRight(lod0Indices,	   0,		start + 0, factor0,		start + factor0, 0, start + factor1);
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
	void _createInsideIndices(int end_, int factor0_, int factor1_, int factor2_)
	{
		auto end = end_, factor0 = factor0_, factor1 = factor1_;
		// split inside square to triangle
		tmpIndices.clear();
		if (squareToTriangleRatio == 4)
		{
			for (int j = 0; j < end_; j++)
			{
				auto yStart = j * factor1 + factor0;
				for (int i = 0; i < end; i++)
				{
					auto xStart = i * factor1 + factor0;

					_emplace_back(tmpIndices,			xStart,			  yStart, xStart + factor1,		      yStart,   xStart + factor0, yStart + factor0);
					_emplace_back(tmpIndices, xStart + factor1,			  yStart, xStart + factor1, yStart + factor1,   xStart + factor0, yStart + factor0);
					_emplace_back(tmpIndices, xStart + factor0, yStart + factor0, xStart + factor1, yStart + factor1,			  xStart, yStart + factor1);
					_emplace_back(tmpIndices,			xStart,			  yStart, xStart + factor0, yStart + factor0,			  xStart, yStart + factor1);
				}
			}
		}
		else if (squareToTriangleRatio == 2)
		{
			for (int j = 0; j < end_; j++)
			{
				auto yStart = j * factor1 + factor0;
				for (int i = 0; i < end; i++)
				{
					auto xStart = i * factor1 + factor0;

					_emplace_back(tmpIndices, xStart, yStart, xStart + factor1,		      yStart,   xStart + factor1, yStart + factor1);
					_emplace_back(tmpIndices, xStart, yStart, xStart + factor1, yStart + factor1,			  xStart, yStart + factor1);
				}
			}
		}
		else
		{
			throw SGE_ERROR("invalid squareToTriangleRatio");
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

	int squareToTriangleRatio = 0;
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
		lodIndices.create(_maxLod, vertexSize_);

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
				else								_copyTo(indices, lodIndices.getTopEdgeIndices_lod0());

				// Bottom
				if (BitUtil::hasBit(tblr, 2))		_copyTo(indices, lodIndices.getBottomEdgeIndices_lod1());
				else								_copyTo(indices, lodIndices.getBottomEdgeIndices_lod0());

				// Left
				if (BitUtil::hasBit(tblr, 1))		_copyTo(indices, lodIndices.getLeftEdgeIndices_lod1());
				else								_copyTo(indices, lodIndices.getLeftEdgeIndices_lod0());

				// Right
				if (BitUtil::hasBit(tblr, 0))		_copyTo(indices, lodIndices.getRightEdgeIndices_lod1());
				else								_copyTo(indices, lodIndices.getRightEdgeIndices_lod0());

				_copyTo(indices, lodIndices.getInsideIndices());

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
			lodIndices.setSquareToTriangleRatio(2);
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
	_size = { 256, 256 };

	_patchCount = desc_.patchCount;
	_patchSize = _size / _patchCount;
	_patches.reserve(_patchCount.x * _patchCount.y);

	_patchSize.x = Math::next_pow2(_patchSize.x) + 1;
	_patchSize.y = _patchSize.x;
	_size = _patchCount * _patchSize;

	int lodIndex = Math::log2(_patchSize.x) - 1;
	if (lodIndex >= kMaxLOD)
		throw SGE_ERROR("lodIndex > kMaxLOD");
	_maxLodIndex = lodIndex;

	_heightMapImage = std::move(desc_.heightMap);

	ChunkIndexGenerator generator;
	if (_indexChunks.size() == 0)
		generator.generate(_indexChunks, _patchSize);

	_init();
}

void Terrain::update(const Math::Camera3f& camera_)
{
	for (Vec2i iPatch{0, 0}; iPatch.y < _patchCount.y; )
	{
		//update();
		Vec2i startPatch = getPatchCoord(camera_);
		
		_updatePatchLOD(startPatch);

		//SGE_LOG("{}, {}", startPatch.x, startPatch.y);

		if (iPatch.x >= _patchCount.x - 1)
		{
			iPatch.y++;
			iPatch.x = 0;
			continue;
		}
		iPatch.x++;
	}
}

void Terrain::_init()
{
	auto vertexCount = _patchSize.x * _patchSize.y;

	EditMesh tmpEditMesh;
	EditMesh& editMesh = tmpEditMesh;
	editMesh.positions.reserve(vertexCount);
	editMesh.uvs[0].reserve(vertexCount);
	editMesh.colors.resize(vertexCount);						// TODO: remove
	editMesh.normals.resize(vertexCount);						// TODO: remove

	{
		Vec2i iPatch_{0, 0};
		Vec2i offset		{		  iPatch_.x * _patchSize.x, iPatch_.y * _patchSize.y};
		Vec2f posOffset		{	 -_patchSize.x / 2 * offset.x, -_patchSize.y / 2 * offset.y};
		Vec2f uvOffset		{ 1.0f / _patchCount.x * offset.x, 1.0f / _patchCount.y * offset.y};

		for (Vec2i iPatchSize{0, 0}; iPatchSize.y < _patchSize.y;)
		{
			//auto height = _heightMapImage.pixel<ColorLs>(iPatchSize.x + offset.x, iPatchSize.y + offset.y).r * _normalizeFactor * _heightFactor - _heightOffset;
			auto height = -1;

			editMesh.positions.emplace_back(posOffset.x + iPatchSize.x, height, posOffset.y + iPatchSize.y);
			editMesh.uvs[0].emplace_back(iPatchSize.x / _size.x + uvOffset.x, iPatchSize.y / _size.y + uvOffset.y);

			if (iPatchSize.x >= _patchSize.x - 1)
			{
				iPatchSize.y++;
				iPatchSize.x = 0;
				continue;
			}
			iPatchSize.x++;
		}
	}

	auto& renderMesh = _testRenderMesh;
	renderMesh.create(editMesh);

	auto& subMesh = renderMesh.subMeshes()[0];

	u8 tblr = 0b00000000;

	for (Vec2i iPatch{0, 0}; iPatch.y < _patchCount.y; )
	{
		Vec2i offset		{ iPatch.x * (_patchSize.x - 1), iPatch.y * (_patchSize.y - 1)};
		auto& patch = _patches.emplace_back(new Patch);

		patch->offset = offset;
		patch->spVertexBuffer = subMesh.getVertexBuffer();
		patch->spIndexBuffer.reset(_indexChunks[_maxLodIndex][tblr + 0]);
		patch->modelMatrix = Mat4f::s_translate(Vec3f(static_cast<float>(patch->offset.x), 0.0f, static_cast<float>(patch->offset.y)));

		if (iPatch.x >= _patchCount.x - 1)
		{
			iPatch.y++;
			iPatch.x = 0;
			continue;
		}
		iPatch.x++;
	}
}

void Terrain::_updatePatchLOD(Vec2i startPatch_)
{
	//Vec2i startPatch{ 3, 7 };
	getPatch(startPatch_)->lod = _maxLodIndex;

	for (Vec2i iPatch{0, 0}; iPatch.y < _patchCount.y; )
	{
		//update();
		if (iPatch != startPatch_)
		{
			auto& patch = getPatch_unsafe(iPatch);
			auto diff = startPatch_ - iPatch;
			diff.x = Math::abs(diff.x);
			diff.y = Math::abs(diff.y);

			auto dist = _maxLodIndex - (diff.x + diff.y);
			if (dist > _maxLodIndex || dist <= 0)
				patch->lod = 0;
			else
				patch->lod = dist;
		}
			
		if (iPatch.x >= _patchCount.x - 1)
		{
			iPatch.y++;
			iPatch.x = 0;
			continue;
		}
		iPatch.x++;
	}

	for (Vec2i iPatch{0, 0}; iPatch.y < _patchCount.y; )
	{
		/*
		 --------> +x
		 |
		 |
		\|/
		 +y
		*/
		auto& current	= getPatch_unsafe(iPatch);
		auto& top		= getPatch_clamp(Vec2i{	   iPatch.x, iPatch.y - 1});
		auto& bottom	= getPatch_clamp(Vec2i{	   iPatch.x, iPatch.y + 1});
		auto& left		= getPatch_clamp(Vec2i{iPatch.x - 1, iPatch.y});
		auto& right		= getPatch_clamp(Vec2i{iPatch.x + 1, iPatch.y});

		u8 tblr = 0b0000;
		if (top->lod <= current->lod)		BitUtil::unset(tblr, 0b1000);
		else								BitUtil::set(  tblr, 0b1000);

		if (bottom->lod <= current->lod)		BitUtil::unset(tblr, 0b0100);
		else								BitUtil::set(  tblr, 0b0100);

		if (left->lod <= current->lod)		BitUtil::unset(tblr, 0b0010);
		else								BitUtil::set(  tblr, 0b0010);

		if (right->lod <= current->lod)		BitUtil::unset(tblr, 0b0001);
		else								BitUtil::set(  tblr, 0b0001);

#if 0
		{
			TempString rowStr;
			auto str = std::to_string(tblr);
			rowStr.append(str.c_str());
			rowStr.append(": ");
			//FmtTo(rowStr, "{},{}: = {} |", iPatch.x, iPatch.y, lod.c_str());
			if (top->lod <= current->lod)		rowStr.push_back('0');
			else								rowStr.push_back('1');
			if (bottom->lod <= current->lod)	rowStr.push_back('0');
			else								rowStr.push_back('1');
			if (left->lod <= current->lod)		rowStr.push_back('0');
			else								rowStr.push_back('1');
			if (right->lod <= current->lod)		rowStr.push_back('0');
			else								rowStr.push_back('1');
			rowStr.append("\n");
			SGE_LOG("{}", rowStr);

		}
#endif // 0

		current->spIndexBuffer.reset(_indexChunks[current->lod][tblr]);

		if (iPatch.x >= _patchCount.x - 1)
		{
			iPatch.y++;
			iPatch.x = 0;
			continue;
}
		iPatch.x++;
	}

#if _SGE_TERRRAIN_DEBUG
	static u64 tick = 0;
	if (tick % 14400 == 0)
	{
		TempString rowStr;
		for (Vec2i iPatch{0, 0}; iPatch.y < _patchCount.y; )
		{
			auto& patch = getPatch_unsafe(iPatch);
			auto lod = std::to_string(patch->lod);
			FmtTo(rowStr, "{},{}: = {} |", iPatch.x, iPatch.y, lod.c_str());

			if (iPatch.x >= _patchCount.x - 1)
			{
				iPatch.y++;
				iPatch.x = 0;

				SGE_LOG(rowStr.c_str());
				SGE_LOG("\n");
				rowStr.clear();
				continue;
			}
			iPatch.x++;
		}
		SGE_LOG("===============End");
		SGE_LOG("\n");
	}
	tick++;
#endif // 1
}

Vec2i Terrain::getPatchCoord(const Math::Camera3f& camera_)
{
	Vec2i startPatch{ 0, 0 };
	startPatch.x = static_cast<int>(camera_.pos().x / _patchSize.x);
	startPatch.y = static_cast<int>(camera_.pos().z / _patchSize.y);
	clampBoundary(startPatch);
	return startPatch;
}

#if 0
void Terrain::_init(const CreateDesc& desc_, RenderMesh& outMesh_, int lodIndex_, Vec2i offset_)
{
#if 1
	auto vertexCount = _patchSize.x * _patchSize.y;

	EditMesh tmpEditMesh;
	EditMesh& editMesh = tmpEditMesh;


	editMesh.positions.reserve(vertexCount);
	editMesh.uvs[0].reserve(vertexCount);
	editMesh.colors.resize(vertexCount);						// TODO: remove
	editMesh.normals.resize(vertexCount);						// TODO: remove
																//tmpEditMesh.indices.reserve(triangleCount * 3);				// TODO: remove

	float normalizeFactor = 1.0f / std::numeric_limits<u16>::max(), yOffset = 400.0f;
	float heightFactor = 800.0f;

	for (int j = 0; j < _patchSize.y; j++)
	{
		auto* row = _heightMapImage.row<ColorLs>(j).data();
		for (int i = 0; i < _patchSize.x; i++)
		{
			auto height = row->r * normalizeFactor * heightFactor - yOffset;
			height = -1;
			editMesh.positions.emplace_back(-_patchSize.x / 2.0f + i + offset_.x, height, -_patchSize.y / 2.0f + j + offset_.y);
			row++;
		}
	}
	for (int j = 0; j < _patchSize.y; j++)
	{
		for (int i = 0; i < _patchSize.x; i++)
		{
			editMesh.uvs[0].emplace_back(i / _patchSize.x, j / _patchSize.y);
		}
	}

	outMesh_.create(tmpEditMesh);

	u8 tblr = 0b00000000;
	outMesh_.subMeshes()[0].setIndexBuffer(_indexChunks[lodIndex_][tblr + 0]);
#endif // 0

#if 0
	auto vertexCount = _patchSize.x * _patchSize.y;

	EditMesh tmpEditMesh;
	tmpEditMesh.positions.reserve(vertexCount);
	tmpEditMesh.uvs[0].reserve(vertexCount);
	tmpEditMesh.colors.resize(vertexCount);				// TODO: remove
	tmpEditMesh.normals.resize(vertexCount);			// TODO: remove

	for (Vec2i iPatch{0, 0}; iPatch.y < _patchCount.y; )
	{
		//update();
		_initMesh(tmpEditMesh, iPatch);

		if (iPatch.x >= _patchCount.x - 1)
		{
			iPatch.y++;
			iPatch.x = 0;
			continue;
		}
		iPatch.x++;
}
#endif // 0

}

void Terrain::_initMesh(EditMesh& editMesh_, Vec2i iPatch_)
{
	//auto& patch = _patches[patchIndex(iPatch_)];
	auto& patch = *_patches.emplace_back(new Patch);

	Vec2i offset		{		  iPatch_.x * _patchSize.x, iPatch_.y * _patchSize.y};
	Vec2f posOffset		{	 -_patchSize.x / 2 * offset.x, -_patchSize.y / 2 * offset.y};
	Vec2f uvOffset		{ 1.0f / _patchCount.x * offset.x, 1.0f / _patchCount.y * offset.y};

	patch.offset = offset;
	patch.modelMatrix = Mat4f::s_translate(Vec3f{static_cast<float>(patch.offset.x), 0, static_cast<float>(patch.offset.y)});

	auto& editMesh = editMesh_;

	for (Vec2i iPatchSize{0, 0}; iPatchSize.y < _patchSize.y; )
	{
		auto height = _heightMapImage.pixel<ColorLs>(iPatchSize.x + offset.x, iPatchSize.y + offset.y).r * _normalizeFactor * _heightFactor - _heightOffset;
#if 0
		int vi = _patchSize.x * iPatchSize.y + iPatchSize.x;
		auto& pos = editMesh.positions[vi];
		pos.set(posOffset.x + iPatchSize.x, height, posOffset.y + iPatchSize.y);

		auto& uv = editMesh.uvs[0][vi];
		uv.set(iPatchSize.x / _size.x + uvOffset.x, iPatchSize.y / _size.y + uvOffset.y);
#else
		editMesh.positions.emplace_back(posOffset.x + iPatchSize.x, height, posOffset.y + iPatchSize.y);
		editMesh.uvs[0].emplace_back(iPatchSize.x / _size.x + uvOffset.x, iPatchSize.y / _size.y + uvOffset.y);
#endif // 0

		if (iPatchSize.x >= _patchSize.x - 1)
		{
			iPatchSize.y++;
			iPatchSize.x = 0;
			continue;
		}
		iPatchSize.x++;
	}

	/*patch.renderMesh.create(editMesh);
	auto& mesh = patch.renderMesh.subMeshes()[0];
	mesh.setIndexBuffer(_indexChunks[patch.lod][patch.tblr]);*/

	editMesh.clear();
}
#endif // 0

}