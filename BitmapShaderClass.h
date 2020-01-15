#pragma once

#include "commoninclude.h"


class BitmapShaderClass
{
public:
	BitmapShaderClass();
	BitmapShaderClass(const BitmapShaderClass& obj);
	~BitmapShaderClass();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int indexcount, Mat4 world, Mat4 view, Mat4 projection, ID3D11ShaderResourceView* Bitmap);


private:
	struct MatrixBufferType
	{
		Mat4 world;
		Mat4 view;
		Mat4 projection;
	};

	bool InitializeShaders(ID3D11Device* device, HWND hwnd, WCHAR* vsPath, WCHAR* psPath);
	void ShutdownShaders();
	void OutputShaderErrorMsg(ID3DBlob* errMsg, HWND hwnd, WCHAR* path);

	bool SetShaderParameters(ID3D11DeviceContext* context, Mat4 world, Mat4 view, Mat4 projection, ID3D11ShaderResourceView* texture);
	void RenderShader(ID3D11DeviceContext* context, int indexcount);

	ComPtr<ID3D11VertexShader> m_VertexShader;
	ComPtr<ID3D11PixelShader> m_PixelShader;
	ComPtr<ID3D11InputLayout> m_InputLayout;
	ComPtr<ID3D11Buffer> m_MatrixBuffer;

	ComPtr < ID3D11SamplerState> m_SampleState;

};

