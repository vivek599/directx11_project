#pragma once

#include "commoninclude.h"

#include "textureclass.h"
#include "FontShaderClass.h"

#include <fstream>
#include <vector>

#include "BaseClass.h"

using namespace std;


class GlyphClass : BaseClass
{
public:
	GlyphClass();
	GlyphClass(const GlyphClass& obj);
	~GlyphClass();

	bool Initialize(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight, WCHAR* path );
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int posX, int posY, Mat4 world, Mat4 view, Mat4 ortho);

	int GetIndexCount();

	ComPtr < ID3D11ShaderResourceView> GetTexture();


private:
	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
	};

	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext* context, int posX, int posY);
	void RenderBuffers(ID3D11DeviceContext* context);

	bool LoadTexture(ID3D11Device* device, WCHAR* path);
	void ReleaseTexture();

	ComPtr<ID3D11Buffer> m_VertexBuffer;
	ComPtr<ID3D11Buffer> m_IndexBuffer;
	int m_VertexCount, m_IndexCount;

	unique_ptr<TextureClass> m_fontTexture;
	unique_ptr<FontShaderClass> m_fontTextureShader;

	int m_ScreenWidth, m_ScreenHeight;

	int m_NewlineCounter;
	int m_EnterKeyCounter;
	int m_LineLocalWidth;
	float m_PosOffsetX;
	float m_PosOffsetY;
	int m_MaxCharInLine;
	string m_StringToDraw;
	char m_GlyphCharacter;
	//int m_GlyphWidth;
	ifstream m_GlyphWidthData;
	vector<int> m_GlyphWidth;

};

