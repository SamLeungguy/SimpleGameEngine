#if 0
#include "Material.h"
#include "../Renderer.h"

#include <nlohmann/json.hpp>
#include "..\..\Material.h"

namespace sge {

// Material_Helper 
#if 1
struct Material_Helper
{
private:
	using json = nlohmann::ordered_json;
public:
	void deserializeParsedInfo(SPtr<ShaderParsedInfo>& spShaderParsedInfo_, MemMapFile& mm_);

	void deserializeEachPass(Vector<ShaderPass>& passes_, SPtr<ShaderParsedInfo>& spShaderParsedInfo_, StrView filename_);
private:
	void deserializeParsedInfo_premutation();
	void deserializeParsedInfo_properties();
	void deserializeParsedInfo_passes();

	std::string getJsonStr(json& j_, const char* name_);

private:
	json _jp;
	SPtr<ShaderParsedInfo>* _pSpShaderParsedInfo = nullptr;
	MemMapFile* _pMm = nullptr;
};

std::string Material_Helper::getJsonStr(json& j_, const char* name_)
{
	return j_.at(name_).get<std::string>();
}

void Material_Helper::deserializeParsedInfo(SPtr<ShaderParsedInfo>& spShaderParsedInfo_, MemMapFile& mm_)
{
	_pSpShaderParsedInfo = &spShaderParsedInfo_;
	_pMm = &mm_;

	auto& spInfo = spShaderParsedInfo_;
	auto& mm = mm_;

	_jp = json::parse(mm.data());
	json& jat = _jp.at("shaderName");
	spInfo->shaderName.append(jat.get<std::string>().c_str());

	jat = _jp.at("passCount");
	spInfo->passCount = jat.get<int>();

	deserializeParsedInfo_premutation();
	deserializeParsedInfo_properties();
	deserializeParsedInfo_passes();
}

void Material_Helper::deserializeEachPass(Vector<ShaderPass>& passes_, SPtr<ShaderParsedInfo>& spShaderParsedInfo_, StrView filename_)
{
	SGE_ASSERT(spShaderParsedInfo_->passInfoSPtrs.size());
	passes_.reserve(spShaderParsedInfo_->passInfoSPtrs.size());
	auto* pRenderer = Renderer::current();
	for (int i = 0; i < spShaderParsedInfo_->passInfoSPtrs.size(); i++)
	{
		auto& shaderPass = passes_.emplace_back();
		RenderShader_CreateDesc desc;
		desc.filename = filename_;
		desc.spShaderParsedInfo.reset(spShaderParsedInfo_);
		desc.passIndex = i;
		shaderPass._spRenderShader.reset(pRenderer->createShader(desc));
	}
}

void Material_Helper::deserializeParsedInfo_premutation()
{
}

void Material_Helper::deserializeParsedInfo_properties()
{
	auto& spInfo = *_pSpShaderParsedInfo;
	//auto& mm = *_pMm;
	auto& jp = _jp;

	// parse Properties
	auto jat = jp.at("properties");
	auto dataSize = jat.at("dataSize").get<size_t>();
	auto varCount = jat.at("variableCount").get<size_t>();

	spInfo->spPropertiesLayout.reset(new CBufferLayout());
	spInfo->propertiesDefaultData.reserve(dataSize);
	spInfo->spPropertiesLayout->elements.reserve(varCount);

	auto& elements = spInfo->spPropertiesLayout->elements;
	auto& data = spInfo->propertiesDefaultData;

	size_t totalSize = 0;
	auto& j_variables = jat.at("variables");
	for (auto j = j_variables.begin(); j < j_variables.end(); j++)
	{
		auto& last = elements.emplace_back();
		last.name = getJsonStr(*j, "name").c_str();
		last.hash = Math::hashStr(last.name);
		last.offset = j->at("offset").get<u32>();
		last.dataType = RenderDataTypeUtil::getRenderDataType(getJsonStr(*j, "dataType").c_str());//RenderDataTypeUtil::get;
		last.size = j->at("size").get<u32>();

		totalSize += last.size;
	}

	data.resize(totalSize);
}

void Material_Helper::deserializeParsedInfo_passes()
{
	auto& spInfo = *_pSpShaderParsedInfo;
	auto& jp = _jp.at("pass").at("info");

	spInfo->passInfoSPtrs.reserve(spInfo->passCount);

	for (auto& j : jp)
	{
		auto& spPassInfo = spInfo->passInfoSPtrs.emplace_back(new ShaderParsedInfo::PassInfo());
		spPassInfo->queue = getJsonStr(j, "Queue").c_str();
		spPassInfo->cull = getJsonStr(j, "Cull").c_str();
		spPassInfo->blendRGB = getJsonStr(j, "BlendRGB").c_str();
		spPassInfo->blendAlpha = getJsonStr(j, "BlendAlpha").c_str();
		spPassInfo->depthTest = getJsonStr(j, "DepthTest").c_str();
		spPassInfo->depthWrite = j.at("DepthWrite").get<bool>();
		spPassInfo->shaderFuncs[enumInt(RenderShaderType::Vertex)] = getJsonStr(j, "VsFunc").c_str();
		spPassInfo->shaderFuncs[enumInt(RenderShaderType::Pixel)] = getJsonStr(j, "PsFunc").c_str();
	}
}

#endif // Material_Helper 

Material::Material(CreateDesc& desc_)
{
	MemMapFile mm;

	// shd check path first, if no then create and compile it, finally load it
	_filename = desc_.filename;

	TempString tmpStr;
	tmpStr.append("LocalTemp/Imported/");
	tmpStr.append(_filename.c_str());
	tmpStr.append("/shaderInfo.json");

	mm.openRead(tmpStr.c_str());

	_spShaderParsedInfo.reset(new ShaderParsedInfo());

	Material_Helper parseHelper;
	parseHelper.deserializeParsedInfo(_spShaderParsedInfo, mm);
	parseHelper.deserializeEachPass(_shaderPasses, _spShaderParsedInfo, _filename);
}

void Material::setFloat(StrView name_, float value)
{
	for (auto& pass : _shaderPasses)
	{
		auto& shaderInfoSPtrs = pass._spRenderShader->getShaderInfoSPtrs();
		for (auto& spShaderInfo : shaderInfoSPtrs)		// vs / ps / gs / ...
		{
			for (size_t iLayout = 0; iLayout < spShaderInfo->_cBufferLayoutSPtrs.size(); ++iLayout)
			{
				auto& spLayout = spShaderInfo->_cBufferLayoutSPtrs[iLayout];
				bool shouldEnd = false;
				for (auto& e : spLayout->elements)
				{
					if (e.hash != Math::hashStr(name_.data()))
						continue;

					*reinterpret_cast<float*>(spShaderInfo->_cBufferData[iLayout].data() + e.offset) = value;
					spShaderInfo->_isDirties[iLayout] = true;
					shouldEnd = true;

					//---------
					//float test_v = *reinterpret_cast<float*>(spShaderInfo->_cBufferData[iLayout].data() + e.offset);
					//SGE_DUMP_VAR(test_v);
					//break;
				}
				if (shouldEnd)
					break;
			}
		}
	}
}

Material::Material(Material_CreateDesc& desc_)
{
}

}
#endif // 0
