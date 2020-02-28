#pragma once

#include "commoninclude.h"
#include "BaseClass.h"

#include "BoundingBoxShader.h"
 

class BoundingBoxClass : BaseClass
{

private:
	struct BBoxVertex
	{
		XMFLOAT3 position;
	};

public:
	BoundingBoxClass(ID3D11Device* device, void* buffer, int size);
	~BoundingBoxClass();

	void Shutdown();
	bool Render(ID3D11DeviceContext* context, Mat4 world, Mat4 view, Mat4 proj );

private:

	bool Initialize(ID3D11Device* device, void* buffer, int size);

	bool InitializeBuffers(ID3D11Device* device, void* buffer, int size );
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* context);


	ComPtr<ID3D11Buffer> m_BBoxVertexBuffer;
	ComPtr<ID3D11Buffer> m_BBoxIndexBuffer;
	int m_IndexCount;
	vector <BBoxVertex> bbox_vertices;
	vector <UINT> indices;

	unique_ptr<BoundingBoxShader> m_BBoxShader;

};


