#pragma once

#include "Chunk.h"

namespace sge {

class LinearAllocator : public NonCopyable
{
public:
	LinearAllocator() = default;
	~LinearAllocator() = default;

	void init(u64 nBytes_ = 4 * 1024);

	void* allocate(u64 nBytes_);
	bool deallocate(u64 nBytes_);
	void clear();

	u64 getChunkCount();
	u64 getTotalByteSize();

	List<Chunk>& getChunkList();
	const List<Chunk>& getChunkList() const;

	List<Chunk>::iterator& getCurrentIterator();
	const List<Chunk>::iterator& getCurrentIterator() const;

private:
	List<Chunk> _chunkList;
	List<Chunk>::iterator _itCurrentChunk;			// it only work for list as it do not resize

	//bool isCleared = false;
};

}