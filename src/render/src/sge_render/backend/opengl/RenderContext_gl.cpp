#if SGE_RENDER_HAS_GL

#include "RenderContext_gl.h"
#include "Renderer_gl.h"
#include "RenderGpuBuffer_gl.h"

namespace sge {

#if SGE_OS_WINDOWS

class OpenGLWindow_FalseContext_Win32 : public NonCopyable {
public:

	~OpenGLWindow_FalseContext_Win32() {
		if (_rc)	{ ::wglDeleteContext(_rc);		_rc = nullptr; }
		if (_dc)	{ ::ReleaseDC(_hwnd, _dc);		_dc = nullptr; }
		if (_hwnd)	{ ::DestroyWindow(_hwnd);		_hwnd = nullptr; }
	}

	void create() {
		auto hInstance = GetModuleHandle(nullptr);

		const wchar_t* className = L"OpenGLWindow_FalseContext";

		//-- Check did we register window class yet
		WNDCLASSEX wc;
		if (!GetClassInfoEx(hInstance, className, &wc))
		{
			//-- register window class
			memset(&wc, 0, sizeof(wc));
			wc.cbSize = sizeof(wc);
			wc.style = CS_OWNDC; //!! <------- CS_OWNDC is required for OpenGL Window
			wc.lpfnWndProc = DefWindowProc;
			wc.hInstance = hInstance;
			wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wc.hbrBackground = nullptr; //!! <------- 
			wc.lpszClassName = className;

			if (!RegisterClassEx(&wc))
			{
				throw SGE_ERROR("RegisterClassEx failed!");
			}
		}

		_hwnd = CreateWindowExW(0,
			className,
			L"",
			WS_POPUP,
			0, 0, 0, 0,
			nullptr, nullptr, hInstance, nullptr);
		if (!_hwnd) {
			throw SGE_ERROR("CreateWindow failed!");
		}

		_dc = GetDC(_hwnd);
		if (!_dc) {
			throw SGE_ERROR("GetDC failed!");
		}

		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 32;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int nPixelFormat = ChoosePixelFormat(_dc, &pfd);
		if (nPixelFormat == 0) {
			throw SGE_ERROR("ChoosePixelFormat");
		}

		BOOL bResult = SetPixelFormat(_dc, nPixelFormat, &pfd);
		if (!bResult) {
			throw SGE_ERROR("SetPixelFormat");
		}

		_rc = wglCreateContext(_dc);
		if (!_rc) {
			throw SGE_ERROR("wglCreateContext");
		}

		wglMakeCurrent(_dc, _rc);
}

	HGLRC getRC() const { return _rc; }


private:
	HWND	_hwnd = nullptr;
	HDC		_dc = nullptr;
	HGLRC	_rc = nullptr;
};

using OpenGLWindow_FalseContext_Impl = OpenGLWindow_FalseContext_Win32;

typedef BOOL(WINAPI* PFNWGLMAKECONTEXTCUREENTARBPROC) (HDC hDrawDC, HDC hReadDC, HGLRC hglrc);

#endif

RenderContext_gl::RenderContext_gl(CreateDesc desc_)
	:
	Base(desc_)
{
	_pRenderer = Renderer_gl::instance();

	OpenGLWindow_FalseContext_Impl falseContext;
	falseContext.create();

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

#if SGE_OS_WINDOWS

	//auto hwnd = desc_.pWindow->_hwnd;
	_hdc = desc_.pWindow->_hdc;
	auto hdc = _hdc;

	// create context
	const int formatAttrs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_DOUBLE_BUFFER_ARB,		/*isDoubleBuffered */ true ? GL_TRUE : GL_FALSE,
		// Multisampling only works when double-buffering is enabled
		WGL_SAMPLE_BUFFERS_ARB,		/*isDoubleBuffered */ true ? GL_TRUE : GL_FALSE,
		WGL_SAMPLES_ARB,			/*isDoubleBuffered */ true ? 4 : 0,
		0 // End of attributes list
	};

	int contextAttrs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 6,

#if 0 // disable deprecated function, such as glBegin, glColor, glLoadMatrix, glPushMatrix
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#else
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
#endif

		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0 // End of attributes list
	};

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	int format, numFormat;
	if (!wglChoosePixelFormatARB(hdc, formatAttrs, nullptr, 1, &format, (UINT*)&numFormat)) {
		throw SGE_ERROR("wglChoosePixelFormatARB");
	}

	PIXELFORMATDESCRIPTOR pfd;
	if (!SetPixelFormat(hdc, format, &pfd)) {
		throw SGE_ERROR("SetPixelFormat");
	}

	HGLRC sharedContext = nullptr;

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	_hrc = wglCreateContextAttribsARB(hdc, sharedContext, contextAttrs);

	if (!_hrc)
		throw SGE_ERROR("wglCreateContext");

	PFNWGLMAKECONTEXTCUREENTARBPROC wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCUREENTARBPROC)wglGetProcAddress("wglMakeContextCurrentARB");
	if (_hrc)
		wglMakeContextCurrentARB(hdc, hdc, _hrc);

	PFNWGLSWAPINTERVALEXTPROC wglSwapInternalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if (_pRenderer->isVsync())
	{
		wglSwapInternalEXT(1);
	}
	else
	{
		wglSwapInternalEXT(0);
	}
