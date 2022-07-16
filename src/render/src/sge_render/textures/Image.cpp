#include "sge_render-pch.h"
#include "Image.h"

namespace sge {

Image::Image(Image&& r_)
	:
	_info		(std::move(r_._info)),
	_pixelData	(std::move(r_._pixelData))
{
}

void Image::operator=(Image&& r_)
{
	_info = std::move(r_._info);
	_pixelData = std::move(r_._pixelData);
}

void Image::clear()
{
	_info = Info();
	_pixelData.clear();
}

void Image::loadFile(StrView filename_)
{

}

void Image::loadPngFile(StrView filename_)
{

}
void Image::loadPngMem(ByteSpan data_)
{

}

void Image::create(ColorType colorType_, int width_, int height_)
{
	create(colorType_, width_, height_, width_ * ColorUtil::pixelSizeInBytes(colorType_));
}

void Image::create(ColorType colorType_, int width_, int height_, int strideInBytes_)
{
	_create(colorType_, width_, height_, strideInBytes_, 1, strideInBytes_ * height_);
}

void Image::_create(ColorType colorType_, int width_, int height_, int strideInBytes_, int mipmapCount_, int dataSizeInBytes_)
{
	clear();

	_info.colorType = colorType_;
	_info.size.set(width_, height_);
	_info.strideInBytes = strideInBytes_;
	_info.mipmapCount = mipmapCount_;

	try{
		_pixelData.resize(dataSizeInBytes_);
	}catch(...) {
		clear();
		throw;
	}
}

}