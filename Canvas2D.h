#pragma once


#include "BaseClass.h"
#include "D3dClass.h"


class Canvas2D : BaseClass
{
public:
	Canvas2D();
	~Canvas2D();

	bool Initialize(D3DClass* renderer);
	void Shutdown();
	bool RenderUI2D( float deltaTime);


private:
	ComPtr< IDXGISurface> m_d2dBackBuffer;

	ComPtr<IDWriteFactory> m_dWriteFactory;
	ComPtr<ID2D1Factory3> m_d2dFactory;
	ComPtr<ID2D1RenderTarget> m_renderTarget;

	ComPtr<ID2D1SolidColorBrush> m_brush;
	ComPtr<ID2D1SolidColorBrush> m_textbrush;
	ComPtr<ID2D1SolidColorBrush> m_fillbrush;
	 
	D2D1::Matrix3x2F m_transform;

	ComPtr<IDWriteTextFormat> m_textFormat;

	void BeginDraw() { m_renderTarget->BeginDraw(); }
	void EndDraw() { m_renderTarget->EndDraw(); }
	void DrawCircle(float x, float y, float radius );
	void DrawTextString( wstring str, int posX, int posY, int textWidth);
	void DrawFillRect(int posX, int posY, int fillWidth, int fillHeight);
	void DrawRect(int posX, int posY, int fillWidth, int fillHeight, float stroke);
};

