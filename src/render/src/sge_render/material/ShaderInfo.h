#pragma once

#include "../vertex/Vertex.h"
#include "../buffer/RenderGpuBuffer.h"

namespace sge {

struct InputAttribute
{
	//String name;
	Vertex_Semantic semantic	= Vertex_Semantic::None;
	RenderDataType dataType		= RenderDataType::None;

	String semanticStr;
};

class CBufferLayout : public Object
{
public:
	struct Element
	{
		String			name;
		size_t			hash;

		RenderDataType	dataType;
		u32				offset = 0;
		u32				size = 0;
	};
	//VectorMap<size_t, Element> elements;
	Vector<Element> elements;
	u32 dataSize = 0;

	// should seperate to CBuffer
	u32 bindPoint = 0;
	u32 bindCount = 0;

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
		
		String shaderFuncs[enumInt(RenderShaderType::Count)]/* = {"vs_main",  "ps_main", "", "", ""}*/;

		//String vsFunc	= "vs_main";
		//String psFunc	= "ps_main";
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

	u32 _passIndex = 0;
	RenderShaderType _type;

	String _version;
	Tuple3u _csWorkGroupSize = {0, 0, 0};

	SPtr<ShaderParsedInfo>		_spShaderParsedInfo;

	Vector<InputAttribute>		_inputs;

	Vector<SPtr<CBufferLayout>> _cBufferLayoutSPtrs;		// cBuffer count
	Vector<Vector<u8>>			_cBufferData;

	Vector<SPtr<RenderGpuBuffer>> _cBufferSPtrs;
	Vector<bool> _isDirties;

	// textures
	// samplers

	ShaderInfo(ShaderInfo&& info_) {}
	void operator=(ShaderInfo&& info_) {}

private:

};


}