#pragma once

#include "RenderMesh.h"

namespace sge {

struct Shader_CreateDesc
{
	StringW filepath;
	VertexLayout* pVertexLayout = nullptr;
};

class Shader : public NonCopyable
{
public:
	using CreateDesc = Shader_CreateDesc;

	Shader(CreateDesc& desc_);
	virtual ~Shader() = default;

	virtual void bind() = 0;

	static Shader* create(CreateDesc& desc_);
private:

};

}