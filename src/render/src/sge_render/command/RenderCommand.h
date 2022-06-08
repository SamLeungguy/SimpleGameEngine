#pragma once

#include "../vertex/Vertex.h"
#include "../buffer/RenderGpuBuffer.h"
#include "../shader/Material.h"

#include <sge_core/allocator/LinearAllocator.h>

namespace sge {

class RenderMesh;
class RenderSubMesh;

enum class RenderCommandType {
	None,
	ClearFrameBuffers,
	SwapBuffers,
	DrawCall,
};

class RenderCommand : NonCopyable {
public:
	using Type = RenderCommandType;

	RenderCommand(Type type) : _type(type) {}

	virtual ~RenderCommand() {}

	Type type() const { return _type; }

#if _DEBUG
	SrcLoc	debugLoc;
#endif

private:
	Type _type = Type::None;
};

class RenderCommand_ClearFrameBuffers : public RenderCommand {
	using Base = RenderCommand;
	using This = RenderCommand_ClearFrameBuffers;
public:
	RenderCommand_ClearFrameBuffers() : Base(Type::ClearFrameBuffers) {}
	virtual ~RenderCommand_ClearFrameBuffers() = default;

	This& setColor(const Color4f& color_) { color = color_; return *this; }
	This& setDepth(float depth_) { depth = depth_; return *this; }
	This& dontClearColor() { color.reset(); return *this; }
	This& dontClearDepth() { depth.reset(); return *this; }

	Opt<Color4f> color = Color4f(1, 1, 1, 1);
	Opt<float>   depth = 0;
};

class RenderCommand_SwapBuffers : public RenderCommand {
	using Base = RenderCommand;
public:
	RenderCommand_SwapBuffers() : Base(Type::SwapBuffers) {}
	virtual ~RenderCommand_SwapBuffers() = default;
};

class RenderCommand_DrawCall : public RenderCommand {
	using Base = RenderCommand;
public:
	RenderCommand_DrawCall() : Base(Type::DrawCall) {}
	virtual ~RenderCommand_DrawCall() = default;

	RenderPrimitiveType		primitive = RenderPrimitiveType::None;
	const VertexLayout* pVertexLayout = nullptr;
	RenderDataType			indexType = RenderDataType::UInt16;

	SPtr<RenderGpuBuffer>	spVertexBuffer;
	SPtr<RenderGpuBuffer>	spIndexBuffer;

	size_t vertexCount = 0;
	size_t indexCount = 0;

	SPtr<Shader>			spShader;
};

class RenderCommandBuffer : public NonCopyable {
public:
	RenderCommandBuffer();
	~RenderCommandBuffer() = default;

	RenderCommand_ClearFrameBuffers* clearFrameBuffers() { return newCommand<RenderCommand_ClearFrameBuffers>(); }
	RenderCommand_SwapBuffers* swapBuffers() { return newCommand<RenderCommand_SwapBuffers>(); }

	void drawMesh(const SrcLoc& debugLoc_, const RenderMesh& mesh_);
	void drawSubMesh(const SrcLoc& debugLoc_, const RenderSubMesh& subMesh_);

	void test_drawMesh(const SrcLoc& debugLoc_, const RenderMesh& mesh_, SPtr<Material>& spMaterial_);

	void reset();

	Span<RenderCommand*>	commands() { return _commands; }

	template<class CMD>
	CMD* newCommand() {
		auto* buf = _allocator.allocate(sizeof(CMD));
		auto* cmd = new(buf) CMD();
		_commands.emplace_back(cmd);
		return cmd;
	}

private:
	Vector_<RenderCommand*, 64>	_commands;

	LinearAllocator _allocator;
};

}