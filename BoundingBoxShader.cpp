#include "BoundingBoxShader.h"



BoundingBoxShader::BoundingBoxShader()
{
  
 

}

BoundingBoxShader::~BoundingBoxShader()
{
	Shutdown();
}

bool BoundingBoxShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd );
	assert(result);

	return true;
}

void BoundingBoxShader::Shutdown()
{
	ShutdownShader();

}

bool BoundingBoxShader::Render(ID3D11DeviceContext* context, int indexCount, Mat4 world, Mat4 view, Mat4 proj )
{
	bool result;

	result = SetShaderParameter(context, indexCount, world, view, proj );
	assert(result);

	RenderShader(context, indexCount);

	return true;
}

bool BoundingBoxShader::InitializeShader(ID3D11Device* device, HWND hwnd )
{
	HRESULT hr;
	ComPtr<ID3D10Blob> errMsg;
	ComPtr<ID3D10Blob> vertexShaderBuffer;
	ComPtr<ID3D10Blob> pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	UINT numElement;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;

	m_bbox_vs =
		"cbuffer MatrixBuffer											 \
		{																 \
			matrix world;													 \
			matrix view;													 \
			matrix Projection;													 \
		};																 \
		struct VertexInputType											 \
		{																 \
			float4 position : POSITION;									 \
		};																 \
																		 \
		struct PixelInputType											 \
		{																 \
			float4 position : SV_POSITION;								 \
		};																 \
		PixelInputType main(VertexInputType input)						 \
		{																 \
			PixelInputType output;										 \
			input.position.w = 1.0f;									 \
			output.position = mul(input.position, world);					 \
			output.position = mul(output.position, view);				 \
			output.position = mul(output.position, Projection);			 \
			return output;												 \
		}";

	m_bbox_ps =
		"													\
		struct PixelInputType								\
		{													\
			float4 position : SV_POSITION; 					\
		};													\
															\
		float4 main(PixelInputType input) : SV_TARGET		\
		{													\
			return float4(1.0f, 1.0f, 1.0f,1.0f );			\
		}";

	hr = D3DCompile(m_bbox_vs.c_str(), m_bbox_vs.size(), NULL, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, vertexShaderBuffer.ReleaseAndGetAddressOf(), errMsg.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		if (errMsg)
		{
			OutputShaderErrorMsg(errMsg.Get(), hwnd, L"" );
		}
		else
		{
			MessageBoxA(hwnd, "bbox", "missing vertex shader file...", MB_OK);
		}

		return false;
	}

	hr = D3DCompile(m_bbox_ps.c_str(),m_bbox_ps.size(),NULL, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, pixelShaderBuffer.ReleaseAndGetAddressOf(), errMsg.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		if (errMsg)
		{
			OutputShaderErrorMsg(errMsg.Get(), hwnd, L"" );
		}
		else
		{
			MessageBoxA(hwnd, "BBOX", "missing pixel shader file...", MB_OK);
		}

		return false;
	}

	hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, m_vertexShader.GetAddressOf());
	assert(SUCCEEDED(hr));

	hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, m_pixelShader.GetAddressOf());
	assert(SUCCEEDED(hr));

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	numElement = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	hr = device->CreateInputLayout(polygonLayout, numElement, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), m_inputLayout.GetAddressOf());
	assert(SUCCEEDED(hr));

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&matrixBufferDesc, NULL, m_matrixBuffer.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));

	return true;

}

void BoundingBoxShader::ShutdownShader()
{
 

}

void BoundingBoxShader::OutputShaderErrorMsg(ID3D10Blob* errMsg, HWND hwnd, const WCHAR* shaderFileName)
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

bool BoundingBoxShader::SetShaderParameter(ID3D11DeviceContext* context, int indexCount, Mat4 world, Mat4 view, Mat4 proj )
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
	assert(SUCCEEDED(hr));

	mdataPtr = (MatrixBufferType*)mappedResource.pData;
	mdataPtr->world = world;
	mdataPtr->view = view;
	mdataPtr->Projection = proj;

	context->Unmap(m_matrixBuffer.Get(), 0);

	bufferNumber = 0;
	context->VSSetConstantBuffers(bufferNumber, 1, m_matrixBuffer.GetAddressOf());

	return true;
}

void BoundingBoxShader::RenderShader(ID3D11DeviceContext* context, int indexCount)
{
	context->IASetInputLayout(m_inputLayout.Get());
	context->VSSetShader(m_vertexShader.Get(), NULL, 0);
	context->PSSetShader(m_pixelShader.Get(), NULL, 0);
	context->DrawIndexed(indexCount, 0, 0);
}









