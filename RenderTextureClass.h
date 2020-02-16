#pragma once

#include "commoninclude.h"

class RenderTextureClass
{
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass& obj);
	~RenderTextureClass();

	bool Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenNear, float screenDepth);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext* deviceContext);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, float r, float g, float b, float a);
	ComPtr < ID3D11ShaderResourceView> GetShaderResourceView();
 

private:
	ComPtr<ID3D11Texture2D> m_renderTargetTexture;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	D3D11_VIEWPORT m_viewport;
 
};

