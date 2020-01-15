#pragma once
#include "commoninclude.h"

using namespace DirectX;



class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass& obj);
	~TextureClass();

	bool Initialize(ID3D11Device* device, const WCHAR* path);
	void Shutdown(); 

	ComPtr<ID3D11ShaderResourceView> GetTexture();

private:
	ComPtr < ID3D11ShaderResourceView> m_Texture;


};

