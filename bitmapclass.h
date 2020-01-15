#pragma once


#include "textureclass.h"
#include "BitmapShaderClass.h"

#include "commoninclude.h"

class BitmapClass
{
public:
	BitmapClass();
	BitmapClass(const BitmapClass&);
	~BitmapClass();

	bool Initialize(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight, WCHAR* path, int bitmapWidth, int bitmapHeight);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, Mat4 world, Mat4 view, Mat4 ortho, int posX, int posY, ID3D11ShaderResourceView* texture);

	int GetIndexCount();

	ComPtr < ID3D11ShaderResourceView> GetTexture();

protected:
private:
// 	struct VertexType
// 	{
// 		Vector3 position;
// 		Vector4 color;
// 	};
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

	TextureClass* m_Texture;

	int m_ScreenWidth, m_ScreenHeight;
	int m_BitmapWidth, m_BitmapHeight;
	int m_PrevPosX, m_PrevPosY;

	BitmapShaderClass* m_BitmapShader;

};


