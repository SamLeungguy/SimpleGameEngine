#pragma once

namespace sge {

struct ImageInfo
{
	Vec2i		size{ 0, 0 };
	int			strideInBytes = 0;
	int			mipmapCount = 1;
	ColorType	colorType = ColorType::None;

	int pixelSizeInBytes() const;
};

class Image : public NonCopyable
{
public:
	using Info = ImageInfo;

	Image() = default;
	Image(Image && r_);

	void operator=(Image && r_);

	void clear();

	void loadFile		(StrView filename_);
	
	void loadPngFile	(StrView filename_);
	void loadPngMem		(ByteSpan data_);

	void loadDdsFile	(StrView filename);
	void loadDdsMem	(ByteSpan data);

	void create			(ColorType colorType_, int width_, int height_);
	void create			(ColorType colorType_, int width_, int height_, int strideInBytes_);
	void create			(ColorType colorType, int width, int height, int strideInBytes, int mipmapCount, size_t dataSizeInBytes);

	template<class COLOR> void fill(const COLOR& color);

	SGE_INLINE const Info&		info				()	const;
	SGE_INLINE const Vec2i&		size				()	const;
	SGE_INLINE const int		strideInBytes		()	const;
	SGE_INLINE const int		pixelSizeInBytes	()	const;
	SGE_INLINE const int		width				()	const;
	SGE_INLINE const int		height				()	const;
	SGE_INLINE const ColorType	colorType			()	const;

	template<typename COLOR> SGE_INLINE Span<		COLOR> row(int y_);
	template<typename COLOR> SGE_INLINE Span<const	COLOR> row(int y_) const;

	template<typename COLOR> SGE_INLINE Span<		COLOR> row_noCheck(int y_);
	template<typename COLOR> SGE_INLINE Span<const	COLOR> row_noCheck(int y_) const;

	template<typename COLOR> SGE_INLINE		  COLOR& pixel(int x_, int y_);
	template<typename COLOR> SGE_INLINE const COLOR& pixel(int x_, int y_) const;

	SGE_INLINE Span<u8>			rowBytes(int y_);
	SGE_INLINE Span<const u8>	rowBytes(int y_) const;

	const void* dataPtr() const;

	void copyToPixelData(ByteSpan src);

private:
	void _create(ColorType colorType_, int width_, int height_, int strideInBytes_, int mipmapCount_, size_t dataSizeInBytes_);
	void _checkType(ColorType colorType) const;
	void _checkBoundary(int x_, int y_) const;

private:
	Info _info;
	Vector<u8> _pixelData;
};

#if 0
#pragma mark ===================== ImageInfo
#endif // 0
#if 1    // ImageInfo

inline int ImageInfo::pixelSizeInBytes() const { return ColorUtil::pixelSizeInBytes(colorType); }

#endif

#if 0
#pragma mark ===================== Image
#endif // 0
#if 1    // Image

SGE_INLINE const Image::Info&	Image::info				()	const { return _info; }
SGE_INLINE const Vec2i&			Image::size				()	const { return _info.size; }
SGE_INLINE const int			Image::strideInBytes	()	const { return _info.strideInBytes; }
SGE_INLINE const int			Image::pixelSizeInBytes	()	const { return _info.pixelSizeInBytes(); }
SGE_INLINE const int			Image::width			()	const { return _info.size.x; }
SGE_INLINE const int			Image::height			()	const { return _info.size.y; }
SGE_INLINE const ColorType		Image::colorType		()	const { return _info.colorType; }

template<typename COLOR> SGE_INLINE Span<		COLOR> Image::row(int y_)		{ _checkType(COLOR::kColorType); return row_noCheck<COLOR>(y_); }
template<typename COLOR> SGE_INLINE Span<const	COLOR> Image::row(int y_) const { _checkType(COLOR::kColorType); return row_noCheck<COLOR>(y_); }

template<typename COLOR> SGE_INLINE Span<		COLOR> Image::row_noCheck(int y_)		{ return Span<		COLOR>(reinterpret_cast<	  COLOR*>(rowBytes(y_).data()), width()); }
template<typename COLOR> SGE_INLINE Span<const	COLOR> Image::row_noCheck(int y_) const { return Span<const COLOR>(reinterpret_cast<const COLOR*>(rowBytes(y_).data()), width()); }

template<typename COLOR> SGE_INLINE		  COLOR& Image::pixel(int x_, int y_)		{ _checkBoundary(x_, y_); return row<COLOR>(y_)[x_];; }
template<typename COLOR> SGE_INLINE const COLOR& Image::pixel(int x_, int y_) const { _checkBoundary(x_, y_); return row<COLOR>(y_)[x_];; }

SGE_INLINE Span<u8>			Image::rowBytes(int y_)			{ return Span<		u8>(&_pixelData[y_ * strideInBytes()], width() * _info.pixelSizeInBytes()); }
SGE_INLINE Span<const u8>	Image::rowBytes(int y_) const	{ return Span<const u8>(&_pixelData[y_ * strideInBytes()], width() * _info.pixelSizeInBytes()); }

SGE_INLINE const void* Image::dataPtr() const { return _pixelData.data(); }

inline void Image::copyToPixelData(ByteSpan src) { _pixelData.assign(src.begin(), src.end()); }

inline void Image::_checkType(ColorType colorType_) const 
{
	if (colorType_ != _info.colorType) throw SGE_ERROR("Invalid ColorType");
}

inline void Image::_checkBoundary(int x_, int y_) const
{
	if (x_ >= width() || y_ >= height()) throw SGE_ERROR("out of Boundary");
}

template<class COLOR> inline
void Image::fill(const COLOR& color)
{
	_checkType(COLOR::kColorType);
	for (int y=0; y < height(); y++) {
		for (auto& p : row_noCheck<COLOR>(y)) {
			p = color;
		}
	}
}


#endif

}