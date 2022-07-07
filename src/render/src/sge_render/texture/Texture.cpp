#include "sge_render-pch.h"

#include "Texture.h"

namespace sge {

Texture::Texture(StrView filepath_, CreateDesc& desc_)
{
	loadFile(filepath_, desc_);
}

Texture::Texture(CreateDesc& desc_)
{
	loadMem(desc_);
}

void Texture::loadFile(StrView filepath_, CreateDesc& desc_)
{
	u32 width = 0, height = 0;
	Vector<Color4b> data;
	desc_.width = width;
	desc_.height = height;
	desc_.data = data;

	loadMem(desc_);
}

void Texture::loadMem(CreateDesc& desc_)
{
	if (desc_.width <= 0 || desc_.height <= 0)
	{
		throw SGE_ERROR("invalid texture width: {} / height: {}", desc_.width, desc_.height);
	}

	_width = desc_.width;
	_height = desc_.height;
}

}