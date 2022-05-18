#pragma once

#if SGE_RENDER_HAS_DX11

#include "Render_Common_DX11.h"
#include <sge_render/material/RenderShader.h>

namespace sge {

struct RenderShader_DX11_Helper;

class RenderShader_DX11 : public RenderShader {
	using Base = RenderShader;
	using Util = DX11Util;

	friend struct RenderShader_DX11_Helper;
public:
	RenderShader_DX11(CreateDesc& desc_);
	~RenderShader_DX11() = default;

	void destroy();
	
	void reload();
	void bind();

	void uploadCBuffers(RenderShaderType type_);

	DX11_ID3DInputLayout* getInputLayout(const VertexLayout* src);

protected:
	ComPtr<DX11_ID3DVertexShader>		_cpVertexShader;
	ComPtr<DX11_ID3DPixelShader>		_cpPixelShader;

	Vector<u8> _vertexSrcCode;		// TODO: remove tmp

	VectorMap<const VertexLayout*, ComPtr<DX11_ID3DInputLayout>> _inputLayouts;
};

}

#endif