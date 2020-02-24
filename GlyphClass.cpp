#include "GlyphClass.h"



GlyphClass::GlyphClass()
{

	//m_GlyphWidth = 20;
	m_MaxCharInLine = 1024;
	m_NewlineCounter = 0;
	m_PosOffsetX = 0.f;
	m_PosOffsetY = 0.f;
	m_LineLocalWidth = 32;
	m_EnterKeyCounter = 0;

}

GlyphClass::~GlyphClass()
{
	Shutdown();
}

bool GlyphClass::Initialize(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight, WCHAR* path)
{
	bool result;
	m_GlyphWidthData.open("../Font/AgencyFBFont_64x64_width.txt");
	while (1)
	{
		if (m_GlyphWidthData.eof())
		{
			break;
		}

		int tmp = 0;
		m_GlyphWidthData >> tmp;
		if (tmp != 0)
		{
			m_GlyphWidth.push_back(tmp);
		}
	}
	m_GlyphWidthData.close();

	m_GlyphCharacter = 'A';
	m_GlyphWidth[char(0x0D)] = 0;//zero width enterkey char
	m_StringToDraw = "@xyz";

	m_fontTextureShader.reset( new FontShaderClass());
	assert(m_fontTextureShader);


	result = m_fontTextureShader->Initialize(device, hwnd);
	assert(result);


	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;

	result = InitializeBuffers(device);
	assert(result);


	result = LoadTexture(device, path);
	assert(result);


	return true;

}

void GlyphClass::Shutdown()
{
	ReleaseTexture();

	ShutdownBuffers();
}

bool GlyphClass::Render(ID3D11DeviceContext* context, int posX, int posY, Mat4 world, Mat4 view, Mat4 ortho)
{
	bool result;

	//Mat4 finalWorld = world * XMMatrixScaling(0.5f, 0.5f, 0.5f);

	result = UpdateBuffers(context, posX, posY);
	if (!result)
	{
		return false;
	}

	RenderBuffers(context);

	result = m_fontTextureShader->Render(context, GetIndexCount(), world, view, ortho, GetTexture().Get());
	if (!result)
	{
		return false;
	}

	return true;
}

int GlyphClass::GetIndexCount()
{
	return m_IndexCount;
}

ComPtr<ID3D11ShaderResourceView> GlyphClass::GetTexture()
{
	return m_fontTexture->GetTexture();

}

