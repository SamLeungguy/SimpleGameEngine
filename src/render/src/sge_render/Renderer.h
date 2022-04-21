#pragma once

#include "Render_Common.h"
#include "RenderCommandBuffer.h"

namespace sge {

class RenderContext;
struct RenderContext_CreateDesc;

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

	const RenderAdapterInfo& adapterInfo() const;

	RenderCommandBuffer* getRenderCommnadBuffer();

	bool isVsync() const;

	virtual RenderContext* onCreateContext(RenderContext_CreateDesc& desc) = 0;

protected:
	static Renderer* _pCurrent;
	RenderAdapterInfo _adapterInfo;
	bool _isVsync : 1;

	RenderCommandBuffer* _pRenderCommandBuffer = nullptr;
};

inline Renderer* Renderer::current() { return _pCurrent; }

inline const RenderAdapterInfo& Renderer::adapterInfo() const { return _adapterInfo; }

inline RenderCommandBuffer* Renderer::getRenderCommnadBuffer() { return _pRenderCommandBuffer; }

inline bool Renderer::isVsync() const { return _isVsync; }


}