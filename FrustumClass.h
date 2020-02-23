#pragma once

#include "commoninclude.h"





class FrustumClass
{
public:
	FrustumClass();
	~FrustumClass();

	void ConstructFrustum(float screenDepth, Mat4 projectionMatrix, Mat4 viewMatrix);

	bool CheckPoint(float x, float y, float z);
	bool CheckCube(float xCenter, float yCenter, float zCenter, float radius);
	bool CheckSphere(float xCenter, float yCenter, float zCenter, float radius);
	bool CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize);

private:
 		Vector4 m_planes[6];
};

