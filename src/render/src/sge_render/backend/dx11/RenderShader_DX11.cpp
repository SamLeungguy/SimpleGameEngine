#if SGE_RENDER_HAS_DX11

#include "RenderShader_DX11.h"
#include "Renderer_DX11.h"
#include "RenderGpuBuffer_DX11.h"

#include <sge_render/vertex/VertexLayoutManager.h>

#include <nlohmann/json.hpp>
#include <d3d11shader.h>

namespace sge {
// RenderShader_DX11_Helper
#if 1

struct RenderShader_DX11_Helper
{
private:
	using Helper = RenderShader_DX11_Helper;
public:
	using json = nlohmann::ordered_json;
	using Util = DX11Util;
	void init(RenderShader_DX11& renderShader_);

	void loadVertexShader(SPtr<ShaderInfo>& spShaderInfo_, ComPtr<DX11_ID3DVertexShader>& cpVertexShader_, StrView filename_, int passIndex_, SPtr<ShaderParsedInfo>& spShaderParsedInfo_);
	void loadPixelShader(SPtr<ShaderInfo>& spShaderInfo_, ComPtr<DX11_ID3DPixelShader>& cpPixelShader_, StrView filename_, int passIndex_, SPtr<ShaderParsedInfo>& spShaderParsedInfo_);

	void deserialize(SPtr<ShaderInfo>& spShaderInfo_, StrView filepath_, SPtr<ShaderParsedInfo>& spShaderParsedInfo_, RenderShaderType type_);

	static void getShaderFilePath(TempString& out_, RenderShaderType type_, StrView filename_, StrView extension_, int passIndex_);
	static std::string getJsonStr(json& j_, const char* name_);

private:
	void deserialize_inputs(SPtr<ShaderInfo>& spShaderInfo_, json& js_);
	void deserialize_params(SPtr<ShaderInfo>& spShaderInfo_, json& js_);
	void deserialize_cBuffers(SPtr<ShaderInfo>& spShaderInfo_, json& js_);
	void deserialize_textures(SPtr<ShaderInfo>& spShaderInfo_, json& js_);
	void deserialize_samplers(SPtr<ShaderInfo>& spShaderInfo_, json& js_);
	void deserialize_storageBuffers(SPtr<ShaderInfo>& spShaderInfo_, json& js_);

