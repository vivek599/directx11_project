#include "modelclass.h"

int ModelClass::m_polygonCount = 0;

ModelClass::BBox ModelClass::GetBBox()
{
	BBox box = m_box;

	box.min += m_Position;
	box.max += m_Position;
	
	return box;
}

ModelClass::ModelClass(ID3D11Device* device, const char* modelFilename, const WCHAR* texpath, const WCHAR* normalpath)
{
	m_box = {};
	m_Texture[0] = 0;
	m_Texture[1] = 0;
	m_ModelShader = 0;
	m_depthShader = 0;
	m_rotationAngle = 0.f;
	m_rotationAxis = Vector3(0.f, 1.f, 0.f);
	m_Position = Vector3(0.f);
	m_Rotation = Vector3(0.f);
	m_Scale3d = Vector3(1.f);
	m_rotationRadius = 0;

	Initialize(device, modelFilename, texpath, normalpath);
}

ModelClass::~ModelClass()
{
	Shutdown();
}

bool ModelClass::Initialize(ID3D11Device* device, const char* modelFilename, const WCHAR* textureFilename, const WCHAR* normalFilename)
{
	bool result;

	//MessageBox(NULL, GetOpenFilePathFromDialog().c_str(), L"File Path", MB_OK);

	// Create the light shader object.
	m_ModelShader.reset( new ModelShaderClass());
	assert(m_ModelShader);


	// Initialize the light shader object.
	result = m_ModelShader->Initialize(device, GetActiveWindow());
	assert(result);


	// Create the light shader object.
	m_depthShader.reset(new DepthShaderClass());
	assert(m_depthShader);


	// Initialize the m_depthShader shader object.
	result = m_depthShader->Initialize(device, GetActiveWindow());
	assert(result);


	// Load in the model data,
	result = LoadModel(modelFilename);
	assert(result);


	result = InitializeBuffers(device);
	assert(result);

	m_BBox.reset(new BoundingBoxClass( device, bbox_vertices.data(), bbox_vertices.size()));

	result = LoadTexture(device, textureFilename, normalFilename);
	assert(result);


	return true;

}

void ModelClass::Shutdown()
{
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

	m_finalMatrix = world * XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z)
						  * XMMatrixScaling(m_Scale3d.x, m_Scale3d.y, m_Scale3d.z)
						  * XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	
	if (depthPass)
	{
		// Render the model using the depth shader.
		result = m_depthShader->Render(context, GetIndexCount(), m_finalMatrix, lightViewMatrix, lightProjectionMatrix, camera->GetPosition());
		assert(result);

	}
	else
	{
		// Render the model using the model light shader.
		result = m_ModelShader->Render( context, GetIndexCount(), m_finalMatrix, view, proj, light, lightViewMatrix, lightProjectionMatrix, GetTextureArray(), renderTexture->GetShaderResourceView().Get(), camera->GetPosition() );
		assert(result);

		result = m_BBox->Render(context, m_finalMatrix, view, proj);
		assert(result);

	}

	return true;
}

void ModelClass::SetPosition(Vector3 position)
{
	m_Position = position;
}

void ModelClass::SetRotation(Vector3 rotation)
{
	m_Rotation = Vector3(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z) );
}

void ModelClass::SetRotation(float angle, float radius,  Vector3 axis)
{
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
	HRESULT hr = S_OK;
 

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = &vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, m_VertexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));


	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * m_IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = &indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&indexBufferDesc, &indexData, m_IndexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));



	return true;
}

void ModelClass::ShutdownBuffers()
{
	m_IndexBuffer.Reset();

	m_VertexBuffer.Reset();

}

