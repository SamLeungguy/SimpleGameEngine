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

	static bool s_createDirectory(StrViewW pathW_)
	{
		//_SECURITY_ATTRIBUTES att;
		BOOL ret = ::CreateDirectory(pathW_.data(), NULL);
		if (ret)
			return true;
		//  the function fails, the return value is zero. To get extended error information, call GetLastError. 
		auto err = ::GetLastError();

		switch (err)
		{
			case ERROR_PATH_NOT_FOUND: { throw SGE_ERROR("ERROR_PATH_NOT_FOUND"); } break;
			case ERROR_ALREADY_EXISTS: { return false; } break;
		}
		return false;;
	}

	//static bool s_isPathExit(StrViewW pathW_) { BOOL ret = ::PathFileExists(pathW_.data()); return ret; }
#else
	using NativeFd = int;
	static const NativeFd s_invaild() { return -1; }
	static bool createDirectory(StrViewW pathW_) { SGE_ASSERT(0); return false; }
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

	void readBytes(Span<u8>		data_);
	void writeBytes(ByteSpan	data_);

	const String& getFilename() const;

	NativeFd getNativeFd(); 
	bool isOpened() const;

private:
	void _ensure_fd();
	bool _isFileExist(TempStringW& filenameW_);

private:

	String _filename;
	NativeFd _fd = s_invaild;

};

inline void FileStream::_ensure_fd() { if (_fd == s_invaild()) { throw SGE_ERROR("Invaild file handle"); } }

inline const String& FileStream::getFilename() const { return _filename; }

inline FileStream::NativeFd FileStream::getNativeFd() { return _fd; }

inline bool FileStream::isOpened() const { return _fd != s_invaild; }

#if SGE_OS_WINDOWS
inline bool FileStream::_isFileExist(TempStringW& filenameW_)
{
	DWORD dwAttrib = GetFileAttributes(filenameW_.c_str());

	return (dwAttrib != INVALID_FILE_ATTRIBUTES 
		&& !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
#else
inline bool FileStream::_isFileExist(StrView path_)
{
	assert(0, "no handle");
	return false;
}
#endif // SGE_OS_WINDOWS

}