	static void _addInputAttrCount(Map<size_t, u8>& bucket_, const char* attrCStr_);
	static const VertexLayout* _makeVertexLayout(Map<size_t, u8>& bucket_);
public:
	TempString tmpStr;
	Renderer_DX11* pRenderer = Renderer_DX11::current();
private:
	RenderShader_DX11* pRenderShader = nullptr;
};

std::string RenderShader_DX11_Helper::getJsonStr(json& j_, const char* name_)
{
	return j_.at(name_).get<std::string>();
}

void RenderShader_DX11_Helper::init(RenderShader_DX11& renderShader_)
{
	pRenderShader = &renderShader_;
}

void RenderShader_DX11_Helper::loadVertexShader(SPtr<ShaderInfo>& spShaderInfo_, ComPtr<DX11_ID3DVertexShader>& cpVertexShader_, StrView filename_, int passIndex_, SPtr<ShaderParsedInfo>& spShaderParsedInfo_)
{
	auto& spShaderInfo = spShaderInfo_;
	getShaderFilePath(tmpStr, RenderShaderType::Vertex, filename_, ".bin", passIndex_);

	MemMapFile mm;
	mm.openRead(tmpStr.c_str());

	auto* dev = pRenderer->d3dDevice();

	auto hr = dev->CreateVertexShader(mm.data(), mm.size(), nullptr, cpVertexShader_.ptrForInit());
	Util::throwIfError(hr);
	pRenderShader->_vertexSrcCode.resize(mm.size());
	memcpy(reinterpret_cast<void*>(const_cast<u8*>(pRenderShader->_vertexSrcCode.data())), mm.data(), mm.size());

	getShaderFilePath(tmpStr, RenderShaderType::Vertex, filename_, ".json", passIndex_);
	deserialize(spShaderInfo, tmpStr.c_str(), spShaderParsedInfo_, RenderShaderType::Vertex);
}

void RenderShader_DX11_Helper::loadPixelShader(SPtr<ShaderInfo>& spShaderInfo_, ComPtr<DX11_ID3DPixelShader>& cpPixelShader_, StrView filename_, int passIndex_, SPtr<ShaderParsedInfo>& spShaderParsedInfo_)
{
	auto& spShaderInfo = spShaderInfo_;
	getShaderFilePath(tmpStr, RenderShaderType::Pixel, filename_, ".bin", passIndex_);

	MemMapFile mm;
	mm.openRead(tmpStr.c_str());

	auto* dev = pRenderer->d3dDevice();

	auto hr = dev->CreatePixelShader(mm.data(), mm.size(), nullptr, cpPixelShader_.ptrForInit());
	Util::throwIfError(hr);

	getShaderFilePath(tmpStr, RenderShaderType::Pixel, filename_, ".json", passIndex_);
	deserialize(spShaderInfo, tmpStr.c_str(), spShaderParsedInfo_, RenderShaderType::Pixel);
}

void RenderShader_DX11_Helper::deserialize(SPtr<ShaderInfo>& spShaderInfo_, StrView filepath_, SPtr<ShaderParsedInfo>& spShaderParsedInfo_, RenderShaderType type_)
{
	MemMapFile mm;
	mm.openRead(filepath_.data());

	auto& spShaderInfo = spShaderInfo_;
	spShaderInfo->_spShaderParsedInfo.reset(spShaderParsedInfo_);

	auto js = json::parse(mm.data());
	spShaderInfo->_type = type_;
	spShaderInfo->_version = Helper::getJsonStr(js, "profile").c_str();
	auto workGroup = js.at("csWorkgroupSize").get<std::vector<u32>>();
	spShaderInfo->_csWorkGroupSize.x = workGroup[0];	spShaderInfo->_csWorkGroupSize.y = workGroup[1];	spShaderInfo->_csWorkGroupSize.z = workGroup[2];

	deserialize_inputs(spShaderInfo, js);
	deserialize_params(spShaderInfo, js);
	deserialize_cBuffers(spShaderInfo, js);
	deserialize_textures(spShaderInfo, js);
	deserialize_samplers(spShaderInfo, js);
	deserialize_storageBuffers(spShaderInfo, js);
}

void RenderShader_DX11_Helper::deserialize_inputs(SPtr<ShaderInfo>& spShaderInfo_, json& js_)
{
	auto& spShaderInfo = spShaderInfo_;
	Map<size_t, u8> inputAttrBucket;

	auto& j_inputs = js_.at("inputs");
	spShaderInfo->_inputs.reserve(j_inputs.size());

	for (auto& j : j_inputs/*= j_inputs.begin(); j < j_inputs.end(); j++*/)
	{
		auto& input = spShaderInfo->_inputs.emplace_back();

		tmpStr.clear();
		auto tmpStdStr = Helper::getJsonStr(j, "attrId");
		Vertex_SemanticType semanticType = Vertex_SemanticType::None;
		Vertex_SemanticUtil::Index semanticIndex = 0;

		if (!StringUtil::hasChar(tmpStdStr.c_str(), '_'))
		{
			auto pair = StringUtil::splitByChar_Inclusive(tmpStdStr.c_str(), "0123456789");
			tmpStr.append(pair.first.begin(), pair.first.end());
			
			_addInputAttrCount(inputAttrBucket, tmpStr.c_str());

			semanticType = Util::getSemanticType(tmpStr.c_str());
			tmpStr.clear();
			tmpStr.append(pair.second.begin(), pair.second.end());
			StringUtil::tryParse(tmpStr, semanticIndex);
		}
		else	// TODO: temp, should handle SV_
		{
			semanticType = Vertex_SemanticType::Pos;
		}

		input.semantic = Vertex_SemanticUtil::make(semanticType, semanticIndex);
		input.dataType = RenderDataTypeUtil::getRenderDataType(Helper::getJsonStr(j, "dataType").c_str());
	}

	pRenderShader->getInputLayout(_makeVertexLayout(inputAttrBucket));
}

void RenderShader_DX11_Helper::deserialize_params(SPtr<ShaderInfo>& spShaderInfo_, json& js_)
{
	auto& j_params = js_.at("params");
	for (auto& j_param : j_params)
	{
		(void)j_param;
	}
}

void RenderShader_DX11_Helper::deserialize_cBuffers(SPtr<ShaderInfo>& spShaderInfo_, json& js_)
{
	auto& spShaderInfo = spShaderInfo_;

	auto& j_cbs = js_.at("cBuffers");
	spShaderInfo->_cBufferLayoutSPtrs.reserve(j_cbs.size());
	spShaderInfo->_cBufferSPtrs.reserve(j_cbs.size());
	spShaderInfo->_cBufferData.reserve(j_cbs.size());
	spShaderInfo->_isDirties.reserve(j_cbs.size());

	int i = 0;
	for (auto& j_cb : j_cbs)
	{
		if (j_cb.empty())
			continue;

		auto& spLayout = spShaderInfo->_cBufferLayoutSPtrs.emplace_back(new CBufferLayout());
		spLayout->bindPoint = j_cb.at("bindPoint").get<u32>();
		spLayout->bindCount = j_cb.at("bindCount").get<u32>();
		spLayout->dataSize = j_cb.at("dataSize").get<u32>();

		spLayout->elements.reserve(j_cb.at("variableCount").get<u32>());
		spShaderInfo->_cBufferData.emplace_back().resize(spLayout->dataSize);
		spShaderInfo->_isDirties.emplace_back();

		for (auto& j_cbVar : j_cb.at("variables"))
		{
			auto& element = spLayout->elements.emplace_back();
			element.name = getJsonStr(j_cbVar, "name").c_str();
			element.hash = Math::hashStr(element.name);
			element.offset = j_cbVar.at("offset").get<u32>();
			element.dataType = RenderDataTypeUtil::getRenderDataType(getJsonStr(j_cbVar, "dataType").c_str());
			element.size = j_cbVar.at("size").get<u32>();
		}

		auto& spCBuffer = spShaderInfo->_cBufferSPtrs.emplace_back();
		RenderGpuBuffer_CreateDesc desc;
		desc.type = RenderGpuBufferType::Const;
		desc.bufferSize = spLayout->dataSize;
		spCBuffer = pRenderer->createGpuBuffer(desc);

		++i;
	}

	SGE_ASSERT(spShaderInfo->_cBufferSPtrs.size() == spShaderInfo->_isDirties.size() && spShaderInfo->_cBufferSPtrs.size() == spShaderInfo->_cBufferData.size());
}

void RenderShader_DX11_Helper::deserialize_textures(SPtr<ShaderInfo>& spShaderInfo_, json& js_)
{
	//auto& spShaderInfo = spShaderInfo_;
	//auto* pRenderer = Renderer::current();

	auto& j_texs = js_.at("textures");
	//spShaderInfo->text.reserve(j_texs.size());
	//spShaderInfo->_cBufferSPtrs.reserve(j_texs.size());
	for (auto& j_tex : j_texs)
	{
		(void)j_tex;
	}
}

void RenderShader_DX11_Helper::deserialize_samplers(SPtr<ShaderInfo>& spShaderInfo_, json& js_)
{
	//auto& spShaderInfo = spShaderInfo_;

	auto& j_samplers = js_.at("samplers");
	//spShaderInfo->text.reserve(j_texs.size());
	//spShaderInfo->_cBufferSPtrs.reserve(j_texs.size());
	for (auto& j_sampler : j_samplers)
	{
		(void)j_sampler;
	}
}

void RenderShader_DX11_Helper::deserialize_storageBuffers(SPtr<ShaderInfo>& spShaderInfo_, json& js_)
{
	//auto& spShaderInfo = spShaderInfo_;

	auto& j_ssbos = js_.at("storageBuffers");
	//spShaderInfo->text.reserve(j_texs.size());
	//spShaderInfo->_cBufferSPtrs.reserve(j_texs.size());
	for (auto& j_ssbo : j_ssbos)
	{
		(void)j_ssbo;
	}
}

void RenderShader_DX11_Helper::getShaderFilePath(TempString& out_, RenderShaderType type_, StrView filename_, StrView extension_, int passIndex_)
{
	static constexpr char* s_shaderTypeStrs[] = { "vs", "ps", "gs", "ts", "cs" };

	out_.clear();
	out_.append("LocalTemp/Imported/");
	out_.append(filename_.data());		// TODO: remove tmp
	out_.append("/dx11/pass");	// 0_vs.bin
	out_.append(std::to_string(passIndex_).c_str());
	out_.append("_");
	out_.append(s_shaderTypeStrs[enumInt(type_)]);
	out_.append(extension_.data());
}

void RenderShader_DX11_Helper::_addInputAttrCount(Map<size_t, u8>& bucket_, const char* attrCStr_)
{
	auto hashValue = Math::hashCStr(attrCStr_);
	auto it = bucket_.find(hashValue);
	it != bucket_.end() ? it->second++ : bucket_[hashValue] = 1;
}

const VertexLayout* RenderShader_DX11_Helper::_makeVertexLayout(Map<size_t, u8>& bucket_)
{
	static const size_t compares[] = { Math::hashStr("POSITION"), Math::hashStr("SV_POSITION"), Math::hashStr("TEXCOORD"), Math::hashStr("COLOR")
								, Math::hashStr("NORMAL"), Math::hashStr("TANGENT"), Math::hashStr("BINORMAL") };

	auto it = bucket_.find(compares[2]);
	u8 uvCount = it != bucket_.end() ? it->second : 0;

	it = bucket_.find(compares[3]);
	u8 colorCount = it != bucket_.end() ? it->second : 0;
	
	it = bucket_.find(compares[4]);
	u8 normalCount = it != bucket_.end() ? it->second : 0;
	
	it = bucket_.find(compares[5]);
	u8 tangentCount = it != bucket_.end() ? it->second : 0;

	it = bucket_.find(compares[6]);
	u8 binormalCount = it != bucket_.end() ? it->second : 0;

	auto vertexType = VertexTypeUtil::make(
		RenderDataTypeUtil::get<Tuple3f>(),
		RenderDataTypeUtil::get<Color4b>(), colorCount,
		RenderDataTypeUtil::get<Tuple2f>(), uvCount,
		RenderDataTypeUtil::get<Tuple3f>(), normalCount, tangentCount, binormalCount);

	return VertexLayoutManager::current()->getLayout(vertexType);
}


#endif // RenderShader_DX11_Helper

RenderShader_DX11::RenderShader_DX11(CreateDesc& desc_)
	:
	Base(desc_)
{
	auto passIndex = desc_.passIndex;
	auto& spShaderParsedInfo = desc_.spShaderParsedInfo;
	auto& spPassInfo = spShaderParsedInfo->passInfoSPtrs[passIndex];

	u32 typeCount = 0;
	if (spPassInfo->shaderFuncs[enumInt(RenderShaderType::Vertex)].size() > 0)			typeCount++;
	if (spPassInfo->shaderFuncs[enumInt(RenderShaderType::Pixel)].size() > 0)			typeCount++;
	if (spPassInfo->shaderFuncs[enumInt(RenderShaderType::Gemotry)].size() > 0)			typeCount++;
	if (spPassInfo->shaderFuncs[enumInt(RenderShaderType::Tessellation)].size() > 0)	typeCount++;
	if (spPassInfo->shaderFuncs[enumInt(RenderShaderType::Compute)].size() > 0)			typeCount++;

	_shaderInfoSPtrs.reserve(typeCount);

	RenderShader_DX11_Helper helper;
	helper.init(*this);

	auto* pSpShaderInfo = &_shaderInfoSPtrs.emplace_back(new ShaderInfo());
	helper.loadVertexShader(*pSpShaderInfo, _cpVertexShader, _filename, passIndex, spShaderParsedInfo);

	pSpShaderInfo = &_shaderInfoSPtrs.emplace_back(new ShaderInfo());
	helper.loadPixelShader(*pSpShaderInfo, _cpPixelShader, _filename, passIndex, spShaderParsedInfo);
}

void RenderShader_DX11::destroy()
{
	if (_cpVertexShader)	_cpVertexShader.reset(nullptr);
	if (_cpPixelShader)		_cpPixelShader.reset(nullptr);
	_inputLayouts.clear();
}

void RenderShader_DX11::bind()
{
	auto* ctx = Renderer_DX11::current()->d3dDeviceContext();
	ctx->VSSetShader(_cpVertexShader, 0, 0);
	ctx->PSSetShader(_cpPixelShader, 0, 0);
}

void RenderShader_DX11::uploadCBuffers(RenderShaderType type_)
{
	auto* ctx = Renderer_DX11::current()->d3dDeviceContext();

	auto& shaderInfoSPtrs	= _shaderInfoSPtrs;
	auto& cBufferLayoutPtrs	= shaderInfoSPtrs[enumInt(type_)]->_cBufferLayoutSPtrs;
	auto& cBufferPtrs		= shaderInfoSPtrs[enumInt(type_)]->_cBufferSPtrs;
	auto& cBufferData		= shaderInfoSPtrs[enumInt(type_)]->_cBufferData;
	auto& isDirties			= shaderInfoSPtrs[enumInt(type_)]->_isDirties;

	Vector_<DX11_ID3DBuffer*, 15> cBufferPtrs_dx;

	SGE_ASSERT(cBufferPtrs.size() == isDirties.size() && cBufferPtrs.size() == cBufferData.size());

	for (size_t i = 0; i < isDirties.size(); i++)
	{
		if (!isDirties[i])
			continue;

		auto* pCBuffer = static_cast<RenderGpuBuffer_DX11*>(cBufferPtrs[i].ptr());
		if (!pCBuffer) { SGE_ASSERT(0); return; }

		Span<u8> span = Span<u8>(cBufferData[i].data(), cBufferData[i].size());

		pCBuffer->uploadToGpu(span);
		cBufferPtrs_dx.emplace_back(pCBuffer->getBuffer());
		isDirties[i] = false;

		auto bindPoint = cBufferLayoutPtrs[i]->bindPoint;
		DX11_ID3DBuffer* ppCBuffers[] = { pCBuffer->getBuffer() };

		switch (type_)
		{
			using Type = RenderShaderType;
			case Type::Vertex:			ctx->VSSetConstantBuffers(bindPoint, 1, ppCBuffers);	break;
			case Type::Pixel:			ctx->PSSetConstantBuffers(bindPoint, 1, ppCBuffers);	break;
			//case Type::Gemotry:			ctx->GSSetConstantBuffers(bindPoint,  count, cBufferPtrs_dx.data());	break;
			//case Type::Tessellation:		ctx->TSSetConstantBuffers(bindPoint, count, cBufferPtrs_dx.data());		break;
			//case Type::Compute:			ctx->CSSetConstantBuffers(bindPoint, count, cBufferPtrs_dx.data());		break;
			default:	throw SGE_ERROR("invalid type");	break;
		}

		//SGE_DUMP_VAR(*reinterpret_cast<float*>(cBufferData[i].data()), cBufferData[i].size());
	}

	if (cBufferPtrs_dx.size() <= 0)
		return;

	//UINT count = Util::castUINT(cBufferPtrs_dx.size());
}

void RenderShader_DX11::reload()
{
	destroy();
}

DX11_ID3DInputLayout* RenderShader_DX11::getInputLayout(const VertexLayout* src)		// TODO: remove temp
{
	if (!src)
		return nullptr;

	auto it = _inputLayouts.find(src);
	if (it != _inputLayouts.end()) {
		return it->second;
	}

	Vector_<D3D11_INPUT_ELEMENT_DESC, 32> inputDesc;

	for (auto& e : src->elements) {
		auto& dst = inputDesc.emplace_back();
		auto semanticType = Vertex_SemanticUtil::getType(e.semantic);
		dst.SemanticName = Util::getDxSemanticName(semanticType);
		dst.SemanticIndex = Vertex_SemanticUtil::getIndex(e.semantic);
		dst.Format = Util::getDxFormat(e.dataType);
		dst.InputSlot = 0;
		dst.AlignedByteOffset = e.offset;
		dst.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		dst.InstanceDataStepRate = 0;
	}

	ComPtr<DX11_ID3DInputLayout>	outLayout;

	auto* dev = Renderer_DX11::current()->d3dDevice();

	auto hr = dev->CreateInputLayout(inputDesc.data(),
		static_cast<UINT>(inputDesc.size()),
		_vertexSrcCode.data(),
		_vertexSrcCode.size(),
		outLayout.ptrForInit());
	Util::throwIfError(hr);

	_inputLayouts[src] = outLayout;
	return outLayout;
}

}

#endif