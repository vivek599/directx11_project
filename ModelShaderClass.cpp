#include "ModelShaderClass.h"



ModelShaderClass::ModelShaderClass()
{
  
 

}

ModelShaderClass::ModelShaderClass(const ModelShaderClass& obj)
{
	UNREFERENCED_PARAMETER(obj);

}

ModelShaderClass::~ModelShaderClass()
{
	Shutdown();
}

bool ModelShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, (WCHAR*)L"../shaders/vmodel_light_shadows.hlsl", (WCHAR*)L"../shaders/pmodel_light_shadows.hlsl");
	assert(result);


	return true;
}

void ModelShaderClass::Shutdown()
{
	ShutdownShader();

}

bool ModelShaderClass::Render(ID3D11DeviceContext* context, int indexCount, Mat4 world, Mat4 view, Mat4 proj, LightClass* light, Mat4 lightViewMatrix,
	Mat4 lightProjectionMatrix, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView* depthMapTexture, Vector3 cameraPosition)
{
	bool result;

	result = SetShaderParameter(context, indexCount, world, view, proj, light, lightViewMatrix, lightProjectionMatrix, 
		textureArray, depthMapTexture, cameraPosition );
	if (!result)
	{
		return false;
	}

	RenderShader(context, indexCount);

	return true;
}

bool ModelShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{
	HRESULT hr;
	ID3D10Blob* errMsg;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	UINT numElement;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc, lightBufferDesc2; 
	D3D11_BUFFER_DESC cameraBufferDesc;

	errMsg = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	hr = D3DCompileFromFile(vsFileName, NULL, NULL, "ShadowVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errMsg);
	if (FAILED(hr))
	{
		if (errMsg)
		{
			OutputShaderErrorMsg(errMsg, hwnd, vsFileName);
		}
		else
		{
			MessageBox(hwnd, vsFileName, L"missing vertex shader file...", MB_OK);
		}

		return false;
	}

	hr = D3DCompileFromFile(psFileName, NULL, NULL, "ShadowPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errMsg);
	if (FAILED(hr))
	{
		if (errMsg)
		{
			OutputShaderErrorMsg(errMsg, hwnd, psFileName);
		}
		else
		{
			MessageBox(hwnd, psFileName, L"missing pixel shader file...", MB_OK);
		}

		return false;
	}

	hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, m_vertexShader.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, m_pixelShader.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	numElement = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	hr = device->CreateInputLayout(polygonLayout, numElement, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), m_inputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// Create a clamp texture sampler state description.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	// Create the texture sampler state.
	hr = device->CreateSamplerState(&samplerDesc, m_sampleStateClamp.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&matrixBufferDesc, NULL, m_matrixBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}
	// Setup the description of the camera dynamic constant buffer that is in the vertex shader.
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&cameraBufferDesc, NULL, m_cameraBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&lightBufferDesc, NULL, m_lightBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the vertex shader.
	lightBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc2.ByteWidth = sizeof(LightBufferType2);
	lightBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc2.MiscFlags = 0;
	lightBufferDesc2.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&lightBufferDesc2, NULL, m_lightBuffer2.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}
	 

	return true;

}

void ModelShaderClass::ShutdownShader()
{
	m_lightBuffer.Reset();

	m_lightBuffer2.Reset();

	m_cameraBuffer.Reset();

	m_matrixBuffer.Reset();

	m_sampleStateClamp.Reset();

	m_samplerState.Reset();

	m_inputLayout.Reset();

	m_pixelShader.Reset();

	m_vertexShader.Reset();

}

void ModelShaderClass::OutputShaderErrorMsg(ID3D10Blob* errMsg, HWND hwnd, WCHAR* shaderFileName)
{
	char* compileErr;
	unsigned long long bufferSize, i;
	ofstream fout;

	compileErr = (char*)errMsg->GetBufferPointer();
	bufferSize = errMsg->GetBufferSize();

	fout.open("light_shader_error.txt");

	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErr[i];
	}

	fout.close();

	errMsg->Release();
	errMsg = 0;

	MessageBox(hwnd, L"Error compiling shader... Check light_shader_error.txt for message.", shaderFileName, MB_OK);

}