void ModelClass::RenderBuffers(ID3D11DeviceContext* context)
{
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

bool ModelClass::LoadTexture(ID3D11Device* device, const WCHAR* texturePath, const WCHAR* normalMapPath)
{
	bool result;

	m_Texture[0].reset( new TextureClass());
	if (!m_Texture[0])
	{
		return false;
	}

	result = m_Texture[0]->Initialize(device, texturePath);
	if (!result)
	{
		return false;
	}

	m_Texture[1].reset(new TextureClass());
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


}

bool ModelClass::LoadModel(const char* modelFilename)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(modelFilename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (!pScene)
	{
		return false;
	}

	aiMesh* mesh = pScene->mMeshes[0];

	m_VertexCount = mesh->mNumVertices;

	m_box.max.x = m_box.min.x = mesh->mVertices[0].x;
	m_box.max.y = m_box.min.y = mesh->mVertices[0].y;
	m_box.max.z = m_box.min.z = mesh->mVertices[0].z;

	// Walk through each of the mesh's vertices
	for (UINT i = 0; i < m_VertexCount; i++)
	{
		VertexType vertex;

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		if (vertex.position.x < m_box.min.x) m_box.min.x = vertex.position.x;
		if (vertex.position.y < m_box.min.y) m_box.min.y = vertex.position.y;
		if (vertex.position.z < m_box.min.z) m_box.min.z = vertex.position.z;
		if (vertex.position.x > m_box.max.x) m_box.max.x = vertex.position.x;
		if (vertex.position.y > m_box.max.y) m_box.max.y = vertex.position.y;
		if (vertex.position.z > m_box.max.z) m_box.max.z = vertex.position.z;

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
		if (face.mNumIndices != 3)
		{
			MessageBox(NULL, L"FACES 3 indices", L"Indices", MB_OK);
			return false;
		}

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	m_IndexCount = indices.size();
	m_polygonCount += mesh->mNumFaces;

	bbox_vertices =
	{
		{ XMFLOAT3(m_box.min.x, m_box.min.y, m_box.min.z) },
		{ XMFLOAT3(m_box.min.x, m_box.max.y, m_box.min.z) },
		{ XMFLOAT3(m_box.max.x, m_box.max.y, m_box.min.z) },
		{ XMFLOAT3(m_box.max.x, m_box.min.y, m_box.min.z) },
		{ XMFLOAT3(m_box.min.x, m_box.min.y, m_box.max.z) },
		{ XMFLOAT3(m_box.max.x, m_box.min.y, m_box.max.z) },
		{ XMFLOAT3(m_box.max.x, m_box.max.y, m_box.max.z) },
		{ XMFLOAT3(m_box.min.x, m_box.max.y, m_box.max.z) }
#if 0
		{ XMFLOAT3(m_box.max.x, m_box.max.y, m_box.min.z)},
		{ XMFLOAT3(m_box.min.x, m_box.max.y, m_box.min.z)},
		{ XMFLOAT3(m_box.min.x, m_box.max.y, m_box.min.z)},
		{ XMFLOAT3(m_box.min.x, m_box.max.y, m_box.max.z)},
		{ XMFLOAT3(m_box.min.x, m_box.max.y, m_box.max.z)},
		{ XMFLOAT3(m_box.max.x, m_box.max.y, m_box.max.z)},
		{ XMFLOAT3(m_box.max.x, m_box.max.y, m_box.max.z)},
		{ XMFLOAT3(m_box.max.x, m_box.max.y, m_box.min.z)},
		{ XMFLOAT3(m_box.max.x, m_box.min.y, m_box.max.z)},
		{ XMFLOAT3(m_box.min.x, m_box.min.y, m_box.max.z)},
		{ XMFLOAT3(m_box.min.x, m_box.min.y, m_box.max.z)},
		{ XMFLOAT3(m_box.min.x, m_box.min.y, m_box.min.z)},
		{ XMFLOAT3(m_box.min.x, m_box.min.y, m_box.min.z)},
		{ XMFLOAT3(m_box.max.x, m_box.min.y, m_box.min.z)},
		{ XMFLOAT3(m_box.max.x, m_box.min.y, m_box.min.z)},
		{ XMFLOAT3(m_box.max.x, m_box.min.y, m_box.max.z)},
		{ XMFLOAT3(m_box.max.x, m_box.max.y, m_box.max.z)},
		{ XMFLOAT3(m_box.min.x, m_box.max.y, m_box.max.z)},
		{ XMFLOAT3(m_box.min.x, m_box.max.y, m_box.max.z)},
		{ XMFLOAT3(m_box.min.x, m_box.min.y, m_box.max.z)},
		{ XMFLOAT3(m_box.min.x, m_box.min.y, m_box.max.z)},
		{ XMFLOAT3(m_box.max.x, m_box.min.y, m_box.max.z)},
		{ XMFLOAT3(m_box.max.x, m_box.min.y, m_box.max.z)},
		{ XMFLOAT3(m_box.max.x, m_box.max.y, m_box.max.z)},
		{ XMFLOAT3(m_box.max.x, m_box.min.y, m_box.min.z)},
		{ XMFLOAT3(m_box.min.x, m_box.min.y, m_box.min.z)},
		{ XMFLOAT3(m_box.min.x, m_box.min.y, m_box.min.z)},
		{ XMFLOAT3(m_box.min.x, m_box.max.y, m_box.min.z)},
		{ XMFLOAT3(m_box.min.x, m_box.max.y, m_box.min.z)},
		{ XMFLOAT3(m_box.max.x, m_box.max.y, m_box.min.z)},
		{ XMFLOAT3(m_box.max.x, m_box.max.y, m_box.min.z)},
		{ XMFLOAT3(m_box.max.x, m_box.min.y, m_box.min.z)},
		{ XMFLOAT3(m_box.min.x, m_box.max.y, m_box.max.z)},
		{ XMFLOAT3(m_box.min.x, m_box.max.y, m_box.min.z)},
		{ XMFLOAT3(m_box.min.x, m_box.max.y, m_box.min.z)},
		{ XMFLOAT3(m_box.min.x, m_box.min.y, m_box.min.z)},
		{ XMFLOAT3(m_box.min.x, m_box.min.y, m_box.min.z)},
		{ XMFLOAT3(m_box.min.x, m_box.min.y, m_box.max.z)},
		{ XMFLOAT3(m_box.min.x, m_box.min.y, m_box.max.z)},
		{ XMFLOAT3(m_box.min.x, m_box.max.y, m_box.max.z)},
		{ XMFLOAT3(m_box.max.x, m_box.max.y, m_box.min.z)},
		{ XMFLOAT3(m_box.max.x, m_box.max.y, m_box.max.z)},
		{ XMFLOAT3(m_box.max.x, m_box.max.y, m_box.max.z)},
		{ XMFLOAT3(m_box.max.x, m_box.min.y, m_box.max.z)},
		{ XMFLOAT3(m_box.max.x, m_box.min.y, m_box.max.z)},
		{ XMFLOAT3(m_box.max.x, m_box.min.y, m_box.min.z)},
		{ XMFLOAT3(m_box.max.x, m_box.min.y, m_box.min.z)},
		{ XMFLOAT3(m_box.max.x, m_box.max.y, m_box.min.z)}
#endif
	};
	//{
		//{ XMFLOAT3(1.0f, 1.0f, -1.0f)},
		//{ XMFLOAT3(-1.0f, 1.0f, -1.0f)},

		//{ XMFLOAT3(-1.0f, 1.0f, -1.0f)},
		//{ XMFLOAT3(-1.0f, 1.0f, 1.0f)},

		//{ XMFLOAT3(-1.0f, 1.0f, 1.0f)},
		//{ XMFLOAT3(1.0f, 1.0f, 1.0f)},

		//{ XMFLOAT3(1.0f, 1.0f, 1.0f)},
		//{ XMFLOAT3(1.0f, 1.0f, -1.0f)},

		//// Bottom face { XMFLOAT3(y = -1.0f)

		//{ XMFLOAT3(1.0f, -1.0f, 1.0f)},
		//{ XMFLOAT3(-1.0f, -1.0f, 1.0f)},

		//{ XMFLOAT3(-1.0f, -1.0f, 1.0f)},
		//{ XMFLOAT3(-1.0f, -1.0f, -1.0f)},

		//{ XMFLOAT3(-1.0f, -1.0f, -1.0f)},
		//{ XMFLOAT3(1.0f, -1.0f, -1.0f)},

		//{ XMFLOAT3(1.0f, -1.0f, -1.0f)},
		//{ XMFLOAT3(1.0f, -1.0f, 1.0f)},

		//// Front face  { XMFLOAT3(z = 1.0f)

		//{ XMFLOAT3(1.0f, 1.0f, 1.0f)},
		//{ XMFLOAT3(-1.0f, 1.0f, 1.0f)},

		//{ XMFLOAT3(-1.0f, 1.0f, 1.0f)},
		//{ XMFLOAT3(-1.0f, -1.0f, 1.0f)},

		//{ XMFLOAT3(-1.0f, -1.0f, 1.0f)},
		//{ XMFLOAT3(1.0f, -1.0f, 1.0f)},

		//{ XMFLOAT3(1.0f, -1.0f, 1.0f)},
		//{ XMFLOAT3(1.0f, 1.0f, 1.0f)},

		//// Back face { XMFLOAT3(z = -1.0f)

		//{ XMFLOAT3(1.0f, -1.0f, -1.0f)},
		//{ XMFLOAT3(-1.0f, -1.0f, -1.0f)},

		//{ XMFLOAT3(-1.0f, -1.0f, -1.0f)},
		//{ XMFLOAT3(-1.0f, 1.0f, -1.0f)},

		//{ XMFLOAT3(-1.0f, 1.0f, -1.0f)},
		//{ XMFLOAT3(1.0f, 1.0f, -1.0f)},

		//{ XMFLOAT3(1.0f, 1.0f, -1.0f)},
		//{ XMFLOAT3(1.0f, -1.0f, -1.0f)},

		//// Left face { XMFLOAT3(x = -1.0f)

		//{ XMFLOAT3(-1.0f, 1.0f, 1.0f)},
		//{ XMFLOAT3(-1.0f, 1.0f, -1.0f)},

		//{ XMFLOAT3(-1.0f, 1.0f, -1.0f)},
		//{ XMFLOAT3(-1.0f, -1.0f, -1.0f)},

		//{ XMFLOAT3(-1.0f, -1.0f, -1.0f)},
		//{ XMFLOAT3(-1.0f, -1.0f, 1.0f)},

		//{ XMFLOAT3(-1.0f, -1.0f, 1.0f)},
		//{ XMFLOAT3(-1.0f, 1.0f, 1.0f)},

		//// Right face { XMFLOAT3(x = 1.0f)

		//{ XMFLOAT3(1.0f, 1.0f, -1.0f)},
		//{ XMFLOAT3(1.0f, 1.0f, 1.0f)},

		//{ XMFLOAT3(1.0f, 1.0f, 1.0f)},
		//{ XMFLOAT3(1.0f, -1.0f, 1.0f)},

		//{ XMFLOAT3(1.0f, -1.0f, 1.0f)},
		//{ XMFLOAT3(1.0f, -1.0f, -1.0f)},

		//{ XMFLOAT3(1.0f, -1.0f, -1.0f)},
		//{ XMFLOAT3(1.0f, 1.0f, -1.0f)}
	//};

	return true;
}

void ModelClass::ReleaseModel()
{
	vertices.clear();
	indices.clear();
}

