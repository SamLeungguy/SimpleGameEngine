#pragma once

#include "File.h"
#include "../base/Error.h"

namespace sge {

class FileStream : public NonCopyable
{
public:
#if SGE_OS_WINDOWS
	using NativeFd = ::HANDLE;
	static const NativeFd s_invaild() { return INVALID_HANDLE_VALUE; }
#else
	using NativeFd = int;
	static const NativeFd s_invaild() { return -1; }
#endif // SGE_OS_WINDOWS

	FileStream() = default;
	~FileStream();

	void openRead(StrView filename_);
	void openAppend(StrView filename_);
	void openWrite(StrView filename_, bool truncate_);

	void close();

	void open(StrView filename_, FileMode mode_, FileAccess access_, FileSharerMode shareMode_);

	void flush();

	FileSize getFileSize();
	void setFileSize(FileSize newSize_);

	FileSize getPos();
	void setPos(FileSize pos_);
	void setPosFromEnd(FileSize pos_);

	void readBytes(Span<u8>			data_);
	void writeBytes(Span<const u8>	data_);

	const String& getFilename() const;

	NativeFd getNativeFd(); 
	bool isOpened() const;

private:
	void _ensure_fd();

	String _filename;
	NativeFd _fd = s_invaild;

};

inline void FileStream::_ensure_fd() { if (_fd == s_invaild()) { throw SGE_ERROR("Invaild file handle"); } }

inline const String& FileStream::getFilename() const { return _filename; }

inline FileStream::NativeFd FileStream::getNativeFd() { return _fd; }

inline bool FileStream::isOpened() const { return _fd != s_invaild; }

}