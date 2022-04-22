#pragma once

#include "Render_Common.h"
#include "RenderCommandBuffer.h"

namespace sge {

class RenderContext;
class VertexBuffer;
class IndexBuffer;
class Shader;

struct RenderContext_CreateDesc;
struct VertexBuffer_CreateDesc;
struct IndexBuffer_CreateDesc;
struct Shader_CreateDesc;

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

	RenderCommandBuffer* getRenderCommnadBuffer();

	bool isVsync() const;
	
	virtual RenderContext* onCreateContext(RenderContext_CreateDesc& desc) = 0;
	virtual VertexBuffer* onCreateVertexBuffer(VertexBuffer_CreateDesc& desc_) = 0;
	virtual IndexBuffer* onCreateIndexBuffer(IndexBuffer_CreateDesc& desc_) = 0;
	virtual Shader* onCreateShader(Shader_CreateDesc& desc_) = 0;

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