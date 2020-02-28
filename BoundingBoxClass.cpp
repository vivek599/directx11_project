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
	
	m_BBoxVertexCount = size;

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
	
	result = m_BBoxShader->Render(context, m_BBoxVertexCount, world, view, proj );
	assert(result);

	return true;
}

bool BoundingBoxClass::InitializeBuffers(ID3D11Device* device, void* buffer, int size)
{
	D3D11_BUFFER_DESC bbox_vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA bbox_vertexData;
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
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

}

