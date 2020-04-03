#include "ATexture.h" 

bool ATexture::CompileVertexShader( const wchar_t* filepath)
{
	HRESULT hr = S_OK;

	ComPtr<ID3D10Blob> errMsg;

	hr = D3DCompileFromFile(filepath, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &m_vsByteCode, &errMsg);
	if (FAILED(hr))
	{
		if (errMsg)
		{
			MessageBoxA( NULL, (LPCSTR)errMsg->GetBufferPointer(), "Error", MB_OK );
		}
		else
		{
			MessageBoxW(NULL, filepath, L"Missing vs shader file...", MB_OK);
		}
		return false;
	}

	hr = m_device->CreateVertexShader(m_vsByteCode->GetBufferPointer(), m_vsByteCode->GetBufferSize(), NULL, m_vertexShader.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool ATexture::CompilePixelShader( const wchar_t* filepath)
{
	HRESULT hr = S_OK; 

	ComPtr<ID3D10Blob> errMsg;

	//compile pixel shader
	hr = D3DCompileFromFile(filepath, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &m_psByteCode, &errMsg);

	if (FAILED(hr))
	{
		if (errMsg)
		{
			MessageBoxA(NULL, (LPCSTR)errMsg->GetBufferPointer(), "Error", MB_OK);
		}
		else
		{
			MessageBoxW(NULL, filepath, L"Missing ps shader file...", MB_OK);
		}
		return false;
	}

	hr = m_device->CreatePixelShader(m_psByteCode->GetBufferPointer(), m_psByteCode->GetBufferSize(), NULL, m_pixelShader.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool ATexture::LoadTexture()
{ 

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = 2048;
	desc.Height = 2048;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	float onePixel[] = { 0.0f,0.0f,0.0f,0.0f };
	initData.pSysMem = onePixel;
	initData.SysMemPitch = 1;
	initData.SysMemSlicePitch = 1;

	HRESULT hr = m_device->CreateTexture2D(&desc, nullptr, &m_textureResource);
	if (SUCCEEDED(hr) && m_textureResource != 0)
	{
		if (&m_textureSrv != nullptr)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			memset(&SRVDesc, 0, sizeof(SRVDesc));
			SRVDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;

			hr = m_device->CreateShaderResourceView(m_textureResource.Get(), &SRVDesc, m_textureSrv.ReleaseAndGetAddressOf());
			if (FAILED(hr))
			{
				m_textureResource->Release();
				return false;
			}
		}
	}

	return true;
}

bool ATexture::CreateBuffer( )
{ 

	D3D11_BUFFER_DESC matrixbufferDesc;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	matrixbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferDesc.ByteWidth = sizeof(XMMATRIX) * 3;
	matrixbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferDesc.MiscFlags = 0;
	matrixbufferDesc.StructureByteStride = 0;
	
	HRESULT hr = m_device->CreateBuffer(&matrixbufferDesc, NULL, m_matrixBuffer.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = quadVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hr = m_device->CreateBuffer(&vertexBufferDesc, &vertexData, m_vertexBuffer.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	unsigned long indices[] = { 0, 1, 2, 3, 4, 5 };

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_vertexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	hr = m_device->CreateBuffer(&indexBufferDesc, &indexData, m_indexBuffer.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool ATexture::UpdateBuffer(Mat4 world, Mat4 view, Mat4 proj )
{ 
	Mat4 worldlocal = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z ) * XMMatrixTranslation( m_position.x, m_position.y, m_position.z );

	HRESULT hr = S_OK;

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	Mat4 mat_world	= XMMatrixTranspose(worldlocal * world);
	Mat4 mat_view	= XMMatrixTranspose(view);
	Mat4 mat_proj	= XMMatrixTranspose(proj);

	hr = m_context->Map(m_matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}

	Mat4* dataPtr = (Mat4*)mappedResource.pData;

	dataPtr[0] = mat_world;
	dataPtr[1] = mat_view;
	dataPtr[2] = mat_proj;

	m_context->Unmap(m_matrixBuffer.Get(), 0);




	return true;

}

bool ATexture::CreateInputLayout( )
{
	HRESULT hr = S_OK; 

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

	//layout
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	const int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	hr = m_device->CreateInputLayout(polygonLayout, numElements, m_vsByteCode->GetBufferPointer(), m_vsByteCode->GetBufferSize(), m_inputLayout.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool ATexture::CreateSamplerState( )
{
	HRESULT hr = S_OK; 

	D3D11_SAMPLER_DESC samplerDesc;

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_device->CreateSamplerState(&samplerDesc, m_sampleState.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

ATexture::ATexture (D3DClass* d3d,
	const wchar_t* vertexShaderFilepath,
	const wchar_t* pixelShaderFilepath )
{

	m_device = d3d->GetDevice().Get();
	m_context = d3d->GetContext().Get();

	m_scale.x = 4.0f;
	m_scale.y = 4.0f;
	m_scale.z = 4.0f;
	 
	m_position = XMFLOAT3( 0.0f, 0.0f, 0.0f );

	CompileVertexShader( vertexShaderFilepath );
	CompilePixelShader( pixelShaderFilepath );

	CreateBuffer( );
	CreateInputLayout(  );
	CreateSamplerState(  );

	LoadTexture( );

}

ATexture::~ATexture()
{

}

void ATexture::Update(Mat4 world, Mat4 view, Mat4 proj)
{
	UpdateBuffer( world, view, proj);

}

void ATexture::Render(D3DClass* d3d) const
{ 
	UINT stride, offset;

	stride = sizeof(Vertex);
	offset = 0;

	d3d->TurnOffCulling();
	m_context->VSSetConstantBuffers(0, 1, m_matrixBuffer.GetAddressOf());
	m_context->PSSetShaderResources(0, 1, m_textureSrv.GetAddressOf());
	m_context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	m_context->IASetIndexBuffer( m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, offset );
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->IASetInputLayout(m_inputLayout.Get());
	m_context->VSSetShader(m_vertexShader.Get(), NULL, 0);
	m_context->PSSetShader(m_pixelShader.Get(), NULL, 0);
	m_context->PSSetSamplers(0, 1, m_sampleState.GetAddressOf());
	m_context->DrawIndexed(m_vertexCount, 0, 0);
	d3d->TurnOnCulling();

}

void ATexture::SetPosition(Vector3 position)
{
	m_position = position;
}

void ATexture::SetScale3D(float scaleX, float scaleY, float scaleZ)
{
	m_scale.x = scaleX;
	m_scale.y = scaleY;
	m_scale.z = scaleZ;
}

void ATexture::SetScale(float scale)
{
	m_scale.x = scale;
	m_scale.y = scale;
	m_scale.z = scale;
}

IDXGISurface* ATexture::GetDXGISurface()
{
	HRESULT hr = m_textureResource->QueryInterface(m_textSurface.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));

	return m_textSurface.Get();
}
