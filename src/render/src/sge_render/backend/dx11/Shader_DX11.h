#pragma once

#if SGE_RENDER_HAS_DX11
#include "Render_Common_DX11.h"
#include <sge_render/Renderer.h>
#include "../../Shader.h"

namespace sge {

class Shader_DX11 : public Shader
{
	using Base = Shader;
	using Util = DX11Util;

public:
	Shader_DX11(CreateDesc& desc_);
	virtual ~Shader_DX11();

	virtual void bind() override;
private:

	StringW _filepath;

	ComPtr<DX11_ID3DVertexShader>		_cpVertexShader;
	ComPtr<DX11_ID3DPixelShader>		_cpPixelShader;
	ComPtr<DX11_ID3DInputLayout>		_cpInputLayout;
};

}

#endif