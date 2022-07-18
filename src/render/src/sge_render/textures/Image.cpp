#include "sge_render-pch.h"
#include "Image.h"
#include "ImageIO_png.h"
#include "ImageIO.dds.h"

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

void Image::loadFile(StrView filename) {
	auto ext = FilePath::extension(filename);
	if (0 == StringUtil::ignoreCaseCompare(ext, "png")) {
		return loadPngFile(filename);
	}

	if (0 == StringUtil::ignoreCaseCompare(ext, "dds")) {
		return loadDdsFile(filename);
	}

	throw SGE_ERROR("unsupported image file format {}", ext);
}

void Image::loadPngFile(StrView filename) {
	MemMapFile mm;
	mm.open(filename);
	loadPngMem(mm);
}

void Image::loadPngMem(ByteSpan data) {
	ImageIO_png::Reader r;
	r.load(*this, data);
}

void Image::loadDdsFile(StrView filename) {
	MemMapFile mm;
	mm.open(filename);
	loadDdsMem(mm);
}

void Image::loadDdsMem(ByteSpan data) {
	ImageIO_dds::Reader r;
	r.load(*this, data);
}

void Image::create(ColorType colorType_, int width_, int height_)
{
	create(colorType_, width_, height_, width_ * ColorUtil::pixelSizeInBytes(colorType_));
}

void Image::create(ColorType colorType_, int width_, int height_, int strideInBytes_)
{
	_create(colorType_, width_, height_, strideInBytes_, 1, strideInBytes_ * height_);
}

void Image::create(ColorType colorType, int width, int height, int strideInBytes, int mipmapCount, size_t dataSizeInBytes) {
	_create(colorType, width, height, strideInBytes, mipmapCount, dataSizeInBytes);
}

void Image::_create(ColorType colorType_, int width_, int height_, int strideInBytes_, int mipmapCount_, size_t dataSizeInBytes_)
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