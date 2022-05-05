#pragma once

#if SGE_RENDER_HAS_GL

#include <sge_render/Render_Common.h>
#include <sge_render/vertex/Vertex.h>

#if SGE_OS_WIN64
//#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GL/wglew.h>

#include <GL/gl.h>
#include <GL/glu.h>
#pragma comment(lib, "Opengl32.lib")
#pragma comment(lib, "Glu32.lib")
#endif // SGE_OS_WIN64


//GLenum sge::GLUtil::_glCheckError(const SrcLoc& loc_);

#if _DEBUG
#define glCheckError() GLUtil::_glCheckError(SGE_LOC)
#define glDebugCall(x) do { (x); glCheckError(); } while (false)
#else
#define glCheckError()
#define glDebugCall(x) (x);
#endif 

namespace sge {
struct GLUtil
{
	GLUtil() = delete;

	static GLenum _glCheckError(const SrcLoc& loc_);

	static GLsizei castGLsizei(size_t v_);

	static GLenum getGLPrimitiveTopology(RenderPrimitiveType t);
	static GLenum getGLFormat(RenderDataType v_);
	static i32 getGLFormatCount(RenderDataType v_);
};

inline GLenum GLUtil::_glCheckError(const SrcLoc& loc_)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		String error;
		switch (errorCode)
		{
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		SGE_LOG("line: {} func: {} file: {}", loc_.line, loc_.func, loc_.file);
		//throw SGE_ERROR("");
	}
	return errorCode;
}

inline GLsizei GLUtil::castGLsizei(size_t v_) { SGE_ASSERT(v_ < LONG_MAX); return static_cast<GLsizei>(v_); }

inline GLenum GLUtil::getGLPrimitiveTopology(RenderPrimitiveType t)
{
	using SRC = RenderPrimitiveType;
	switch (t) {
		case SRC::Points:		return GL_POINTS;
		case SRC::Lines:		return GL_LINES;
		case SRC::Triangles:	return GL_TRIANGLES;
		default: throw SGE_ERROR("unknown RenderPrimitiveType");
	}
}

inline GLenum GLUtil::getGLFormat(RenderDataType v_)
{
	using SRC = RenderDataType;
	switch (v_) {
		case SRC::Int32:		return GL_INT; break;
		case SRC::Int32x2:		return GL_INT; break;
		case SRC::Int32x3:		return GL_INT; break;
		case SRC::Int32x4:		return GL_INT; break;
			//--
		case SRC::UNorm8:		return GL_UNSIGNED_BYTE; break;
		case SRC::UNorm8x2:		return GL_UNSIGNED_BYTE; break;
		case SRC::UNorm8x3:		return GL_UNSIGNED_BYTE; break;
		case SRC::UNorm8x4:		return GL_UNSIGNED_BYTE; break;
			//--
		case SRC::Float16:		return GL_HALF_FLOAT; break;
		case SRC::Float16x2:	return GL_HALF_FLOAT; break;
		case SRC::Float16x3:	return GL_HALF_FLOAT; break;
		case SRC::Float16x4:	return GL_HALF_FLOAT; break;
			//---
		case SRC::Float32:		return GL_FLOAT; break;
		case SRC::Float32x2:	return GL_FLOAT; break;
		case SRC::Float32x3:	return GL_FLOAT; break;
		case SRC::Float32x4:	return GL_FLOAT; break;
		//---
		default: throw SGE_ERROR("unsupported RenderDataType");
	}
}

inline i32 GLUtil::getGLFormatCount(RenderDataType v_)
{
	using SRC = RenderDataType;
	switch (v_)
	{
		case SRC::Int8:			return 1; break;
		case SRC::Int8x2:		return 2; break;
		case SRC::Int8x3:		return 3; break;
		case SRC::Int8x4:		return 4; break;
			//--
		case SRC::Int32:		return 1; break;
		case SRC::Int32x2:		return 2; break;
		case SRC::Int32x3:		return 3; break;
		case SRC::Int32x4:		return 4; break;
			//--
		case SRC::UInt8:		return 1; break;
		case SRC::UInt8x2:		return 2; break;
		case SRC::UInt8x3:		return 3; break;
		case SRC::UInt8x4:		return 4; break;
			//--
		case SRC::UNorm8:		return 1; break;
		case SRC::UNorm8x2:		return 2; break;
		case SRC::UNorm8x3:		return 3; break; //does not support in DX11
		case SRC::UNorm8x4:		return 4; break;
			//--
		case SRC::Float16:		return 1; break;
		case SRC::Float16x2:	return 2; break;
		case SRC::Float16x3:	return 3; break; //does not support in DX11
		case SRC::Float16x4:	return 4; break;
			//---
		case SRC::Float32:		return 1; break;
		case SRC::Float32x2:	return 2; break;
		case SRC::Float32x3:	return 3; break;
		case SRC::Float32x4:	return 4; break;
			//---
		default: throw SGE_ERROR("unsupported RenderDataType");
	}
}

}


#endif