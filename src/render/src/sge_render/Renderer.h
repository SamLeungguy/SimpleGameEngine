#pragma once

#include "Render_Common.h"
#include "command/RenderCommands.h"

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
	static Renderer* current();

	Renderer();
	virtual ~Renderer();

	VertexLayout* createVertexLayout();

	const RenderAdapterInfo& adapterInfo() const;

	bool isVsync() const;
	
	RenderContext* createContext(RenderContext_CreateDesc& desc)		{ return onCreateContext(desc); }
	RenderGpuBuffer* createGpuBuffer(RenderGpuBuffer_CreateDesc& desc)	{ return onCreateGpuBuffer(desc); }

protected:
	virtual RenderContext* onCreateContext(RenderContext_CreateDesc& desc) = 0;
	virtual RenderGpuBuffer* onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) = 0;

protected:
	static Renderer* _pCurrent;
	RenderAdapterInfo _adapterInfo;
	bool _isVsync : 1;

};

inline Renderer* Renderer::current() { return _pCurrent; }

inline const RenderAdapterInfo& Renderer::adapterInfo() const { return _adapterInfo; }

inline bool Renderer::isVsync() const { return _isVsync; }


}