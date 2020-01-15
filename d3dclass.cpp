#include "d3dclass.h"



D3DClass::D3DClass()
{
 
	m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	 
}

D3DClass::D3DClass(const D3DClass& obj)
{

	UNREFERENCED_PARAMETER(obj);
}

D3DClass::~D3DClass()
{

}


bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsyncEnabled, HWND hwnd, bool full_screen, float screen_depth, float screen_near)
{
	HRESULT result;
	ComPtr<IDXGIFactory> factory;
	ComPtr<IDXGIAdapter> adapter;
	ComPtr<IDXGIOutput> adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	size_t stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	//D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	const unsigned int numFeaturelevels = _countof(featureLevels);
	ComPtr<ID3D11Texture2D> backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;

	float fieldOfView, screenAspect;

	D3D11_DEPTH_STENCIL_DESC depthDisableStencilDesc;
	m_vsyncEnabled = vsyncEnabled;

	//creat directx graphic interface factory
	result = CreateDXGIFactory( IID_PPV_ARGS(factory.GetAddressOf()));
	if (FAILED(result))
	{
		return false;
	}

	//use factory to creat an adapter for primary graphic interface( video card )
	result = factory->EnumAdapters(0, adapter.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	//enumerate primary adapter output (monitor)
	result = adapter->EnumOutputs(0, adapterOutput.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}
	
	//get the number of modes that fits DXGI_FORMAT_R8G8B8A8_UNORM display format for the monitor
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	//fill the display mode list desc
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	for ( i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth )
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}


	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	m_videoCardMemory = (int)adapterDesc.DedicatedVideoMemory / (1024*1024);
	error = wcstombs_s(  &stringLength, m_videoCardDesc, 128, adapterDesc.Description, 128);
	if (error != 0 )
	{
		return false;
	}

	delete[] displayModeList;
	displayModeList = 0;

	adapterOutput.Reset();
	adapter.Reset();
	factory.Reset();

	//init swap chain desc
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//set to single back buffer
	swapChainDesc.BufferCount = 1;

	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (m_vsyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;

	//multisampling off
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.Windowed = !full_screen;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	swapChainDesc.Flags = 0;

	//featureLevel = D3D_FEATURE_LEVEL_11_0;

	//create swap chain device and context
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, featureLevels, numFeaturelevels,
		D3D11_SDK_VERSION, &swapChainDesc, m_swapChain.GetAddressOf(),  m_device.GetAddressOf(), &m_featureLevel, m_deviceContext.GetAddressOf() );
	if (FAILED(result))
	{
		return false;
	}

	result = m_swapChain->GetBuffer(0, IID_PPV_ARGS( backBufferPtr.GetAddressOf()));
	if (FAILED(result))
	{
		return false;
	}

	//create render target view with back buffer pointer
	result = m_device->CreateRenderTargetView(backBufferPtr.Get(), NULL, m_renderTargetView.GetAddressOf() );
	if (FAILED(result))
	{
		return false;
	}

	backBufferPtr.Reset();

	//depth buffer

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//create texture for depth buffer
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, m_depthStencilBuffer.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	//depth stencil state
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//stencil operation if pixel is front facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//if pixel is back facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//create depth stencil state
	result = m_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	m_deviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 1);

	// init depth stencil view
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, m_depthStencilView.GetAddressOf() );
	if (FAILED(result))
	{
		return false;
	}

	// bind the render target view and depth stencil buffer  to the output render pipeline
	m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;//or wireframe
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.f;
	
	result = m_device->CreateRasterizerState(&rasterDesc, m_rasterState.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	m_deviceContext->RSSetState(m_rasterState.Get());

	// Setup a raster description which turns off back face culling.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the no culling rasterizer state.
	result = m_device->CreateRasterizerState(&rasterDesc, m_rasterStateNoCulling.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	//viewport to entire size of window
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height= (float)screenHeight;
	m_viewport.MinDepth = 0.f;
	m_viewport.MaxDepth = 1.f;
	m_viewport.TopLeftX = 0.f;
	m_viewport.TopLeftY = 0.f;

	m_deviceContext->RSSetViewports(1, &m_viewport);

	//projection matrix
	fieldOfView = (float)XM_PI / 4.f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	m_projectionMatrix = XMMatrixPerspectiveFovLH( fieldOfView, screenAspect, screen_near, screen_depth);

	//world matrix to identity
	m_worldMatrix = XMMatrixIdentity();

	// here create view matrix , 
	// implemented in camera class

	//ortho matrix
	m_orthoMatrix = XMMatrixOrthographicLH( (float)screenWidth, (float)screenHeight, screen_near, screen_depth );

	ZeroMemory(&depthDisableStencilDesc, sizeof(depthDisableStencilDesc) );
	depthDisableStencilDesc.DepthEnable = false;
	depthDisableStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisableStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisableStencilDesc.StencilEnable = true;
	depthDisableStencilDesc.StencilReadMask = 0xFF;
	depthDisableStencilDesc.StencilWriteMask = 0xFF;
	
	depthDisableStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisableStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisableStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisableStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthDisableStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisableStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisableStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisableStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = m_device->CreateDepthStencilState(&depthDisableStencilDesc, m_DepthDisableStencilState.GetAddressOf() );
	if (FAILED(result))
	{
		return false;
	}


	D3D11_BLEND_DESC blendStateDescription = {};

	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
    //blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	// Create the blend state using the description.
	result = m_device->CreateBlendState(&blendStateDescription, m_alphaEnableBlendingState.GetAddressOf());
	if(FAILED(result))
	{
		return false;
	}
	return true;

}

void D3DClass::Shutdown()
{
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	m_alphaEnableBlendingState.Reset();
	m_DepthDisableStencilState.Reset();

	m_rasterState.Reset();

	m_depthStencilView.Reset();

	m_depthStencilState.Reset();

	m_rasterStateNoCulling.Reset();

	m_depthStencilBuffer.Reset();

	m_renderTargetView.Reset();

	m_deviceContext.Reset();

	m_device.Reset();

	m_swapChain.Reset();




}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//clear back buffer
	m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), color);
	
	//clear depth buffer
	m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);

	TurnOnAlphaBlending();

}

