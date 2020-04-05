#include "Canvas2D.h"



Canvas2D::Canvas2D()
{
 
}

Canvas2D::~Canvas2D()
{
}

bool Canvas2D::Initialize( D3DClass* renderer)
{
	HRESULT hr;

	D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};
	d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(*m_d2dFactory.Get()), &d2dFactoryOptions, (void**)m_d2dFactory.ReleaseAndGetAddressOf());
	MYASSERT(SUCCEEDED(hr));

	// Create the DXGI Surface Render Target.
	//FLOAT dpiX = GetDpiForWindow(GetActiveWindow());
	//FLOAT dpiY = GetDpiForWindow(GetActiveWindow());
	//m_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);

	FLOAT dpiX, dpiY;

	HDC screen = GetDC(0);
	dpiX = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSX));
	dpiY = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSY));
	

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpiX,
			dpiY
		);

	// Create a Direct2D render target which can draw into the surface in the swap chain
	hr = m_d2dFactory->CreateDxgiSurfaceRenderTarget(renderer->GetBackBufferSurface().Get(), &props, m_renderTarget.ReleaseAndGetAddressOf());
	MYASSERT(SUCCEEDED(hr));




	hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), m_brush.ReleaseAndGetAddressOf());
	MYASSERT(SUCCEEDED(hr));

	hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), m_textbrush.ReleaseAndGetAddressOf());
	MYASSERT(SUCCEEDED(hr));

	hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), m_fillbrush.ReleaseAndGetAddressOf());
	MYASSERT(SUCCEEDED(hr));


	D2D1_SIZE_F targetSize = m_renderTarget->GetSize();
	m_transform = D2D1::Matrix3x2F::Scale(targetSize);
	m_brush->SetTransform(m_transform);
	m_textbrush->SetTransform(m_transform);
	m_fillbrush->SetTransform(m_transform);

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)m_dWriteFactory.ReleaseAndGetAddressOf());
	MYASSERT(SUCCEEDED(hr));

	hr = m_dWriteFactory->CreateTextFormat(
		L"Trebuchet MS",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		18,
		L"en-us",
		m_textFormat.ReleaseAndGetAddressOf()
	);

	hr = m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	MYASSERT(SUCCEEDED(hr));

	hr = m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	MYASSERT(SUCCEEDED(hr));

	hr = m_dWriteFactory->CreateTextFormat(
		L"Trebuchet MS",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		2048,
		L"en-us",
		m_textFormat96.ReleaseAndGetAddressOf()
	);

	hr = m_textFormat96->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	MYASSERT(SUCCEEDED(hr));

	hr = m_textFormat96->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	MYASSERT(SUCCEEDED(hr));

	return true;
}

bool Canvas2D::InitializeWorldText(ATexture* textQuad)
{
	FLOAT dpiX = GetDpiForWindow(GetActiveWindow());
	FLOAT dpiY = GetDpiForWindow(GetActiveWindow());

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpiX,
			dpiY
		);

	HRESULT hr = m_d2dFactory->CreateDxgiSurfaceRenderTarget(textQuad->GetDXGISurface(), &props, m_renderTargetWorldText.ReleaseAndGetAddressOf());
	MYASSERT(SUCCEEDED(hr));
	
	return true;
}

void Canvas2D::Shutdown()
{

}

bool Canvas2D::RenderUI2D( float deltaTime)
{
	HRESULT hr;

	BeginDraw(); 
	m_renderTargetWorldText->BeginDraw();
	m_renderTargetWorldText->Clear();

	DrawFillRect(900, 600, 300, 100);
	DrawRect(100, 600, 300, 100, 8.0f);
	DrawCircle( 800, 600, 80.0f );

	wstring wstr = wstring( userInputString.begin(), userInputString.end());
	DrawTextString(wstr, 15, 15, 1250);

	DrawTextStringWorld(L"HELLO WORLD..." + wstring(L"\nDeltaTime : ") + to_wstring(deltaTime), 0, 0, 65536);


	m_renderTargetWorldText->EndDraw();
	EndDraw();

	return true;
}

void Canvas2D::DrawFillRect(int posX, int posY, int fillWidth, int fillHeight)
{
	D2D1_RECT_F fillRect = D2D1::RectF(posX, posY, posX + fillWidth, posY + fillHeight); 
	 
	m_renderTarget->FillRectangle(fillRect, m_fillbrush.Get());
} 

void Canvas2D::DrawRect(int posX, int posY, int fillWidth, int fillHeight, float stroke)
{
	D2D1_RECT_F fillRect = D2D1::RectF(posX, posY, posX + fillWidth, posY + fillHeight);

	m_renderTarget->DrawRectangle(fillRect, m_fillbrush.Get(), stroke);
}

void Canvas2D::DrawTextStringWorld(wstring str, int posX, int posY, int textWidth)
{
	D2D1_RECT_F textRect = D2D1::RectF(posX, posY, posX + textWidth, posY + textWidth); 

	D2D1_SIZE_F size;
	size.width = 0.25f * 0.25f;
	size.height = 1.0f * 0.25f;

	m_renderTargetWorldText->SetTransform( D2D1::Matrix3x2F::Scale(size ) );
	m_renderTargetWorldText->SetTextAntialiasMode( D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE );
	m_renderTargetWorldText->DrawText(
		str.c_str(),
		str.length(),
		m_textFormat96.Get(),
		&textRect,
		m_textbrush.Get()
	);

}

void Canvas2D::DrawCircle(float x, float y, float radius)
{
	
	m_renderTarget->DrawEllipse( D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), m_brush.Get(), 8.f );

}

void Canvas2D::DrawTextString( wstring str, int posX, int posY, int textWidth)
{
	D2D1_RECT_F textRect = D2D1::RectF(posX, posY, posX + textWidth, posY + textWidth);//rtSize.width, rtSize.height);
	
	m_renderTarget->DrawText(
		str.c_str(),
		str.length(),
		m_textFormat.Get(),
		&textRect,
		m_textbrush.Get()
	);
}
