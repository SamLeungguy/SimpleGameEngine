
#if SGE_RENDER_HAS_DX11

#include "Shader_DX11.h"
#include "Renderer_DX11.h"
#include "RenderContext_DX11.h"

namespace sge {

Shader_DX11::Shader_DX11(StrView filename_) 
	: Base(filename_)
{
	auto* proj = ProjectSettings::instance();
	TempString passPath;

	size_t n = _info.passes.size();
	_passes.reserve(n);
	for (size_t i = 0; i < n; i++) {
		FmtTo(passPath, "{}/{}/dx11/pass{}", proj->importedPath(), filename_, i);
		auto* pass = new MyPass(this, passPath, _info.passes[i]);
		_passes.emplace_back(pass);
	}
}

void Shader_DX11::_loadStageFile(StrView passPath_, ShaderStageMask stageMask_, Vector<u8>& outBytecode, ShaderStageInfo& outInfo_)
{
	auto* profile = Util::getDxStageProfile(stageMask_);

	auto filename = Fmt("{}/{}.bin", passPath_, profile);
	File::readFile(filename, outBytecode);

	filename += ".json";
	JsonUtil::readFile(filename, outInfo_);
}

void Shader_DX11::MyVertexStage::load(MyPass* pass_, StrView passPath_, DX11_ID3DDevice* pDev_)
{
	_loadStageFile(passPath_, stageMask(), _bytecode, _info);
	auto hr = pDev_->CreateVertexShader(_bytecode.data(), _bytecode.size(), nullptr, _cpShader.ptrForInit());
	Util::throwIfError(hr);
}
void Shader_DX11::MyVertexStage::bind(RenderContext_DX11* pCtx_)
{
	auto* dc = pCtx_->renderer()->d3dDeviceContext();
	if (!_cpShader) throw SGE_ERROR("dx shader is null");
	dc->VSSetShader(_cpShader, 0, 0);
}

void Shader_DX11::MyPixelStage::load(MyPass* pass_, StrView passPath_, DX11_ID3DDevice* pDev_)
{
	_loadStageFile(passPath_, stageMask(), _bytecode, _info);
	auto hr = pDev_->CreatePixelShader(_bytecode.data(), _bytecode.size(), nullptr, _cpShader.ptrForInit());
	Util::throwIfError(hr);
}

void Shader_DX11::MyPixelStage::bind(RenderContext_DX11* pCtx_)
{
	auto* dc = pCtx_->renderer()->d3dDeviceContext();
	if (!_cpShader) throw SGE_ERROR("dx shader is null");
	dc->PSSetShader(_cpShader, 0, 0);
}

Shader_DX11::MyPass::MyPass(Shader_DX11* pShader_, StrView passPath_, ShaderInfo::Pass& info_)
	:
	Base(pShader_, info_)
{
	_vertexStage = &_myVertexStage;
	_pixelStage  = &_myPixelStage;

	auto* renderer = Renderer_DX11::instance();
	auto* dev = renderer->d3dDevice();

	if (info_.vsFunc.size()) { _myVertexStage.load(this, passPath_, dev); }
	if (info_.psFunc.size()) {  _myPixelStage.load(this, passPath_, dev); }
}

}
#endif