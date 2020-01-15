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
const float SCREEN_DEPTH = 1000.f;
const float SCREEN_NEAR = 1.0f;

const int SHADOWMAP_WIDTH = 512;
const int SHADOWMAP_HEIGHT = 512;

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

	D3DClass* m_D3D;
	CameraClass* m_Camera;
	vector <ModelClass*> m_Models;
	SkyboxClass* m_skybox;
	FrustumClass* m_frustum;
	BitmapClass* m_Bitmap;
	GlyphClass* m_Glyph;
	RenderTextureClass* m_renderTexture;
		
	LightClass* m_Light;
	 
	Mat4 m_ViewMatrix2D;

};