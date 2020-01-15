#pragma once

#include "commoninclude.h"

class SkyboxShaderClass
{
public:
	SkyboxShaderClass();
	SkyboxShaderClass(const SkyboxShaderClass& obj);
	~SkyboxShaderClass();

	struct MatrixBufferType
	{
		Mat4 world;
		Mat4 view;
		Mat4 proj;
	};

	bool Initialize(ID3D11Device* device);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int indexCount, Mat4 worldMatrix, Mat4 viewMatrix, Mat4 projMatrix,
		ID3D11ShaderResourceView* texture );

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName);
	void ShutdownShader();
	void OutputShaderErrorMsg(ID3D10Blob* errMsg, HWND hwnd, WCHAR* shaderFileName);
	bool SetShaderParameter(ID3D11DeviceContext* context, Mat4 world, Mat4 view, Mat4 proj, ID3D11ShaderResourceView* texture );
	void RenderShader(ID3D11DeviceContext* context, int indexCount);

	ComPtr<ID3D11VertexShader> m_vertexShader;
	ComPtr<ID3D11PixelShader> m_pixelShader;
	ComPtr<ID3D11InputLayout> m_inputLayout;
	ComPtr<ID3D11SamplerState> m_samplerState;
	ComPtr<ID3D11Buffer> m_matrixBuffer;
};

