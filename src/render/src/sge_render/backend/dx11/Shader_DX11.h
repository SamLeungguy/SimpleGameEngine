#pragma once

#if SGE_RENDER_HAS_DX11

#include <sge_render/shader/Shader.h>
#include <sge_render/backend/dx11/Render_Common_DX11.h>

namespace sge {

class Shader_DX11 : public Shader
{
	using Base = Shader;
public:
	using Util = DX11Util;
	Shader_DX11(StrView filename_);

	struct MyPass;

	static void _loadStageFile(StrView passPath_, ShaderStageMask stageMask_, Vector<u8>& outBytecode, ShaderStageInfo& outInfo_);

	struct MyVertexStage : public ShaderVertexStage
	{
		void load(MyPass* pass_, StrView passPath_, DX11_ID3DDevice* pDev_);
		void bind(RenderContext_DX11* pCtx_);
		ByteSpan bytecode() const { return _bytecode; }

	private:
		ComPtr<DX11_ID3DVertexShader> _cpShader;
		Vector<u8> _bytecode;
	};

	struct MyPixelStage : public ShaderPixelStage
	{
		void load(MyPass* pass_, StrView passPath_, DX11_ID3DDevice* pDev_);
		void bind(RenderContext_DX11* pCtx_);
		ByteSpan bytecode() const { return _bytecode; }

	private:
		ComPtr<DX11_ID3DPixelShader> _cpShader;
		Vector<u8> _bytecode;
	};

	struct MyPass : public ShaderPass
	{
		using Base = ShaderPass;

		MyPass(Shader_DX11* pShader_, StrView passPath_, ShaderInfo::Pass& info_);

	private:
		MyVertexStage _myVertexStage;
		MyPixelStage _myPixelStage;
	};

private:
};

}

#endif