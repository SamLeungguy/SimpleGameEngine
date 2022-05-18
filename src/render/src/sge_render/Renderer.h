#pragma once

#include "Render_Common.h"

namespace sge {

class RenderContext;
class RenderGpuBuffer;
class RenderShader;
class Material;

struct RenderContext_CreateDesc;
struct RenderGpuBuffer_CreateDesc;
struct RenderShader_CreateDesc;
struct Material_CreateDesc;

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
	static Renderer* current();

	Renderer();
	virtual ~Renderer();

	VertexLayout* createVertexLayout();

	const RenderAdapterInfo& adapterInfo() const;

	bool isVsync() const;
	
	RenderContext* createContext(RenderContext_CreateDesc& desc_)		{ return onCreateContext(desc_); }
	RenderGpuBuffer* createGpuBuffer(RenderGpuBuffer_CreateDesc& desc_)	{ return onCreateGpuBuffer(desc_); }
	RenderShader* createShader(RenderShader_CreateDesc& desc_)			{ return onCreateShader(desc_); }
	Material* createMaterial(Material_CreateDesc& desc_)				{ return onCreateMaterial(desc_); }

protected:
	virtual RenderContext* onCreateContext(RenderContext_CreateDesc& desc) = 0;
	virtual RenderGpuBuffer* onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) = 0;
	virtual RenderShader* onCreateShader(RenderShader_CreateDesc& desc_) = 0;
	virtual Material* onCreateMaterial(Material_CreateDesc& desc_) = 0;

protected:
	static Renderer* _pCurrent;
	RenderAdapterInfo _adapterInfo;
	bool _isVsync : 1;

};

inline Renderer* Renderer::current() { return _pCurrent; }

inline const RenderAdapterInfo& Renderer::adapterInfo() const { return _adapterInfo; }

inline bool Renderer::isVsync() const { return _isVsync; }


}