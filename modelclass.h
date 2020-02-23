#pragma once

#include "commoninclude.h"
#include "BaseClass.h"

#include "ModelShaderClass.h"
#include "depthshaderclass.h"
#include "textureclass.h"
#include "LightClass.h"
#include "PointLightClass.h"
#include "RenderTextureClass.h"
#include "cameraclass.h"


class ModelClass : BaseClass
{
public:
	ModelClass(ID3D11Device* device, const char* modelFilename, const WCHAR* texpath, const WCHAR* normalpath);
	~ModelClass();

	void Shutdown();
	bool Render(ID3D11DeviceContext* context, RenderTextureClass* renderTexture, LightClass* light, CameraClass* camera, Mat4 world, Mat4 view, Mat4 proj, Mat4 lightViewMatrix, Mat4 lightProjectionMatrix, bool depthPass);
	void SetPosition(Vector3 position);
	void SetRotation(Vector3 rotation);
	void SetRotation(float angle, float radius, Vector3 axis);
	void SetScale3D(Vector3 scale);

	Vector3 GetPosition() { return m_Position; }

	int GetIndexCount();

	ComPtr<ID3D11ShaderResourceView> GetTexture();
	ID3D11ShaderResourceView** GetTextureArray();
	static int m_polygonCount;
protected:
private:

	struct VertexType
	{
		Vector3 position;
		Vector2 tex;
		Vector3 normal;
		Vector3 tangent;
		Vector3 binormal;
	};

	bool Initialize(ID3D11Device* device, const char* modelFilename, const WCHAR* textureFilename, const WCHAR* normalFilename);

	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* context);

	bool LoadTexture(ID3D11Device* device, const WCHAR* texturePath, const WCHAR* normalMapPath );
	ComPtr < ID3D11ShaderResourceView> m_textureArray[2];
	void ReleaseTexture();


	ComPtr<ID3D11Buffer> m_VertexBuffer;
	ComPtr<ID3D11Buffer> m_IndexBuffer;
	int m_VertexCount; 
	int m_IndexCount;

	unique_ptr<ModelShaderClass> m_ModelShader;
	unique_ptr<DepthShaderClass> m_depthShader;
	unique_ptr<TextureClass> m_Texture[2];

	bool LoadModel(const char* modelFilename);
	void ReleaseModel();

	// Data to fill
	vector <VertexType> vertices;
	vector <UINT> indices;

	Mat4 m_finalMatrix;
	Vector3 m_Position;
	Vector3 m_Rotation;
	Vector3 m_Scale3d;

	float m_rotationAngle;
	float m_rotationRadius;
	Vector3 m_rotationAxis;

};


