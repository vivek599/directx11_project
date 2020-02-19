#pragma once

#include <Windows.h>

#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "SkyboxClass.h"
#include "FrustumClass.h"
#include "bitmapclass.h"
#include "RenderTextureClass.h"
#include "textureshaderclass.h" 

#include "GlyphClass.h"
#include "LightClass.h"
#include "PointLightClass.h"

const bool FULL_SCREEN = false;
const bool VSYNCE_ENABLED = false;
const float SCREEN_DEPTH = 2500.f;
const float SCREEN_NEAR = 0.1f;

const int SHADOWMAP_WIDTH = 2048;
const int SHADOWMAP_HEIGHT = 2048;

class GraphicClass
{

public:
	GraphicClass();
	GraphicClass(const GraphicClass& obj);
	~GraphicClass();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	bool Frame( float deltaTime);

private:
	bool Render(float deltaTime);
	bool RenderToTexture(float deltaTime);
	bool RenderScene(float deltaTime, bool depthPass);

	unique_ptr<D3DClass> m_D3D;
	unique_ptr<CameraClass> m_Camera;
	vector <ModelClass*> m_Models;
	unique_ptr<SkyboxClass> m_skybox;
	unique_ptr<FrustumClass> m_frustum;
	unique_ptr<BitmapClass> m_Bitmap;
	unique_ptr<GlyphClass> m_Glyph;
	unique_ptr<RenderTextureClass> m_renderTexture;
		
	unique_ptr < LightClass> m_Light;
	 
	Mat4 m_ViewMatrix2D;

};