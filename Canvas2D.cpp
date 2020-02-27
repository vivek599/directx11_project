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
	assert(SUCCEEDED(hr));

	// Create the DXGI Surface Render Target.
	FLOAT dpiX;
	FLOAT dpiY;
	m_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpiX,
			dpiY
		);

	// Create a Direct2D render target which can draw into the surface in the swap chain
	hr = m_d2dFactory->CreateDxgiSurfaceRenderTarget(renderer->GetBackBufferSurface().Get(), &props, m_renderTarget.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));

	hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), m_brush.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));

	hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), m_textbrush.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));

	hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), m_fillbrush.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));


	D2D1_SIZE_F targetSize = m_renderTarget->GetSize();
	m_transform = D2D1::Matrix3x2F::Scale(targetSize);
	m_brush->SetTransform(m_transform);
	m_textbrush->SetTransform(m_transform);
	m_fillbrush->SetTransform(m_transform);

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)m_dWriteFactory.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));

	hr = m_dWriteFactory->CreateTextFormat(
		L"RobotoCondensed-Regular.ttf",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		18,
		L"en-us",
		m_textFormat.ReleaseAndGetAddressOf()
	);
	
	hr = m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	assert(SUCCEEDED(hr));

	hr = m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	assert(SUCCEEDED(hr));

	return true;
}

void Canvas2D::Shutdown()
{

}

bool Canvas2D::RenderUI2D( float deltaTime)
{
	HRESULT hr;

	BeginDraw(); 

	DrawFillRect(900, 600, 300, 100);
	DrawRect(100, 600, 300, 100, 8.0f);
	DrawCircle( 800, 600, 80.0f );

	wstring wstr = wstring( userInputString.begin(), userInputString.end());
	DrawTextString(wstr, 15, 15, 1250);

	
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
