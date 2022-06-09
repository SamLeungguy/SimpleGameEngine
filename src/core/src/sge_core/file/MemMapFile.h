#pragma once

#include "FileStream.h"

namespace sge {

class MemMapFile : public NonCopyable
{
public:
	MemMapFile() = default;
	~MemMapFile();

	void open(StrView filename_);
	void close();
	
	void openRead(StrView filename_);
	void openAppend(StrView filename_);
	void openWrite(StrView filename_, bool truncate_);

	void open(StrView filename_, FileMode mode_, FileAccess access_, FileSharerMode shareMode_);
	void writeBytes(Span<const u8>	data_);

	const u8* data()	const;
	size_t size()		const;

	Span<const u8> span() const;
	operator Span<const u8>() { return _span; }

	const String& getFilename() const;

private:
	FileStream _fs;
#if SGE_OS_WINDOWS
	::HANDLE _mapping = nullptr; // !! it's null, not INVALID_HANDLE_VALUE for memory mapping 
#endif
	Span<const u8> _span;
};

inline const u8* MemMapFile::data()	const { return _span.data(); }
inline size_t MemMapFile::size()		const { return _span.size(); }

inline Span<const u8> MemMapFile::span() const { return _span; }

inline const String& MemMapFile::getFilename() const { return _fs.getFilename(); }

}