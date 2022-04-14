#include "GraphicsContext_D3d11.h"

#include "sge_core/native_ui/NativeUI.h"

#if SGE_OS_WINDOWS

namespace sge {

GraphicsContext_D3d11::~GraphicsContext_D3d11()
{
	destroy();
}

void GraphicsContext_D3d11::init(void* hWnd_)
{
	SGE_ASSERT(_pSwapchain, "");
	SGE_ASSERT(_pDev, "");
	SGE_ASSERT(_pDevcon, "");

	// init swapchain
	{
		// create a struct to hold information about the swap chain
		DXGI_SWAP_CHAIN_DESC scd;

		// clear out the struct for use
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

		// fill the swap chain description struct
		scd.BufferCount = 1;                                    // one back buffer
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
		scd.BufferDesc.Width = s_temp_width;                    // set the back buffer width
		scd.BufferDesc.Height = s_temp_height;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
		scd.OutputWindow = (HWND)hWnd_;                                // the window to be used
		scd.SampleDesc.Count = 4;                               // how many multisamples
		scd.Windowed = TRUE;                                    // windowed/full-screen mode
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching

		// create a device, device context and swap chain using the information in the scd struct
		D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			NULL,
			NULL,
			NULL,
			D3D11_SDK_VERSION,
			&scd,
			&_pSwapchain,
			&_pDev,
			NULL,
			&_pDevcon);

		SGE_LOG("D3D11 SwapChain created sucessfully!");
	}

	// init backbuffer and config render target
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer;
		_pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		_pDev->CreateRenderTargetView(pBackBuffer, NULL, &_pRTV);
		pBackBuffer->Release();

		// set the render target as the back buffer
		_pDevcon->OMSetRenderTargets(1, &_pRTV, NULL);
	}
	
	// Set the viewport
	{
		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = s_temp_width;
		viewport.Height = s_temp_height;

		_pDevcon->RSSetViewports(1, &viewport);
	}

	// init shader
	{
		D3DCompileFromFile(L"../../../../../shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, &_pVS_Blob, &_pErrorMsgs);
		if (_pErrorMsgs)
		{
			//SGE_LOG_ERROR(_pErrorMsgs->GetBufferPointer());
			SGE_ASSERT(0);
		}

		D3DCompileFromFile(L"../../../../../shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, &_pPS_Blob, &_pErrorMsgs);
		if (_pErrorMsgs)
		{
			//SGE_LOG_ERROR(_pErrorMsgs->GetBufferPointer());
			SGE_ASSERT(0);
		}

		// encapsulate both shaders into shader objects
		_pDev->CreateVertexShader(_pVS_Blob->GetBufferPointer(), _pVS_Blob->GetBufferSize(), NULL, &_pVS);
		_pDev->CreatePixelShader(_pPS_Blob->GetBufferPointer(), _pPS_Blob->GetBufferSize(), NULL, &_pPS);

		// set the shader objects
		_pDevcon->VSSetShader(_pVS, 0, 0);
		_pDevcon->PSSetShader(_pPS, 0, 0);
	}

	// init vertex buffer
	{
		VertexLayout OurVertices[] =
		{
			{0.0f, 0.5f, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f} },
			{0.45f, -0.5, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f} },
			{-0.45f, -0.5f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f} },
		};

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DYNAMIC;						// write access access by CPU and GPU
		bd.ByteWidth = sizeof(VertexLayout) * 3;            // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// use as a vertex buffer
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			// allow CPU to write in buffer

		_pDev->CreateBuffer(&bd, NULL, &_pVBuffer);			// create the buffer

		D3D11_MAPPED_SUBRESOURCE ms;
		_pDevcon->Map(_pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
		memcpy(ms.pData, OurVertices, sizeof(OurVertices));                // copy the data
		_pDevcon->Unmap(_pVBuffer, NULL);                                     // unmap the buffer

		// Set vertex buffer layout to as same as shader layout
		ID3D11InputLayout* pLayout;    // global

		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		_pDev->CreateInputLayout(ied, 2, _pVS_Blob->GetBufferPointer(), _pVS_Blob->GetBufferSize(), &pLayout);
		_pDevcon->IASetInputLayout(pLayout);
	}

	
}

void GraphicsContext_D3d11::destroy()
{
	SGE_ASSERT(!_pSwapchain, "");
	SGE_ASSERT(!_pDev, "");
	SGE_ASSERT(!_pDevcon, "");

	_pSwapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

	_pVS->Release();
	_pPS->Release();

	_pSwapchain->Release();

	_pRTV->Release();

	_pDev->Release();
	_pDevcon->Release();

	SGE_LOG("D3D11 SwapChain destroyed sucessfully!");
}

void GraphicsContext_D3d11::swapBuffers()
{
	// clear the back buffer to a deep blue
	float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	_pDevcon->ClearRenderTargetView(_pRTV, color);

	// temp
	// do 3D rendering on the back buffer here
	{
		// configure primitives topology and render to screen
		{
			UINT stride = sizeof(VertexLayout);
			UINT offset = 0;
			_pDevcon->IASetVertexBuffers(0, 1, &_pVBuffer, &stride, &offset);

			_pDevcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// select which primtive type we are using
			_pDevcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// draw the vertex buffer to the back buffer
			_pDevcon->Draw(3, 0);
		}
	}

	// switch the back buffer and the front buffer
	_pSwapchain->Present(0, 0);
}

//void GraphicsContext_D3d11::makeContextCurrent()
//{
//
//}
//
//void GraphicsContext_D3d11::enableVsync(bool isVsync_)
//{
//
//}

}


#endif // sge_os_windows
