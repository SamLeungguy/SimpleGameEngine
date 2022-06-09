#include "FileStream.h"
#include "../string/UtfUtil.h"

namespace sge {

FileStream::~FileStream()
{
	close();
}

void FileStream::openRead(StrView filename_)
{
	open(filename_, FileMode::OpenExists, FileAccess::Read, FileSharerMode::Read);
}

void FileStream::openAppend(StrView filename_)
{
	open(filename_, FileMode::OpenOrCreate, FileAccess::ReadWrite, FileSharerMode::Read);
	setPosFromEnd(0);
}

void FileStream::openWrite(StrView filename_, bool truncate_)
{
	open(filename_, FileMode::OpenOrCreate, FileAccess::ReadWrite, FileSharerMode::Read);
	if (truncate_)
	{
		setFileSize(0);
	}
}

#if 0
#pragma mark------Windows
#endif // 0
#if SGE_OS_WINDOWS

void FileStream::close()
{
	if (!isOpened())
		return;

	BOOL ret = ::CloseHandle(_fd);
	if (!ret)
		throw SGE_ERROR("close file error");
	_fd = s_invaild();
}

#if 0
static ::HANDLE _s_createFile(TempStringW& filename_, DWORD create_flags_, DWORD access_flags_, DWORD share_flags_)
{
	auto fd = ::CreateFile(filename_.c_str(), access_flags_, share_flags_, NULL, create_flags_, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fd == FileStream::s_invaild())
	{
		DWORD err = ::GetLastError();
		switch (err)
		{
		case ERROR_FILE_NOT_FOUND:		throw SGE_ERROR("file not found");
		case ERROR_PATH_NOT_FOUND:		throw SGE_ERROR("path not found");
		case ERROR_FILE_EXISTS:			throw SGE_ERROR("file doesn't exists");
		case ERROR_ALREADY_EXISTS:		throw SGE_ERROR("file already exists");
		case ERROR_ACCESS_DENIED:		throw SGE_ERROR("access denied");
		case ERROR_SHARING_VIOLATION:	throw SGE_ERROR("sharing violation");
		default:						throw SGE_ERROR("open file error");
		}
	}
	return fd;
}
#endif // 0

void FileStream::open(StrView filename_, FileMode mode_, FileAccess access_, FileSharerMode shareMode_)
{
	close();
	_filename = filename_;

	DWORD create_flags = 0;
	DWORD access_flags = 0;
	DWORD share_flags = 0;

	switch (mode_)
	{
		case FileMode::CreateNew:		create_flags |= CREATE_NEW;		break;
		case FileMode::OpenExists:		create_flags |= OPEN_EXISTING;	break;
		case FileMode::OpenOrCreate:	create_flags |= OPEN_ALWAYS;	break;
	}

	switch (access_)
	{
		case FileAccess::Read:		access_flags |= GENERIC_READ;					break;
		case FileAccess::ReadWrite:	access_flags |= GENERIC_READ | GENERIC_WRITE;	break;
		case FileAccess::WriteOnly:	access_flags |= GENERIC_WRITE;					break;
	}

	switch (shareMode_)
	{
		case FileSharerMode::None:			break;
		case FileSharerMode::Read:		share_flags |= FILE_SHARE_READ;						break;
		case FileSharerMode::Write:		share_flags |= FILE_SHARE_WRITE;					break;
		case FileSharerMode::ReadWrite:	share_flags |= FILE_SHARE_READ | FILE_SHARE_WRITE; break;
	}

	TempStringW filenameW;
	UtfUtil::convert(filenameW, filename_);

	_fd = ::CreateFile(filenameW.c_str(), access_flags, share_flags, NULL, create_flags, FILE_ATTRIBUTE_NORMAL, NULL);
	if (_fd == s_invaild())
	{
		DWORD err = ::GetLastError();
		switch (err)
		{
		case ERROR_FILE_NOT_FOUND:		throw SGE_ERROR("file not found");
		case ERROR_PATH_NOT_FOUND:		throw SGE_ERROR("path not found");
		case ERROR_FILE_EXISTS:			throw SGE_ERROR("file doesn't exists");
		case ERROR_ALREADY_EXISTS:		throw SGE_ERROR("file already exists");
		case ERROR_ACCESS_DENIED:		throw SGE_ERROR("access denied");
		case ERROR_SHARING_VIOLATION:	throw SGE_ERROR("sharing violation");
		default:						throw SGE_ERROR("open file error");
		}
	}
#if 0


	if (mode_ == FileMode::OpenOrCreate && !_isFileExist(filenameW))
	{
		auto new_create_flags = CREATE_NEW;
		_fd = _s_createFile(filenameW, new_create_flags, access_flags, share_flags);
		close();
		_fd = _s_createFile(filenameW, create_flags, access_flags, share_flags);
	}
	else
	{
		_fd = ::CreateFile(filenameW.c_str(), access_flags, share_flags, NULL, create_flags, FILE_ATTRIBUTE_NORMAL, NULL);
		if (_fd == s_invaild())
		{
			DWORD err = ::GetLastError();
			switch (err)
			{
			case ERROR_FILE_NOT_FOUND:		throw SGE_ERROR("file not found");
			case ERROR_PATH_NOT_FOUND:		throw SGE_ERROR("path not found");
			case ERROR_FILE_EXISTS:			throw SGE_ERROR("file doesn't exists");
			case ERROR_ALREADY_EXISTS:		throw SGE_ERROR("file already exists");
			case ERROR_ACCESS_DENIED:		throw SGE_ERROR("access denied");
			case ERROR_SHARING_VIOLATION:	throw SGE_ERROR("sharing violation");
			default:						throw SGE_ERROR("open file error");
			}
		}
	}
#endif // 0

}

void FileStream::flush()
{
	throw SGE_ERROR("no impl");
}

void FileStream::readBytes(Span<u8> data_)
{
	_ensure_fd();
	if (data_.size() <= 0)
		return;
	if (data_.size() >= UINT32_MAX)
		throw SGE_ERROR("file read");

	DWORD dwSize = static_cast<DWORD>(data_.size());
	DWORD result;
	BOOL ret = ::ReadFile(_fd, data_.data(), dwSize, &result, nullptr);
	if (!ret)
	{
		DWORD e = ::GetLastError();
		switch (e)
		{
			case ERROR_LOCK_VIOLATION: throw SGE_ERROR("file lock violation");	break;
		}
		throw SGE_ERROR("file read");
	}
}

void FileStream::writeBytes(ByteSpan data_)
{
	_ensure_fd();
	if (data_.size() <= 0)
		return;
	if (data_.size() >= UINT32_MAX)
		throw SGE_ERROR("file read");

	DWORD dwSize = static_cast<DWORD>(data_.size());
	DWORD result;
	BOOL ret = ::WriteFile(_fd, data_.data(), dwSize, &result, nullptr);
	if (!ret) 
		throw SGE_ERROR("file write");
}

FileSize FileStream::getFileSize()
{
	_ensure_fd();

	DWORD high;
	DWORD low = ::GetFileSize(_fd, &high);
	if (low == INVALID_FILE_SIZE)
		throw SGE_ERROR("file size");

	auto fileSize = static_cast<FileSize>(high) << 32 | low;
	return fileSize;
}

void FileStream::setFileSize(FileSize newSize_)
{
	_ensure_fd();
	FileSize oldPos = getPos();
	setPos(newSize_);
	::SetEndOfFile(_fd);

	if (oldPos < newSize_)
		setPos(oldPos);
}

FileSize FileStream::getPos()
{
	_ensure_fd();
	LONG high = 0;
	LONG low = ::SetFilePointer(_fd, 0, &high, FILE_CURRENT);
	if (low < 0 || high < 0) throw SGE_ERROR("get file pos");
	auto pos = static_cast<FileSize>(low) | static_cast<FileSize>(high) << 32;
	return pos;
}

void FileStream::setPos(FileSize pos_)
{
	_ensure_fd();
	LONG high = static_cast<LONG>(pos_ >> 32);
	LONG low = static_cast<LONG>(pos_);
	::SetFilePointer(_fd, low, &high, FILE_BEGIN);
}

void FileStream::setPosFromEnd(FileSize pos_)
{
	_ensure_fd();
	LONG high = static_cast<LONG>(pos_ >> 32);
	LONG low = static_cast<LONG>(pos_);
	::SetFilePointer(_fd, low, &high, FILE_END);
}

#else

#if 0
#pragma mark ------ Linux
#endif // 0

void FileStream::close()
{
}

void FileStream::open(StrView filename_)
{
}

void FileStream::flush()
{
}

FileSize FileStream::getFileSize()
{
	return FileSize();
}

void FileStream::setFileSize(FileSize newSize_)
{
}

FileSize FileStream::getPos()
{
	return FileSize();
}

void FileStream::setPos(FileSize pos_)
{
}

void FileStream::setPosFromEnd(FileSize pos_)
{
}

void FileStream::readBytes(Span<u8> data_)
{
}

void FileStream::writeBytes(ByteSpan data_)
{
}
#endif // SGE_OS_WINDOWS

}