void D3DClass::EndScene()
{
	//present the back buffer to screen  since rendering is complete
	if (m_vsyncEnabled)
	{
		m_swapChain->Present(1, 0);
	}
	else
	{
		// as fast as possible
		m_swapChain->Present(0, 0);
	}

}

ComPtr<ID3D11Device> D3DClass::GetDevice()
{
	return m_device;
}

ComPtr < ID3D11DeviceContext> D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(Mat4& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void D3DClass::GetWorldMatrix(Mat4& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}

void D3DClass::GetOrthoMatrix(Mat4& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDesc);
	memory = m_videoCardMemory;



}

void D3DClass::WriteVideoCardInfoToFile()
{
	ofstream VideoCardInfo("VideoCardInfo.txt");
	VideoCardInfo << "Video Card Name: " << m_videoCardDesc << endl;
	VideoCardInfo << "Video Card Memory: " << m_videoCardMemory << endl;

	switch (m_featureLevel)
	{
		case D3D_FEATURE_LEVEL_11_0:
			VideoCardInfo << "Supported Directx: Directx 11" << endl;
			break;
		case D3D_FEATURE_LEVEL_10_1:
		case D3D_FEATURE_LEVEL_10_0:
			VideoCardInfo << "Supported Directx: Directx 10" << endl;
			break;
		case D3D_FEATURE_LEVEL_9_3:
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_1:
			VideoCardInfo << "Supported Directx: Directx 9" << endl;
			break;
		default:
			break;
	}

	
	VideoCardInfo.close();
}

void D3DClass::TurnOnAlphaBlending()
{
	float blendFactor[4];

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn on the alpha blending.
	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState.Get(), blendFactor, 0xffffffff);

}

ComPtr < ID3D11DepthStencilView> D3DClass::GetDepthStencilView()
{
	return m_depthStencilView;
}

void D3DClass::SetBackBufferRenderTarget()
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
}

void D3DClass::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 1);
}

void D3DClass::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(m_DepthDisableStencilState.Get(), 1);
}

void D3DClass::TurnOnCulling()
{
	// Set the culling rasterizer state.
	m_deviceContext->RSSetState(m_rasterState.Get());
}

void D3DClass::TurnOffCulling()
{
	// Set the culling rasterizer state.
	m_deviceContext->RSSetState(m_rasterStateNoCulling.Get());
}

void D3DClass::ResetViewport()
{
	// Set the viewport.
	m_deviceContext->RSSetViewports(1, &m_viewport);

}