#pragma once

#include "SkyboxShaderClass.h"
#include "textureclass.h"

class SkyboxClass
{
public:
	SkyboxClass();
	~SkyboxClass();

	bool Initialize(ID3D11Device* device);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, Mat4 world, Mat4 view, Mat4 proj, Vector3 cameraPositon);
	int GetIndexCount();

	ComPtr<ID3D11ShaderResourceView> GetTexture();

private:
	struct VertexType
	{
		Vector3 position;
		Vector2 tex;
		Vector3 normal;
	};

	bool LoadSkyboxModel(const char* filename);
	void ReleaseSkyboxModel();
	bool InitializeBuffers(ID3D11Device* device);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext* context);

	bool LoadTexture(ID3D11Device* device, const WCHAR* path);
	void ReleaseTexture();

	int m_vertexCount, m_indexCount;
	ComPtr<ID3D11Buffer> m_vertexBuffer; 
	ComPtr<ID3D11Buffer> m_indexBuffer;

	// Data to fill
	vector <VertexType> vertices;
	vector <UINT> indices;

	unique_ptr<SkyboxShaderClass> m_skyboxShader;
	unique_ptr<TextureClass> m_Texture;

};

