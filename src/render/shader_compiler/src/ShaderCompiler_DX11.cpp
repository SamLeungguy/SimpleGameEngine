#include "ShaderCompiler_DX11.h"

#include <d3d11.h>
#include <d3d11_4.h>
#include <dxgi1_4.h>

#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace sge {

void ShaderCompiler_DX11::compile(StrView outPath_, ShaderStage shaderStage_, StrView srcFilename_, StrView entryFunc_) {
	TempStringA entryPoint = entryFunc_;

	MemMapFile memmap;
	memmap.open(srcFilename_);

	auto hlsl = memmap.span();

	UINT flags1 = 0; // D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
	UINT flags2 = 0;

#if _DEBUG
	//flags1 |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob>	byteCode;
	ComPtr<ID3DBlob>	errorMsg;

	auto profile = Util::getDxStageProfile(shaderStage_);

	auto hr = D3DCompile2(
		hlsl.data(), hlsl.size(), memmap.getFilename().c_str(),
		nullptr, nullptr,
		entryPoint.c_str(),
		profile,
		flags1, flags2, 0, nullptr, 0,
		byteCode.ptrForInit(),
		errorMsg.ptrForInit());

	if (FAILED(hr)) {
		throw SGE_ERROR("HRESULT={}\n Error Message: {}", hr, Util::toStrView(errorMsg));
	}

	Directory::create(outPath_);

	auto outFilename = Fmt("{}/{}.bin", outPath_, profile);
	File::writeFile(outFilename, Util::toSpan(byteCode), false);
}
}