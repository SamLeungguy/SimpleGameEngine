#include "sge_render-pch.h"

#include "Texture.h"

namespace sge {

Texture::Texture(StrView filepath_, CreateDesc& desc_)
{
}

void Texture::loadMem(const Color4b* data_, CreateDesc& desc_)
{
	if (desc_.width <= 0 || desc_.height <= 0)
	{
		throw SGE_ERROR("invalid texture width: {} / height: {}", desc_.width, desc_.height);
	}

	_width = desc_.width;
	_height = desc_.height;
}

void Texture::_init(const PngReader& reader_, CreateDesc& outDesc_)
{
	outDesc_.width = reader_.width();
	outDesc_.height = reader_.height();

	if (outDesc_.width <= 0 || outDesc_.height <= 0)
	{
		throw SGE_ERROR("invalid texture width: {} / height: {}", outDesc_.width, outDesc_.height);
	}

	_width = outDesc_.width;
	_height = outDesc_.height;
}


}