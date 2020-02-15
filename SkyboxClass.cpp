#include "SkyboxClass.h"



SkyboxClass::SkyboxClass()
{
 
	m_Texture = 0;
	m_skyboxShader = 0;
}


SkyboxClass::~SkyboxClass()
{
	Shutdown();
}

bool SkyboxClass::Initialize(ID3D11Device* device)
{
	bool result;

	// Create the sky dome shader object.
	m_skyboxShader.reset( new SkyboxShaderClass());
	assert(m_skyboxShader);


	// Initialize the sky dome shader object.
	result = m_skyboxShader->Initialize(device);
	assert(result);


	// Load in the sky dome model.
	result = LoadSkyboxModel("../Models/skybox/skybox1.obj"); 
	assert(result);


	// Load the sky dome into a vertex and index buffer for rendering.
	result = InitializeBuffers(device);
	assert(result);


	result = LoadTexture(device, (WCHAR*)L"../Models/skybox/skybox.png");
	assert(result);


	return true;
}

void SkyboxClass::Shutdown()
{

	ReleaseTexture();

	// Release the vertex and index buffer that were used for rendering the sky dome.
	ReleaseBuffers();

	// Release the sky dome model.
	ReleaseSkyboxModel();
}

bool SkyboxClass::Render(ID3D11DeviceContext* context, Mat4 world, Mat4 view, Mat4 proj, Vector3 cameraPosition)
{
	// Render the sky dome.
	bool result;
	RenderBuffers(context);

	// Translate the sky dome to be centered around the camera position.
	Mat4 m_finalMatrix = world * XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// Render the model using the model light shader.
	result = m_skyboxShader->Render(context, GetIndexCount(), m_finalMatrix, view, proj, GetTexture().Get() );
	if (!result)
	{
		return false;
	}

	return true;
}

int SkyboxClass::GetIndexCount()
{
	return m_indexCount;
}

ComPtr<ID3D11ShaderResourceView> SkyboxClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool SkyboxClass::LoadSkyboxModel(const char* filename)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (!pScene)
	{
		return false;
	}

	aiMesh* mesh = pScene->mMeshes[0];

	m_vertexCount = mesh->mNumVertices;
	// Walk through each of the mesh's vertices
	for (UINT i = 0; i < m_vertexCount; i++)
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

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		if (face.mNumIndices != 3)
		{
			MessageBox(GetActiveWindow(), L"FACES 3 indices", L"Indices", MB_OK);
			return false;
		}

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	m_indexCount = indices.size();
	return true;
}

void SkyboxClass::ReleaseSkyboxModel()
{
	vertices.clear();
	indices.clear();
}

bool SkyboxClass::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = &vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, m_vertexBuffer.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = &indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, m_indexBuffer.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}


	return true;

}

void SkyboxClass::ReleaseBuffers()
{
	// Release the index buffer.
	m_indexBuffer.Reset();

	// Release the vertex buffer.
	m_vertexBuffer.Reset();
}

void SkyboxClass::RenderBuffers(ID3D11DeviceContext* context)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool SkyboxClass::LoadTexture(ID3D11Device* device, const WCHAR* path)
{
	bool result;

	m_Texture.reset(new TextureClass());
	if (!m_Texture)
	{
		return false;
	}

	result = m_Texture->Initialize(device, path);
	if (!result)
	{
		return false;
	}

	return true;
}

void SkyboxClass::ReleaseTexture()
{

}
