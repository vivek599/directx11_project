#include "bitmapclass.h"


BitmapClass::BitmapClass()
{
 
	m_Texture = 0;
	m_BitmapShader = 0;
}

BitmapClass::~BitmapClass()
{
	Shutdown();
}

bool BitmapClass::Initialize(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight, WCHAR* path, int bitmapWidth, int bitmapHeight)
{
	bool result;
	
	m_BitmapShader.reset( new BitmapShaderClass());
	assert(m_BitmapShader);

	result = m_BitmapShader->Initialize(device, hwnd);
	assert(result);



	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;

	m_BitmapWidth = bitmapWidth;
	m_BitmapHeight = bitmapHeight;

	m_PrevPosX = -1;
	m_PrevPosY = -1;

	result = InitializeBuffers(device);
	assert(result);


	result = LoadTexture(device, path);
	assert(result);


	return true;

}

void BitmapClass::Shutdown()
{
	ReleaseTexture();

	ShutdownBuffers();

}

bool BitmapClass::Render(ID3D11DeviceContext* context, Mat4 world, Mat4 view, Mat4 ortho, int posX, int posY, ID3D11ShaderResourceView* texture)
{
	bool result;

	result = UpdateBuffers(context, posX, posY);
	if (!result)
	{
		return false;
	}

	RenderBuffers(context);
	
	result = m_BitmapShader->Render(context, GetIndexCount(), world, view, ortho, texture);
	if (!result)
	{
		return false;
	}

	return true;

}

int BitmapClass::GetIndexCount()
{
	return m_IndexCount;

}

ComPtr<ID3D11ShaderResourceView> BitmapClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	m_VertexCount = 6;

	m_IndexCount = m_VertexCount;

	vertices = new VertexType[m_VertexCount];
	if (!vertices)
	{
		return false;
	}

	indices = new unsigned long[m_IndexCount];
	if (!indices)
	{
		return false;
	}

	memset(vertices, 0, sizeof(VertexType)*m_VertexCount);

	for (i = 0; i < m_IndexCount; i++)
	{
		indices[i] = i;
	}


// 	vertices[0].position = Vector3(-1.f, -1.f, 0.f);
// 	vertices[1].position = Vector3(0.f, 1.f, 0.f);
// 	vertices[2].position = Vector3(1.f, -1.f, 0.f);
// 
// 	vertices[0].texture = Vector2(0.f, 1.f);
// 	vertices[1].texture = Vector2(0.5f, 0.f);
// 	vertices[2].texture = Vector2(1.f, 1.f);


	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, m_VertexBuffer.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, m_IndexBuffer.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;


}

void BitmapClass::ShutdownBuffers()
{
	m_IndexBuffer.Reset();

	m_VertexBuffer.Reset();
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* context, int posX, int posY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* vertexPtr;
	HRESULT hr;

	if( (posX == m_PrevPosX) && (posY == m_PrevPosY) )
	{
		return true;
	}

	m_PrevPosX = posX;
	m_PrevPosY = posY;

	left = (float)((m_ScreenWidth / 2) * -1) + (float)posX;
	right = left + (float)m_BitmapWidth;
	top = (float)(m_ScreenHeight / 2) - (float)posY;
	bottom = top - (float)m_BitmapHeight;

	vertices = new VertexType[m_VertexCount];
	if (!vertices)
	{
		return false;
	}


	//triangle 1 - clockwise
	vertices[0].position = Vector3( left, top, 0.f );
	vertices[0].texture = Vector2(0.f, 0.f);
	
	vertices[1].position = Vector3(right, bottom, 0.f);
	vertices[1].texture = Vector2(1.f, 1.f);
	
	vertices[2].position = Vector3(left, bottom, 0.f);
	vertices[2].texture = Vector2(0.f, 1.f);
	
	//triangle 2
	vertices[3].position = Vector3(left, top, 0.f);
	vertices[3].texture = Vector2(0.f, 0.f);
	
	vertices[4].position = Vector3(right, top, 0.f);
	vertices[4].texture = Vector2(1.f, 0.f);
	
	vertices[5].position = Vector3(right, bottom, 0.f);
	vertices[5].texture = Vector2(1.f, 1.f);

	hr = context->Map(m_VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}

	vertexPtr = (VertexType*)mappedResource.pData;

	memcpy(vertexPtr, (void*)vertices, sizeof(VertexType)*m_VertexCount );

	context->Unmap(m_VertexBuffer.Get(), 0);

	delete[] vertices;
	vertices = 0;

	return true;

}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* context)
{
	unsigned int stride, offset;

	stride = sizeof(VertexType);
	offset = 0;

	context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


}

bool BitmapClass::LoadTexture(ID3D11Device* device, WCHAR* path)
{
	bool result;

	m_Texture.reset( new TextureClass());
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

void BitmapClass::ReleaseTexture()
{


}

