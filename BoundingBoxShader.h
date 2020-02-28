#pragma once

#include "commoninclude.h"
#include "LightClass.h"
 
class BoundingBoxShader
{
public:
	BoundingBoxShader();
	~BoundingBoxShader();

	string m_bbox_ps;
	string m_bbox_vs;
 
	struct MatrixBufferType
	{
		Mat4 world;
		Mat4 view;
		Mat4 Projection;
	};

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int indexCount, Mat4 world, Mat4 view, Mat4 proj );

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd );
	void ShutdownShader();
	void OutputShaderErrorMsg(ID3D10Blob* errMsg, HWND hwnd, const WCHAR* shaderFileName);
	bool SetShaderParameter(ID3D11DeviceContext* context, int indexCount, Mat4 world, Mat4 view, Mat4 proj );
	void RenderShader(ID3D11DeviceContext* context, int indexCount);

	ComPtr<ID3D11VertexShader> m_vertexShader;
	ComPtr<ID3D11PixelShader> m_pixelShader;
	ComPtr<ID3D11InputLayout> m_inputLayout;
	 
	ComPtr<ID3D11Buffer> m_matrixBuffer;
};

