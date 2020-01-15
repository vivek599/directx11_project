#include "textureclass.h"



TextureClass::TextureClass()
{

}

TextureClass::TextureClass(const TextureClass& obj)
{
	UNREFERENCED_PARAMETER(obj);

}

TextureClass::~TextureClass()
{

}

bool TextureClass::Initialize(ID3D11Device* device, const WCHAR* path)
{
	HRESULT result;

	//result = D3DX11CreateShaderResourceViewFromFile(device, path, NULL, NULL, &m_Texture, NULL );
	result = CreateWICTextureFromFile(device, path, NULL, m_Texture.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	return true;

}

void TextureClass::Shutdown()
{
	m_Texture.Reset();

}

ComPtr<ID3D11ShaderResourceView> TextureClass::GetTexture()
{
	return m_Texture;
}