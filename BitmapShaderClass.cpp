#include "BitmapShaderClass.h"


BitmapShaderClass::BitmapShaderClass()
{
 

}

BitmapShaderClass::~BitmapShaderClass()
{
	Shutdown();
}

bool BitmapShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShaders(device, hwnd, (WCHAR*)L"../shaders/texture_vs.hlsl", (WCHAR*)L"../shaders/texture_ps.hlsl");
	assert(result);

	return true;

}

void BitmapShaderClass::Shutdown()
{
	ShutdownShaders();
}

bool BitmapShaderClass::Render(ID3D11DeviceContext* context, int indexcount, Mat4 world, Mat4 view, Mat4 projection, ID3D11ShaderResourceView* texture)
{
	bool result;

	result = SetShaderParameters(context, world, view, projection, texture);
	if (!result)
	{
		return false;
	}

	RenderShader(context, indexcount);

	return true;

}

bool BitmapShaderClass::InitializeShaders(ID3D11Device* device, HWND hwnd, WCHAR* vsPath, WCHAR* psPath)
{
	HRESULT result;
	ID3D10Blob* errMsg;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	D3D11_SAMPLER_DESC samplerDesc;


	errMsg = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//compile vertex shader
	result = D3DCompileFromFile(vsPath, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errMsg);
	if (FAILED(result))
	{
		if (errMsg)
		{
			OutputShaderErrorMsg(errMsg, hwnd, vsPath);
		}
		else
		{
			MessageBox(hwnd, vsPath, L"Missing vs shader file...", MB_OK);
		}
		return false;
	}

	//compile pixel shader
	result = D3DCompileFromFile(psPath, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errMsg);

	if (FAILED(result))
	{
		if (errMsg)
		{
			OutputShaderErrorMsg(errMsg, hwnd, psPath);
		}
		else
		{
			MessageBox(hwnd, psPath, L"Missing ps shader file...", MB_OK);
		}
		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, m_VertexShader.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, m_PixelShader.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	//fill layout
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

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), m_InputLayout.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//setup constant buffers
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, m_MatrixBuffer.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, m_SampleState.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	return true;

}

void BitmapShaderClass::ShutdownShaders()
{
	m_SampleState.Reset();

	m_MatrixBuffer.Reset();

	m_InputLayout.Reset();

	m_PixelShader.Reset();

	m_VertexShader.Reset();

}

void BitmapShaderClass::OutputShaderErrorMsg(ID3DBlob* errMsg, HWND hwnd, WCHAR* path)
{
	char* compileError;
	unsigned long long bufferSize, i;
	ofstream fout;

	compileError = (char*)errMsg->GetBufferPointer();
	bufferSize = errMsg->GetBufferSize();

	fout.open("ShaderError.txt");
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileError[i];
	}

	fout.close();

	errMsg->Release();
	errMsg = 0;

	MessageBox(hwnd, L"Error compiling shader, check ShaderError.txt", path, MB_OK);

}

bool BitmapShaderClass::SetShaderParameters(ID3D11DeviceContext* context, Mat4 world, Mat4 view, Mat4 projection, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	projection = XMMatrixTranspose(projection); 

	result = context->Map(m_MatrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = projection;

	context->Unmap(m_MatrixBuffer.Get(), 0);

	bufferNumber = 0;
	context->VSSetConstantBuffers(bufferNumber, 1, m_MatrixBuffer.GetAddressOf());

	context->PSSetShaderResources(0, 1, &texture);



	return true;


}

void BitmapShaderClass::RenderShader(ID3D11DeviceContext* context, int indexcount)
{
	context->IASetInputLayout(m_InputLayout.Get());
	context->VSSetShader(m_VertexShader.Get(), NULL, 0);
	context->PSSetShader(m_PixelShader.Get(), NULL, 0);

	context->PSSetSamplers(0, 1, m_SampleState.GetAddressOf());

	context->DrawIndexed(indexcount, 0, 0);

}
