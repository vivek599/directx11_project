#pragma once

#include "commoninclude.h"
#include "LightClass.h"
 
class ModelShaderClass
{
public:
	ModelShaderClass();
	ModelShaderClass(const ModelShaderClass& obj);
	~ModelShaderClass();

 
	struct LightBufferType
	{
		Vector4 ambientColor;
		Vector4 diffuseColor;
		Vector3 lightDirection;
		float specularPower;
		Vector4 specularColor;
	};

	struct LightBufferType2
	{
		Vector3 lightPosition;
		float padding;
	};

	struct MatrixBufferType
	{
		Mat4 world;
		Mat4 view;
		Mat4 proj;
		Mat4 lightView;
		Mat4 lightProjection;
	};

	struct CameraBufferType
	{
		Vector3 cameraPosition;
		float padding;
	};
	
	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int indexCount, Mat4 world, Mat4 view, Mat4 proj, LightClass* light, Mat4 lightViewMatrix, 
		Mat4 lightProjectionMatrix,	ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView* depthMapTexture,	Vector3 cameraPosition);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName);
	void ShutdownShader();
	void OutputShaderErrorMsg(ID3D10Blob* errMsg, HWND hwnd, WCHAR* shaderFileName);
	bool SetShaderParameter(ID3D11DeviceContext* context, int indexCount, Mat4 world, Mat4 view, Mat4 proj, LightClass* light, Mat4 lightViewMatrix, Mat4 lightProjectionMatrix,
		ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView* depthMapTexture,	Vector3 cameraPosition );
	void RenderShader(ID3D11DeviceContext* context, int indexCount);

	ComPtr<ID3D11VertexShader> m_vertexShader;
	ComPtr<ID3D11PixelShader> m_pixelShader;
	ComPtr<ID3D11InputLayout> m_inputLayout;
	ComPtr<ID3D11SamplerState> m_samplerState;
	ComPtr<ID3D11SamplerState> m_sampleStateClamp;
	 
	ComPtr<ID3D11Buffer> m_matrixBuffer;
	ComPtr<ID3D11Buffer> m_lightBuffer;
	ComPtr<ID3D11Buffer> m_lightBuffer2;
	ComPtr<ID3D11Buffer> m_lightPositionBuffer;
	ComPtr<ID3D11Buffer> m_cameraBuffer;
};

