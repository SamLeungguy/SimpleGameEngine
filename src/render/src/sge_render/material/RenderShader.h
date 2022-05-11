#pragma once

namespace sge {

struct RenderShader_CreateDesc {
	StrView filename;
};

class RenderShader : public Object
{
public:
	using CreateDesc = RenderShader_CreateDesc;

	RenderShader(CreateDesc& desc_);
	virtual ~RenderShader() = default;

private:

};

}