bool GlyphClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	m_VertexCount = 6;

	m_IndexCount = m_VertexCount * m_MaxCharInLine;

	vertices = new VertexType[m_VertexCount * m_MaxCharInLine];
	if (!vertices)
	{
		return false;
	}

	indices = new unsigned long[m_IndexCount];
	if (!indices)
	{
		return false;
	}

	memset(vertices, 0, sizeof(VertexType) * m_VertexCount * m_MaxCharInLine);

	for (i = 0; i < m_IndexCount; i++)
	{
		indices[i] = i;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount * m_MaxCharInLine;
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

void GlyphClass::ShutdownBuffers()
{
	m_IndexBuffer.Reset();

	m_VertexBuffer.Reset();
}

bool GlyphClass::UpdateBuffers(ID3D11DeviceContext* context, int posX, int posY)
{
	m_StringToDraw = userInputString != "" ? userInputString : "Enter a string: ..... ";
	int stringLength = m_StringToDraw.length();
			
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* vertexPtr;
	HRESULT hr;

	vertices = new VertexType[m_VertexCount * stringLength];
	if (!vertices)
	{
		return false;
	}
	
	// Initialize vertex array to zeros at first.
	memset(vertices, 0, sizeof(VertexType) * m_VertexCount * stringLength);


	m_PosOffsetX = 0.f;
	m_PosOffsetY = 0.f;
	m_NewlineCounter = 0;
	m_EnterKeyCounter = 0;
	int itrOffset = 0;

	for ( int i = 0; i < stringLength; i++ )
	{
		int glyphBlockWidth = 64;

		int cx = int ( m_StringToDraw[i] ) % 16;
		int cy = int ( m_StringToDraw[i] ) / 16;
		bool bNewLine = /*m_StringToDraw[i] == char(0x0D) || */m_StringToDraw[i] == char(0x0A);
		if ( bNewLine || ( (i >= ( m_LineLocalWidth * (m_NewlineCounter + 1) ) + itrOffset ) && (userInputString != "") )  )
		{
			m_PosOffsetX = 0.f;
			if (bNewLine)
			{
				m_EnterKeyCounter++;
				itrOffset = i % m_LineLocalWidth + 1;
			}
			else
			{
				m_NewlineCounter++;
			}

			m_PosOffsetY = (glyphBlockWidth + 4.f) * (m_NewlineCounter + m_EnterKeyCounter);
			continue;
		}

		float tex_left = (float)cx * (1.f / 16.f);
		float tex_top = (float)cy * (1.f / 16.f);
		float tex_right = tex_left + (1.f / 16.f) * ((float)m_GlyphWidth[m_StringToDraw[i]] / (float)glyphBlockWidth);
		float tex_bottom = tex_top + (1.f / 16.f);


		float left = (float)((float)(m_ScreenWidth / 2.f) * -1) + m_PosOffsetX + (float)posX;
		float right = left + (float)m_GlyphWidth[m_StringToDraw[i]];
		float top = (float)(m_ScreenHeight / 2.f) - (m_PosOffsetY + (float)posY);
		float bottom = top - (float)glyphBlockWidth;

		//triangle 1 - clockwise
		vertices[0 + m_VertexCount * i].position = Vector3(left, top, 0.f);
		vertices[0 + m_VertexCount * i].texture = Vector2(tex_left, tex_top);

		vertices[1 + m_VertexCount * i].position = Vector3(right, bottom, 0.f);
		vertices[1 + m_VertexCount * i].texture = Vector2(tex_right, tex_bottom);

		vertices[2 + m_VertexCount * i].position = Vector3(left, bottom, 0.f);
		vertices[2 + m_VertexCount * i].texture = Vector2(tex_left, tex_bottom);

		//triangle + i 2
		vertices[3 + m_VertexCount * i].position = Vector3(left, top, 0.f);
		vertices[3 + m_VertexCount * i].texture = Vector2(tex_left, tex_top);

		vertices[4 + m_VertexCount * i].position = Vector3(right, top, 0.f);
		vertices[4 + m_VertexCount * i].texture = Vector2(tex_right, tex_top);

		vertices[5 + m_VertexCount * i].position = Vector3(right, bottom, 0.f);
		vertices[5 + m_VertexCount * i].texture = Vector2(tex_right, tex_bottom);

		m_PosOffsetX += m_GlyphWidth[m_StringToDraw[i]];
	
	}


	hr = context->Map(m_VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}

	vertexPtr = (VertexType*)mappedResource.pData;
	memset(vertexPtr, 0, sizeof(VertexType) * m_VertexCount * m_MaxCharInLine);
	int bufferSize = sizeof(VertexType) * m_VertexCount * ( stringLength <= m_MaxCharInLine ? stringLength : m_MaxCharInLine );
	memcpy(vertexPtr, (void*)vertices, bufferSize);

	context->Unmap(m_VertexBuffer.Get(), 0);

	delete[] vertices;
	vertices = 0;

	return true;

}

void GlyphClass::RenderBuffers(ID3D11DeviceContext* context)
{

	unsigned int stride, offset;

	stride = sizeof(VertexType);
	offset = 0;

	context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

bool GlyphClass::LoadTexture(ID3D11Device* device, WCHAR* path)
{

	bool result;

	m_fontTexture.reset(new TextureClass());
	if (!m_fontTexture)
	{
		return false;
	}

	result = m_fontTexture->Initialize(device, path);
	if (!result)
	{
		return false;
	}

	return true;

}

void GlyphClass::ReleaseTexture()
{

}
