
#pragma once

namespace sge {

class Chunk : public NonCopyable
{
public:
	Chunk() = default;
	~Chunk() = default;

	void init(u64 nBytes_ = 4 * 1024);
	void* allocate(u64 nBytes_);
	bool deallocate(u64 nBytes_);

	void clear();

	void* getData();

	u64 getRemainingByteSize();
	u64 getByteSize();
	u64 getTotalByteSize();

private:
	u64 _offset = 0;
	Vector<u8> _buffer;
};

}