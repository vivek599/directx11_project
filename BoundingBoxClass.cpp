#include "BoundingBoxClass.h"

BoundingBoxClass::BoundingBoxClass(ID3D11Device* device, void* buffer, int size)
{
	Initialize(device, buffer, size);
}

BoundingBoxClass::~BoundingBoxClass()
{
	Shutdown();
}

bool BoundingBoxClass::Initialize(ID3D11Device* device, void* buffer, int size )
{
	bool result;
	
	//m_BBoxVertexCount = size;

	m_BBoxShader.reset(new BoundingBoxShader());
	assert(m_BBoxShader);

	result = m_BBoxShader->Initialize(device, GetActiveWindow());
	assert(result);

	result = InitializeBuffers(device, buffer, size);
	assert(result);

	return true;

}

void BoundingBoxClass::Shutdown()
{
	ShutdownBuffers();
}

bool BoundingBoxClass::Render(ID3D11DeviceContext* context, Mat4 world, Mat4 view, Mat4 proj )
{
	bool result;  
	
	RenderBuffers(context);
	
	result = m_BBoxShader->Render(context, m_IndexCount, world, view, proj );
	assert(result);

	return true;
}

bool BoundingBoxClass::InitializeBuffers(ID3D11Device* device, void* buffer, int size)
{
	D3D11_BUFFER_DESC bbox_vertexBufferDesc, bbox_indexBufferDesc;
	D3D11_SUBRESOURCE_DATA bbox_vertexData, bbox_indexData;
	HRESULT hr = S_OK;

	bbox_vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bbox_vertexBufferDesc.ByteWidth = sizeof(BBoxVertex) * size;
	bbox_vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bbox_vertexBufferDesc.CPUAccessFlags = 0;
	bbox_vertexBufferDesc.MiscFlags = 0;
	bbox_vertexBufferDesc.StructureByteStride = 0;

	bbox_vertexData.pSysMem = buffer;
	bbox_vertexData.SysMemPitch = 0;
	bbox_vertexData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&bbox_vertexBufferDesc, &bbox_vertexData, m_BBoxVertexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));
	
	indices.resize(36);

	// Front Face
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;

	// Back Face
	indices[6] = 4; indices[7] = 5; indices[8] = 6;
	indices[9] = 4; indices[10] = 6; indices[11] = 7;

	// Top Face
	indices[12] = 1; indices[13] = 7; indices[14] = 6;
	indices[15] = 1; indices[16] = 6; indices[17] = 2;

	// Bottom Face
	indices[18] = 0; indices[19] = 4; indices[20] = 5;
	indices[21] = 0; indices[22] = 5; indices[23] = 3;

	// Left Face
	indices[24] = 4; indices[25] = 7; indices[26] = 1;
	indices[27] = 4; indices[28] = 1; indices[29] = 0;

	// Rindicesght Face
	indices[30] = 3; indices[31] = 2; indices[32] = 6;
	indices[33] = 3; indices[34] = 6; indices[35] = 5;
	 

	m_IndexCount = indices.size();
	bbox_indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bbox_indexBufferDesc.ByteWidth = sizeof(UINT) * m_IndexCount;
	bbox_indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bbox_indexBufferDesc.CPUAccessFlags = 0;
	bbox_indexBufferDesc.MiscFlags = 0;
	bbox_indexBufferDesc.StructureByteStride = 0;

	bbox_indexData.pSysMem = &indices[0];
	bbox_indexData.SysMemPitch = 0;
	bbox_indexData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&bbox_indexBufferDesc, &bbox_indexData, m_BBoxIndexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;
}

void BoundingBoxClass::ShutdownBuffers()
{
	m_BBoxVertexBuffer.Reset();
}

void BoundingBoxClass::RenderBuffers(ID3D11DeviceContext* context)
{
	unsigned int stride = sizeof(BBoxVertex);
	unsigned int offset = 0;
					    
	context->IASetVertexBuffers(0, 1, m_BBoxVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_BBoxIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

}