#endif

	{
		SGE_LOG("OpenGL Renderer:");
		SGE_LOG("Vendor: {}",	(const char*)glGetString(GL_VENDOR));
		SGE_LOG("Renderer: {}", (const char*)glGetString(GL_RENDERER));
		SGE_LOG("Version: {}",	(const char*)glGetString(GL_VERSION));

		GLint isMultiSample = 0;
		int texture_units = 0;
		glGetIntegerv(GL_SAMPLE_BUFFERS, &isMultiSample);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
		SGE_LOG("max texture size: {}", texture_units);

		if (isMultiSample)
			SGE_LOG("able to multi sample");
		else
			SGE_LOG("unable to multi sample");
	}
	_onInit();
}

RenderContext_gl::~RenderContext_gl()
{
	if (_testShaderID)
	{
		glDebugCall(glDeleteProgram(_testShaderID));
		_testShaderID = 0;
	}

	if (_testInputLayouts.size())
	{
		for (auto& e : _testInputLayouts)
		{
			glDebugCall(glDeleteVertexArrays(1, &e.second));
			e.second = 0;
		}
	}

	if (_hrc)
	{
		wglDeleteContext(_hrc);
		_hrc = nullptr;
	}
}

void RenderContext_gl::_onInit()
{
	glDebugCall(glEnable(GL_DEPTH_TEST));
	glDebugCall(glDepthMask(GL_TRUE));
	glDebugCall(glEnable(GL_MULTISAMPLE));
	glDebugCall(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
	glDebugCall(glEnable(GL_BLEND));
	glDebugCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	glDebugCall(glEnable(GL_LINE_SMOOTH));

	glDebugCall(glEnable(GL_CULL_FACE));
	glDebugCall(glCullFace(GL_BACK));

	glDebugCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

	//glDebugCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(1.0, 1.0);
}

void RenderContext_gl::onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd_)
{
	GLenum buffer_flags = 0;

	if (cmd_.depth.has_value())
	{
		buffer_flags |= GL_DEPTH_BUFFER_BIT;
		glDebugCall(glClearDepth(1.0f - cmd_.depth.value()));
	}

	if (cmd_.color.has_value())
	{
		glDebugCall(glClearColor(cmd_.color->r, cmd_.color->g, cmd_.color->b, cmd_.color->a));
		buffer_flags |= GL_COLOR_BUFFER_BIT;
		glDebugCall(glClear(buffer_flags));
	}
}

void RenderContext_gl::onCmd_SwapBuffers(RenderCommand_SwapBuffers& cmd_)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderContext_gl::onCmd_DrawCall(RenderCommand_DrawCall& cmd_)
{
	if (!cmd_.pVertexLayout) { SGE_ASSERT(false); return; }

	auto* vertexBuffer = static_cast<RenderGpuBuffer_gl*>(cmd_.spVertexBuffer.ptr());
	if (!vertexBuffer) { SGE_ASSERT(false); return; }

	if (cmd_.vertexCount <= 0) { SGE_ASSERT(false); return; }
	if (cmd_.primitive == RenderPrimitiveType::None) { SGE_ASSERT(false); return; }

	bool hasIndexBuffer = false;
	RenderGpuBuffer_gl* indexBuffer = nullptr;
	if (cmd_.indexCount > 0) {
		indexBuffer = static_cast<RenderGpuBuffer_gl*>(cmd_.spIndexBuffer.ptr());
		hasIndexBuffer = true;
		if (!indexBuffer) { SGE_ASSERT(false); return; }
	}

	auto indexCount = Util::castGLsizei(cmd_.indexCount);

	_setTestShaders();

	auto inputLayoutID = _getTestInputLayout(cmd_.pVertexLayout);
	SGE_ASSERT(inputLayoutID);
	glDebugCall(glBindVertexArray(inputLayoutID));
	//SGE_DUMP_VAR(inputLayoutID);

#if 1				// no glEnableVertexAttribArray and glVertexAttribPointer again then gg
	u32 index = 0;
	auto stride = Util::castGLsizei(cmd_.pVertexLayout->stride);
	for (auto& element : cmd_.pVertexLayout->elements)
	{
		GLenum glslBaseDataType = Util::getGLFormat(element.dataType);

		glDebugCall(glEnableVertexAttribArray(index));
		if (glslBaseDataType == GL_INT)
		{
			glDebugCall(glVertexAttribIPointer(index, Util::getGLFormatCount(element.dataType), glslBaseDataType,
				stride, reinterpret_cast<const void*>((intptr_t)element.offset)));
		}
		else
		{
			glDebugCall(glVertexAttribPointer(index, Util::getGLFormatCount(element.dataType), glslBaseDataType,
				GL_FALSE, stride, reinterpret_cast<const void*>((intptr_t)element.offset)));
		}
		index++;
	}
#endif // 1

	auto vertexBufferID = vertexBuffer->getRendererID();
	SGE_ASSERT(vertexBufferID);
	glDebugCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID));

	if (hasIndexBuffer)
	{
		auto indexType = Util::getGLFormat(cmd_.indexType);

		auto indexBufferID = indexBuffer->getRendererID();
		SGE_ASSERT(indexBufferID);
		glDebugCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID));

		glDebugCall(glDrawElements(Util::getGLPrimitiveTopology(cmd_.primitive), indexCount, indexType, 0));
	}
	else
	{
		glDebugCall(glDrawArrays(Util::getGLPrimitiveTopology(cmd_.primitive), 0, indexCount));
	}
}

