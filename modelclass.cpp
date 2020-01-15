#include "modelclass.h"

int ModelClass::m_polygonCount = 0;

ModelClass::ModelClass(const ModelClass& obj)
{
	UNREFERENCED_PARAMETER(obj);

}

ModelClass::ModelClass(ID3D11Device* device, const char* modelFilename, const WCHAR* texpath, const WCHAR* normalpath)
{

	m_Texture[0] = 0;
	m_Texture[1] = 0;
	m_ModelShader = 0;
	m_depthShader = 0;
	m_rotationAngle = 0.f;
	m_rotationAxis = Vector3(0.f, 1.f, 0.f);
	m_bRotateAboutAxis = false;
	m_Position = Vector3(0.f);
	m_Rotation = Vector3(0.f);
	m_Scale3d = Vector3(1.f);
	m_rotationRadius = 0;

	Initialize(device, modelFilename, texpath, normalpath);
}

ModelClass::~ModelClass()
{

}

bool ModelClass::Initialize(ID3D11Device* device, const char* modelFilename, const WCHAR* textureFilename, const WCHAR* normalFilename)
{
	bool result;

	//MessageBox(NULL, GetOpenFilePathFromDialog().c_str(), L"File Path", MB_OK);

	// Create the light shader object.
	m_ModelShader = new ModelShaderClass;
	if (!m_ModelShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_ModelShader->Initialize(device, GetActiveWindow());
	if (!result)
	{
		MessageBox(GetActiveWindow(), L"Could not initialize the ModelShaderClass object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_depthShader = new DepthShaderClass;
	if (!m_depthShader)
	{
		return false;
	}

	// Initialize the m_depthShader shader object.
	result = m_depthShader->Initialize(device, GetActiveWindow());
	if (!result)
	{
		MessageBox(GetActiveWindow(), L"Could not initialize the DepthShaderClass object.", L"Error", MB_OK);
		return false;
	}

	// Load in the model data,
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	result = LoadTexture(device, textureFilename, normalFilename);
	if (!result)
	{
		return false;
	}

	return true;

}

void ModelClass::Shutdown()
{
	if (m_ModelShader)
	{
		m_ModelShader->Shutdown();
		delete m_ModelShader;
		m_ModelShader = 0;

	}

	if (m_depthShader)
	{
		m_depthShader->Shutdown();
		delete m_depthShader;
		m_depthShader = 0;

	}

	ReleaseTexture();

	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();
}

bool ModelClass::Render(ID3D11DeviceContext* context, RenderTextureClass* renderTexture, LightClass* light, CameraClass* camera, 
	Mat4 world, Mat4 view, Mat4 proj, Mat4 lightViewMatrix, Mat4 lightProjectionMatrix, bool depthPass)
{
	bool result;  
	
	RenderBuffers(context);

	m_finalMatrix = world * XMMatrixRotationX(m_Rotation.x)* XMMatrixRotationY(m_Rotation.y)* XMMatrixRotationZ(m_Rotation.z)
						  * XMMatrixScaling(m_Scale3d.x, m_Scale3d.y, m_Scale3d.z)
						  * XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	
	if (depthPass)
	{
		// Render the model using the depth shader.
		result = m_depthShader->Render(context, GetIndexCount(), m_finalMatrix, lightViewMatrix, lightProjectionMatrix);
		if (!result)
		{
			return false;
		}

	}
	else
	{
		// Render the model using the model light shader.
		result = m_ModelShader->Render( context, GetIndexCount(), m_finalMatrix, view, proj, light, lightViewMatrix, lightProjectionMatrix, GetTextureArray(), renderTexture->GetShaderResourceView().Get(), camera->GetPosition() );
		if (!result)
		{
			return false;
		}
	}

	return true;
}

void ModelClass::SetPosition(Vector3 position)
{
	m_Position = position;
}

void ModelClass::SetRotation(Vector3 rotation)
{
	m_bRotateAboutAxis = false;
	m_Rotation = rotation;
}

void ModelClass::SetRotation(float angle, float radius,  Vector3 axis)
{
	m_bRotateAboutAxis = true;
	m_rotationAxis = axis;// rotate(mat4 model(1.0), angle, rotation_axis);
	m_rotationAngle = angle;
	m_rotationRadius = radius;
}

void ModelClass::SetScale3D(Vector3 scale)
{
	m_Scale3d = scale;
}

int ModelClass::GetIndexCount()
{
	return m_IndexCount;

}

ComPtr<ID3D11ShaderResourceView> ModelClass::GetTexture()
{
	return m_Texture[0]->GetTexture();
}

ID3D11ShaderResourceView** ModelClass::GetTextureArray()
{
	//ID3D11ShaderResourceView* texArr[] = { m_Texture[0]->GetTexture(), m_Texture[1]->GetTexture() };

	return m_textureArray[0].GetAddressOf();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = &vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, m_VertexBuffer.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * m_IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = &indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, m_IndexBuffer.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ModelClass::ShutdownBuffers()
{
	m_IndexBuffer.Reset();

	m_VertexBuffer.Reset();

}

void ModelClass::RenderBuffers(ID3D11DeviceContext* context)
{
	unsigned int stride, offset;

	stride = sizeof(VertexType);
	offset = 0;

	context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

bool ModelClass::LoadTexture(ID3D11Device* device, const WCHAR* texturePath, const WCHAR* normalMapPath)
{
	bool result;

	m_Texture[0] = new TextureClass;
	if (!m_Texture[0])
	{
		return false;
	}

	result = m_Texture[0]->Initialize(device, texturePath);
	if (!result)
	{
		return false;
	}

	m_Texture[1] = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	result = m_Texture[1]->Initialize(device, normalMapPath);
	if (!result)
	{
		return false;
	}

	m_textureArray[0] = m_Texture[0]->GetTexture();
	m_textureArray[1] = m_Texture[1]->GetTexture();

	return true;

}

void ModelClass::ReleaseTexture()
{
	if (m_Texture[0])
	{
		m_Texture[0]->Shutdown();
		delete m_Texture[0];
		m_Texture[0] = 0;
	}

	if (m_Texture[1])
	{
		m_Texture[1]->Shutdown();
		delete m_Texture[1];
		m_Texture[1] = 0;
	}

}

bool ModelClass::LoadModel(const char* modelFilename)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(modelFilename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded );
	if (!pScene)
	{
		return false;
	}

	aiMesh* mesh = pScene->mMeshes[0];

	m_VertexCount = mesh->mNumVertices;
	// Walk through each of the mesh's vertices
	for (UINT i = 0; i < m_VertexCount; i++)
	{
		VertexType vertex;

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		//if (mesh->mTextureCoords[0])
		{
			vertex.tex.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.tex.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		vertex.tangent.x = mesh->mTangents[i].x;
		vertex.tangent.y = mesh->mTangents[i].y;
		vertex.tangent.z = mesh->mTangents[i].z;

		vertex.binormal.x = mesh->mBitangents[i].x;
		vertex.binormal.y = mesh->mBitangents[i].y;
		vertex.binormal.z = mesh->mBitangents[i].z;

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		if (face.mNumIndices != 3 )
		{
			MessageBox( GetActiveWindow(),L"FACES 3 indices", L"Indices", MB_OK );
			return false;
		}

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	m_IndexCount = indices.size();
	m_polygonCount += mesh->mNumFaces;
	return true;
}

void ModelClass::ReleaseModel()
{
	vertices.clear();
	indices.clear();
}
