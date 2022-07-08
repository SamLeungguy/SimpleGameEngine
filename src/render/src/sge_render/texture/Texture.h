#pragma once

#include <sge_render/image/ImageLoader.h>

namespace sge {

//struct TargaHeader
//{
//	unsigned char data1[12];
//	unsigned short width;
//	unsigned short height;
//	unsigned char bpp;
//	unsigned char data2;
//};

enum class TextureWrapper {
	None,
};

struct Texture_CreateDesc
{
	TextureWrapper param;

	u32 width, height = 0;
};

class Texture : public Object
{
public:
	using CreateDesc = Texture_CreateDesc;

	Texture() = default;
	Texture(StrView filepath_, CreateDesc& desc_);
	virtual ~Texture() = default;

	void loadMem(const Color4b* data_, CreateDesc& desc_);

	//RenderDataType getType() const { return type_; }

protected:
	//RenderDataType type_;
	void _init(const PngReader& reader_, CreateDesc& outDesc_);

protected:

	u32 _width, _height = 0;

private:

};

#if 0
#pragma --------- mark Texture
#endif // 0
#if 1    // Texture

#endif // 1    // Texture



}