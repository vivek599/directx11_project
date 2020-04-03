#pragma once

#include "d3dclass.h"

using namespace DirectX;

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#define PATH_DEFAULT_PIXEL_SHADER		L"../shaders/ps_texure.hlsl"
#define PATH_DEFAULT_VERTEX_SHADER		L"../shaders/vs_texure.hlsl"
#define PATH_DEFAULT_TEXTURE			L"../Textures/PaperCrumpled0033_S.jpg"

class ATexture
{
private:
	bool CompileVertexShader( const wchar_t* filepath);
	bool CompilePixelShader( const wchar_t* filepath);
	bool LoadTexture( );
	bool CreateBuffer( );
	bool UpdateBuffer(Mat4 world, Mat4 view, Mat4 proj);
	bool CreateInputLayout( );
	bool CreateSamplerState( );

	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
	};

	const static int m_vertexCount = 6;
	float scale = 3.0f;
	Vertex quadVertices[m_vertexCount] =
	{
		{ { -2.0f * scale,  1.0f * scale, 0.0f },{ 0.0f, 0.0f } },
		{ {  2.0f * scale, -1.0f * scale, 0.0f },{ 1.0f, 1.0f } },
		{ { -2.0f * scale, -1.0f * scale, 0.0f },{ 0.0f, 1.0f } },

		{ { -2.0f * scale,  1.0f * scale, 0.0f },{ 0.0f, 0.0f } },
		{ {  2.0f * scale,  1.0f * scale, 0.0f },{ 1.0f, 0.0f } },
		{ {  2.0f * scale, -1.0f * scale, 0.0f },{ 1.0f, 1.0f } }
	};

	ComPtr<ID3D11Texture2D>					m_textureResource;
	ComPtr<ID3D11ShaderResourceView>		m_textureSrv;

	ComPtr<ID3D11VertexShader>				m_vertexShader;
	ComPtr<ID3D11PixelShader>				m_pixelShader;
	ComPtr<ID3D11InputLayout>				m_inputLayout;
	ComPtr<ID3D11Buffer>					m_matrixBuffer;
	ComPtr<ID3D11SamplerState>				m_sampleState;

	ComPtr<ID3D10Blob>						m_vsByteCode;
	ComPtr<ID3D10Blob>						m_psByteCode; 

	ComPtr<ID3D11Buffer>					m_vertexBuffer;
	ComPtr<ID3D11Buffer>					m_indexBuffer;
	ComPtr<ID3D11Device>					m_device;
	ComPtr<ID3D11DeviceContext>				m_context;

	ComPtr<IDXGISurface>					m_textSurface;

	UINT			m_width;
	UINT			m_height;
	XMFLOAT3		m_scale;
public:
	ATexture( D3DClass* d3d,
		const wchar_t* vertexShaderFilepath = PATH_DEFAULT_VERTEX_SHADER,
		const wchar_t* pixelShaderFilepath = PATH_DEFAULT_PIXEL_SHADER
	);

	~ATexture();

	void Update(Mat4 world, Mat4 view, Mat4 proj);
	void Render(D3DClass* d3d ) const;
	void SetPosition(Vector3 position );
	void SetScale3D(float scaleX, float scaleY, float scaleZ);
	void SetScale(float scale);
	Vector4 GetPosition() const { return XMLoadFloat3(&m_position); }
	IDXGISurface* GetDXGISurface();

};

