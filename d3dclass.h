#pragma once



#include "commoninclude.h"

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass& obj);
	~D3DClass();

	bool Initialize(int screenWidth, int screenHeight, bool vsyncEnabled, HWND hwnd, bool full_screen,
		float screen_depth, float screen_near);
	void Shutdown();

	void BeginScene(float param1, float param2, float param3, float param4);
	void EndScene();

	ComPtr<ID3D11Device> GetDevice();
	ComPtr<ID3D11DeviceContext> GetDeviceContext();

	void GetProjectionMatrix(Mat4& projectionMatrix);
	void GetWorldMatrix(Mat4& worldMatrix);
	void GetOrthoMatrix(Mat4& orthoMatrix);

	void GetVideoCardInfo(char* cardName, int& memory);
	void WriteVideoCardInfoToFile();

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnOnCulling();
	void TurnOffCulling();

	void ResetViewport();

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	ComPtr < ID3D11DepthStencilView> GetDepthStencilView();
	void SetBackBufferRenderTarget();

protected:
private:
	bool m_vsyncEnabled;
	int m_videoCardMemory;
	char m_videoCardDesc[128];
	
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_deviceContext;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
	ComPtr<ID3D11DepthStencilState> m_depthStencilState;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	ComPtr<ID3D11RasterizerState> m_rasterState;
	ComPtr<ID3D11RasterizerState> m_rasterStateNoCulling;

	Mat4 m_projectionMatrix;
	Mat4 m_worldMatrix;
	Mat4 m_orthoMatrix;

	D3D_FEATURE_LEVEL m_featureLevel;
	ComPtr < ID3D11DepthStencilState> m_DepthDisableStencilState;
	ComPtr < ID3D11BlendState> m_alphaEnableBlendingState;

	D3D11_VIEWPORT m_viewport;
};
