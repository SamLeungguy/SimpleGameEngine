#pragma once

#include "../vertex/Vertex.h"
#include "../buffer/RenderGpuBuffer.h"

namespace sge {

struct InputAttribute
{
	//String name;
	Vertex_Semantic semantic_type = Vertex_Semantic::None;
	RenderDataType data_type	  = RenderDataType::None;
};

class CBufferLayout : public Object
{
public:
	struct Element
	{
		String			name;
		size_t			hash;

		RenderDataType	data_type;
		u16				offset = 0;
		u16				size = 0;
	};
	//VectorMap<size_t, Element> elements;
	Vector<Element> elements;
	u32 stride = 0;

private:
};

struct ShaderParsedInfo : public Object			// one per whole shader
{
#if 0
	enum class PassTokenType
	{
		None = 0,
		Queue,
		Cull,
		BlendRGB,
		BlendAlpha,
		DepthTest,
		DepthWrite,
		VsFunc,
		PsFunc,
	};
	struct PassToken
	{
		PassTokenType type;
		String value;
	};
#endif // 0
	struct PassInfo : public Object
	{
		String name;
		String queue	= "Transparent";	// shd be qeue enum
		String cull		= "None";			// shd be cull enum

		String blendRGB;					// shd be blend enum
		String blendAlpha;					// shd be blend enum
		
		String depthTest;					// shd be depthTest enum
		bool depthWrite = false;			// shd be depthWrite enum

		String vsFunc	= "vs_main";
		String psFunc	= "ps_main";
	};

	String shaderName;
	u32 passCount = 0;

	SPtr<CBufferLayout> spPropertiesLayout;
	Vector<u8> propertiesDefaultData;

	SPtr<CBufferLayout> spPremutationLayout;
	Vector<u8> premutationData;

	//Vector<PassToken> passTokens;
	Vector<SPtr<PassInfo>> passInfoSPtrs;
};

class ShaderInfo : public Object		// one per vertex / pixel shader
{
public:
	ShaderInfo() = default;

	String _version;
	Tuple3u _csWorkGroupSize = {0, 0, 0};

	SPtr<ShaderParsedInfo>		_spShaderParsedInfo;

	Vector<InputAttribute>		_inputs;
	Vector<SPtr<CBufferLayout>> _cBufferLayoutSPtrs;		// cBuffer count
	Vector<Vector<u8>>			_cBufferDatas;

	Vector<SPtr<RenderGpuBuffer>> _cBufferSPtrs;
	Vector<bool> isDirties;

	// textures
	// samplers

	ShaderInfo(ShaderInfo&& info_) {}
	void operator=(ShaderInfo&& info_) {}

private:

};


}