#pragma once

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

// don't store it!!!
struct Texture_CreateDesc
{
	TextureWrapper param;

	u32 width, height = 0;
	Span<Color4b> data;
};

class Texture : public Object
{
public:
	using CreateDesc = Texture_CreateDesc;

	Texture() = default;
	Texture(StrView filepath_, CreateDesc& desc_);
	Texture(CreateDesc& desc_);
	virtual ~Texture() = default;

	void loadFile(StrView filepath_, CreateDesc& desc_);
	void loadMem(CreateDesc& desc_);

	//RenderDataType getType() const { return type_; }

protected:
	//RenderDataType type_;

	u32 _width, _height = 0;

private:

};

#if 0
#pragma --------- mark Texture
#endif // 0
#if 1    // Texture

#endif // 1    // Texture



}