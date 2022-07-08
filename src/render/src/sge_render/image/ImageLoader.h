#pragma once

#include <sge_core/base/Error.h>
#include <libpng16/png.h>

namespace sge {

class PngReader {
public:
	~PngReader() {
		destroy();
	}

	void destroy() {

		if (_png) {
			png_destroy_read_struct(&_png, &_info, nullptr);
			_png = nullptr;
			_info = nullptr;
		}
	}

	static void _onRead(png_structp png_ptr, png_bytep data, png_size_t length) {
		auto* p = reinterpret_cast<PngReader*>(png_get_io_ptr(png_ptr));
		p->onRead(data, length);
	}

	void onRead(png_bytep data, png_size_t length) {
		auto n = length;
		if (_pRead + n > _mm.span().end())
		{
			throw SGE_ERROR("onRead out of range");
		}
		memcpy(data, _pRead, n);
		_pRead += n;
	}

	void loadFile(const char* filename, bool mulAlpha) {
		destroy();

		_mm.openRead(filename);
		_pRead = _mm.data();

		_png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!_png)
			throw SGE_ERROR("png");

		_info = png_create_info_struct(_png);
		if (!_info)
			throw SGE_ERROR("png info");

		png_set_read_fn(_png, this, _onRead);
		png_read_info(_png, _info);

		png_uint_32 w, h;
		int bit;
		int colorType;
		int interlace;
		int compression;
		int filter;

		png_get_IHDR(_png, _info, &w, &h, &bit, &colorType, &interlace, &compression, &filter);

		_width = w;
		_height = h;

		switch (colorType) {
		case PNG_COLOR_TYPE_RGB_ALPHA: {
		}break;

		case PNG_COLOR_TYPE_RGB: {
			png_set_filler(_png, 255, PNG_FILLER_AFTER);
		}break;

		case PNG_COLOR_TYPE_GRAY: {
			png_set_gray_to_rgb(_png);
			png_set_filler(_png, 255, PNG_FILLER_AFTER);
		}break;

		case PNG_COLOR_TYPE_GRAY_ALPHA: {
			png_set_gray_to_rgb(_png);
		}break;

		case PNG_COLOR_TYPE_PALETTE: {
			png_set_palette_to_rgb(_png);

			bool hasAlpha = false;

			{
				//get palette transparency table
				png_bytep		trans_alpha = nullptr;
				int				nu_trans   = 0;
				png_color_16p	trans_color = nullptr;
				png_get_tRNS(_png, _info, &trans_alpha, &nu_trans, &trans_color);
				if (trans_alpha)
					hasAlpha = true;
			}

			if (!hasAlpha)
				png_set_filler(_png, 255, PNG_FILLER_AFTER);
		}break;
		}

		_pixels.resize(w * h * 4);

		Vector<uint8_t*> rows;
		rows.resize(h);

		for (size_t i = 0; i < h; i++) {
			rows[i] = &_pixels[i * w * 4];
		}

		png_read_image(_png, rows.data());

		if (mulAlpha) {
			for (auto& p : _pixels) {
				auto* ptr = &p;
				ptr[0] = static_cast<uint8_t>(static_cast<int>(ptr[0]) * ptr[3] / 255);
				ptr[1] = static_cast<uint8_t>(static_cast<int>(ptr[1]) * ptr[3] / 255);
				ptr[2] = static_cast<uint8_t>(static_cast<int>(ptr[2]) * ptr[3] / 255);
			}
		}
	}

	int width () const { return _width; }
	int height() const { return _height; }

	const Color4b* pixels() const { return reinterpret_cast<const Color4b*>(_pixels.data()); }

private:
	int _width = 0;
	int _height = 0;
	png_structp _png = nullptr;
	png_infop _info = nullptr;
	Vector<u8> _pixels;
	MemMapFile _mm;
	const u8* _pRead = nullptr;
};

}