bool ModelShaderClass::SetShaderParameter(ID3D11DeviceContext* context, int indexCount, Mat4 world, Mat4 view, Mat4 proj, LightClass* light, Mat4 lightViewMatrix, Mat4 lightProjectionMatrix,
	ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView* depthMapTexture, Vector3 cameraPosition)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	UINT bufferNumber;
	MatrixBufferType* mdataPtr;
	LightBufferType* ldataPtr;
	LightBufferType2* l2dataPtr; 
	CameraBufferType* cdataPtr;

	// Transpose the matrices to prepare them for the shader.
	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	proj = XMMatrixTranspose(proj);

	hr = context->Map(m_matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}

	mdataPtr = (MatrixBufferType*)mappedResource.pData;
	mdataPtr->world = world;
	mdataPtr->view = view;
	mdataPtr->proj = proj;
	mdataPtr->lightView = lightViewMatrix;
	mdataPtr->lightProjection = lightProjectionMatrix;

	context->Unmap(m_matrixBuffer.Get(), 0);

	bufferNumber = 0;
	context->VSSetConstantBuffers(bufferNumber, 1, m_matrixBuffer.GetAddressOf());

	// Lock the second light constant buffer so it can be written to.
	hr = context->Map(m_lightBuffer2.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	l2dataPtr = (LightBufferType2*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	l2dataPtr->lightPosition = light->GetPosition();
	l2dataPtr->padding = 0.0f;

	// Unlock the constant buffer.
	context->Unmap(m_lightBuffer2.Get(), 0);

	// Set the position of the light constant buffer in the vertex shader.
	bufferNumber = 1;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	context->VSSetConstantBuffers(bufferNumber, 1, m_lightBuffer2.GetAddressOf());

	// Lock the camera constant buffer so it can be written to.
	hr = context->Map(m_cameraBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	cdataPtr = (CameraBufferType*)mappedResource.pData;

	// Copy the camera position into the constant buffer.
	cdataPtr->cameraPosition = cameraPosition;
	cdataPtr->padding = 0.0f;

	// Unlock the camera constant buffer.
	context->Unmap(m_cameraBuffer.Get(), 0);

	// Set the position of the camera constant buffer in the vertex shader.
	bufferNumber = 2;

	// Now set the camera constant buffer in the vertex shader with the updated values.
	context->VSSetConstantBuffers(bufferNumber, 1, m_cameraBuffer.GetAddressOf());

	 
	context->PSSetShaderResources(0, 1, &depthMapTexture);
	context->PSSetShaderResources(1, 2, textureArray);

	hr = context->Map(m_lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}

	ldataPtr = (LightBufferType*)mappedResource.pData;
	ldataPtr->ambientColor = light->GetAmbientColor();
	ldataPtr->diffuseColor = light->GetDiffuseColor();
	ldataPtr->lightDirection = light->GetDirection();
	ldataPtr->specularColor = light->GetSpecularColor();
	ldataPtr->specularPower = light->GetSpecularPower();

	context->Unmap(m_lightBuffer.Get(), 0);
	bufferNumber = 0;

	context->PSSetConstantBuffers(bufferNumber, 1, m_lightBuffer.GetAddressOf());



	return true;
}

void ModelShaderClass::RenderShader(ID3D11DeviceContext* context, int indexCount)
{
	context->IASetInputLayout(m_inputLayout.Get());
	context->VSSetShader(m_vertexShader.Get(), NULL, 0);
	context->PSSetShader(m_pixelShader.Get(), NULL, 0);
	context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
	context->PSSetSamplers(1, 1, m_sampleStateClamp.GetAddressOf());

	context->DrawIndexed(indexCount, 0, 0);
}