void RenderContext_gl::onBeginRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDebugCall(glViewport(0, 0, static_cast<GLsizei>(_frameBufferSize.x), static_cast<GLsizei>(_frameBufferSize.y)));
}

void RenderContext_gl::onEndRender()
{

}

void RenderContext_gl::onSetFrameBufferSize(Vec2f newSize_)
{
	glDebugCall(glViewport(0, 0, static_cast<GLsizei>(newSize_.x), static_cast<GLsizei>(newSize_.y)));
}

void RenderContext_gl::onCommit(RenderCommandBuffer& cmdBuf_)
{
	_dispatch(this, cmdBuf_);
}

void RenderContext_gl::_setTestShaders()
{
	if (_testShaderID == 0)
	{
		String vertexShaderStr = R"(
		#version 450 core
		layout (location = 0) in vec3 a_position;
		layout (location = 1) in vec2 a_uv;
		layout (location = 2) in vec3 a_normal;

		out VertexData
		{
			vec3 pos;
			vec2 uv;
			vec3 normal;
		} v2f;

		void main()
		{
			v2f.pos = a_position;
			v2f.uv = a_uv;
			v2f.normal =  a_normal;

		   gl_Position = vec4(a_position, 1.0f);
		};
)";

		String pixelShaderStr = R"(
		#version 450 core

		out vec4 o_color;

		in VertexData
		{
			vec3 pos;
			vec2 texCoord;
			vec3 normal;
		} v2f;

		void main()
		{
		   o_color = vec4(v2f.normal, 1.0f);
			
		   //o_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
		};
)";

		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		const char* tmp = vertexShaderStr.c_str();
		glShaderSource(vertexShader, 1, &tmp, NULL);
		glCompileShader(vertexShader);
		// check for shader compile errors
		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// fragment shader
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		tmp = pixelShaderStr.c_str();
		glShaderSource(fragmentShader, 1, &tmp, NULL);
		glCompileShader(fragmentShader);
		// check for shader compile errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		// link shaders
		_testShaderID = glCreateProgram();
		glAttachShader(_testShaderID, vertexShader);
		glAttachShader(_testShaderID, fragmentShader);
		glLinkProgram(_testShaderID);
		// check for linking errors
		glGetProgramiv(_testShaderID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(_testShaderID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	glDebugCall(glUseProgram(_testShaderID));
}

u32 RenderContext_gl::_getTestInputLayout(const VertexLayout* pVertexLayout_)
{
	if (!pVertexLayout_)
		return 0;

	auto it = _testInputLayouts.find(pVertexLayout_);
	if (it)
		return it->second;

	auto stride = Util::castGLsizei(pVertexLayout_->stride);

	auto& layoutID = _testInputLayouts[pVertexLayout_];

	glDebugCall(glGenVertexArrays(1, &layoutID));
	glDebugCall(glBindVertexArray(layoutID));

#if 1
	u32 index = 0;

	for (auto& element : pVertexLayout_->elements)
	{
		GLenum glslBaseDataType = Util::getGLFormat(element.dataType);

		glDebugCall(glEnableVertexAttribArray(index));
		if (glslBaseDataType == GL_INT)
		{
			glDebugCall(glVertexAttribIPointer(index, Util::getGLFormatCount(element.dataType), glslBaseDataType,
				stride, reinterpret_cast<const void*>((intptr_t)element.offset)));
		}
		else
		{
			glDebugCall(glVertexAttribPointer(index, Util::getGLFormatCount(element.dataType), glslBaseDataType,
				GL_FALSE, stride, reinterpret_cast<const void*>((intptr_t)element.offset)));
		}
		index++;
	}
#else
	glDebugCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(0)));
	glDebugCall(glEnableVertexAttribArray(0));

	glDebugCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(12)));
	glDebugCall(glEnableVertexAttribArray(1));

	glDebugCall(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(20)));
	glDebugCall(glEnableVertexAttribArray(2));
#endif // 0

	glDebugCall(glBindVertexArray(0));

	return layoutID;
}

}

#endif