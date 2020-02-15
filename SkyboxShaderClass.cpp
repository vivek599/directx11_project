#include "SkyboxShaderClass.h"



SkyboxShaderClass::SkyboxShaderClass()
{
 
}


SkyboxShaderClass::~SkyboxShaderClass()
{
	Shutdown();
}

bool SkyboxShaderClass::Initialize(ID3D11Device* device)
{
	bool result;

	result = InitializeShader(device, GetActiveWindow(), (WCHAR*)L"../shaders/skybox_vs.hlsl", (WCHAR*)L"../shaders/skybox_ps.hlsl");
	assert(result);


	return true;
}

void SkyboxShaderClass::Shutdown()
{
	ShutdownShader();
}

bool SkyboxShaderClass::Render(ID3D11DeviceContext* context, int indexCount, Mat4 worldMatrix, Mat4 viewMatrix, Mat4 projMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;

	result = SetShaderParameter(context, worldMatrix, viewMatrix, projMatrix, texture );
	if (!result)
	{
		return false;
	}

	RenderShader(context, indexCount);

	return true;
}

bool SkyboxShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{
	HRESULT hr;
	ID3D10Blob* errMsg;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	UINT numElement;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;

	errMsg = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	hr = D3DCompileFromFile(vsFileName, NULL, NULL, "main", "vs_5_0",
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

	hr = D3DCompileFromFile(psFileName, NULL, NULL, "main", "ps_5_0",
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

	/*polygonLayout[3].SemanticName = "NORMAL";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;*/

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


	return true;
}

void SkyboxShaderClass::ShutdownShader()
{
	m_matrixBuffer.Reset();
	m_samplerState.Reset();

	m_inputLayout.Reset();

	m_pixelShader.Reset();

	m_vertexShader.Reset();

}

void SkyboxShaderClass::OutputShaderErrorMsg(ID3D10Blob* errMsg, HWND hwnd, WCHAR* shaderFileName)
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

bool SkyboxShaderClass::SetShaderParameter(ID3D11DeviceContext* context, Mat4 world, Mat4 view, Mat4 proj, ID3D11ShaderResourceView* texture)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	UINT bufferNumber;
	MatrixBufferType* mdataPtr;

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

	context->Unmap(m_matrixBuffer.Get(), 0);

	bufferNumber = 0;
	context->VSSetConstantBuffers(bufferNumber, 1, m_matrixBuffer.GetAddressOf());
	context->PSSetShaderResources(0, 1, &texture);

	return true;
}

void SkyboxShaderClass::RenderShader(ID3D11DeviceContext* context, int indexCount)
{
	context->IASetInputLayout(m_inputLayout.Get());
	context->VSSetShader(m_vertexShader.Get(), NULL, 0);
	context->PSSetShader(m_pixelShader.Get(), NULL, 0);
	context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

	context->DrawIndexed(indexCount, 0, 0);
}
