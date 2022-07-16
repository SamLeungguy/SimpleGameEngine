#pragma once

#include "Render_Common.h"
#include "RenderContext.h"
#include "shader/Material.h"
#include "textures/Texture.h"

namespace sge {

class RenderContext;
class RenderGpuBuffer;

struct RenderContext_CreateDesc;
struct RenderGpuBuffer_CreateDesc;

struct VertexLayout;

class Renderer : public NonCopyable
{
public:
	enum class ApiType 
	{
		None = 0,
		DX11,
		OpenGL,
	};

	struct CreateDesc
	{
		CreateDesc();
		ApiType apiType;
		bool isMultithread : 1;
	};

	static Renderer* create(CreateDesc& desc_);
	static Renderer* instance();

	Renderer();
	virtual ~Renderer();

	VertexLayout* createVertexLayout();

	const RenderAdapterInfo& adapterInfo() const;

	bool isVsync() const;
	
	SPtr<RenderContext>		createContext	(RenderContext_CreateDesc& desc_);
	SPtr<RenderGpuBuffer>	createGpuBuffer	(RenderGpuBuffer_CreateDesc& desc_);
	SPtr<Texture2D>			createTexture2D	(Texture2D_CreateDesc& desc_);
	SPtr<Shader>			createShader	(StrView filename_);
	SPtr<Material>			createMaterial	();

	void onShaderDestory(Shader* pShader_);

	struct StockTextures {
		SPtr<Texture2D>	white;
		SPtr<Texture2D>	black;
		SPtr<Texture2D>	red;
		SPtr<Texture2D>	green;
		SPtr<Texture2D>	blue;
		SPtr<Texture2D>	magenta;
		SPtr<Texture2D>	error;
	};

	StockTextures stockTextures;

	SPtr<Texture2D>	createSolidColorTexture2D(const Color4b& color_);

protected:
	virtual SPtr<RenderContext>		onCreateContext		(RenderContext_CreateDesc&		desc_)	= 0;
	virtual SPtr<RenderGpuBuffer>	onCreateGpuBuffer	(RenderGpuBuffer_CreateDesc&	desc_)	= 0;
	virtual SPtr<Texture2D>			onCreateTexture2D	(Texture2D_CreateDesc&			desc_)	= 0;
	virtual SPtr<Shader>			onCreateShader		(StrView filename_)						= 0;
	virtual SPtr<Material>			onCreateMaterial	()										= 0;

protected:
	static Renderer* s_pInstance;
	RenderAdapterInfo _adapterInfo;
	bool _isVsync : 1;

	StringMap<Shader*>	_shaders;
};

inline Renderer* Renderer::instance() { return s_pInstance; }

inline const RenderAdapterInfo& Renderer::adapterInfo() const { return _adapterInfo; }

inline bool Renderer::isVsync() const { return _isVsync; }

inline SPtr<RenderContext>		Renderer::createContext		(RenderContext_CreateDesc&		desc_)	{ return onCreateContext(desc_); }
inline SPtr<RenderGpuBuffer>	Renderer::createGpuBuffer	(RenderGpuBuffer_CreateDesc&	desc_)	{ return onCreateGpuBuffer(desc_); }
inline SPtr<Texture2D>			Renderer::createTexture2D	(Texture2D_CreateDesc&			desc_)	{ return onCreateTexture2D(desc_); }
inline SPtr<Material>			Renderer::createMaterial	()										{ return onCreateMaterial(); }

}