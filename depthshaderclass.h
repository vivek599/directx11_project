#pragma once

#include "commoninclude.h"


class DepthShaderClass
{
private:
	struct MatrixBufferType
	{
		Mat4 world;
		Mat4 view;
		Mat4 projection;
	};

	struct CameraBufferType
	{
		Vector3 cameraPosition;
		float padding;
	};

public:
	DepthShaderClass();
	DepthShaderClass(const DepthShaderClass&);
	~DepthShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, Mat4, Mat4, Mat4, Vector3 cameraPosition);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, Mat4, Mat4, Mat4, Vector3 cameraPosition);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ComPtr<ID3D11VertexShader> m_vertexShader;
	ComPtr<ID3D11PixelShader> m_pixelShader;
	ComPtr<ID3D11InputLayout> m_layout;
	ComPtr<ID3D11Buffer> m_matrixBuffer;
	ComPtr<ID3D11Buffer> m_cameraBuffer;
};

