#include "FrustumClass.h"



FrustumClass::FrustumClass()
{

}

FrustumClass::~FrustumClass()
{

}

void FrustumClass::ConstructFrustum(float screenDepth, Mat4 projectionMatrix, Mat4 viewMatrix)
{
	float zMinimum, r;
	Mat4 vpmatrix;

	// Calculate the minimum Z distance in the frustum.
	zMinimum = -projectionMatrix._43 / projectionMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);
	projectionMatrix._33 = r;
	projectionMatrix._43 = -r * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	vpmatrix = viewMatrix * projectionMatrix;

	// Calculate near plane of frustum.
	m_planes[0].x = vpmatrix._14 + vpmatrix._13;
	m_planes[0].y = vpmatrix._24 + vpmatrix._23;
	m_planes[0].z = vpmatrix._34 + vpmatrix._33;
	m_planes[0].w = vpmatrix._44 + vpmatrix._43;
	m_planes[0] = XMPlaneNormalize(m_planes[0]);

	// Calculate far plane of frustum.
	m_planes[1].x = vpmatrix._14 - vpmatrix._13;
	m_planes[1].y = vpmatrix._24 - vpmatrix._23;
	m_planes[1].z = vpmatrix._34 - vpmatrix._33;
	m_planes[1].w = vpmatrix._44 - vpmatrix._43;
	m_planes[1] = XMPlaneNormalize(m_planes[1]);

	// Calculate left plane of frustum.
	m_planes[2].x = vpmatrix._14 + vpmatrix._11;
	m_planes[2].y = vpmatrix._24 + vpmatrix._21;
	m_planes[2].z = vpmatrix._34 + vpmatrix._31;
	m_planes[2].w = vpmatrix._44 + vpmatrix._41;
	m_planes[2] = XMPlaneNormalize(m_planes[2]);

	// Calculate right plane of frustum.
	m_planes[3].x = vpmatrix._14 - vpmatrix._11;
	m_planes[3].y = vpmatrix._24 - vpmatrix._21;
	m_planes[3].z = vpmatrix._34 - vpmatrix._31;
	m_planes[3].w = vpmatrix._44 - vpmatrix._41;
	m_planes[3] = XMPlaneNormalize(m_planes[3]);

	// Calculate top plane of frustum.
	m_planes[4].x = vpmatrix._14 - vpmatrix._12;
	m_planes[4].y = vpmatrix._24 - vpmatrix._22;
	m_planes[4].z = vpmatrix._34 - vpmatrix._32;
	m_planes[4].w = vpmatrix._44 - vpmatrix._42;
	m_planes[4] = XMPlaneNormalize(m_planes[4]);

	// Calculate bottom plane of frustum.
	m_planes[5].x = vpmatrix._14 + vpmatrix._12;
	m_planes[5].y = vpmatrix._24 + vpmatrix._22;
	m_planes[5].z = vpmatrix._34 + vpmatrix._32;
	m_planes[5].w = vpmatrix._44 + vpmatrix._42;
	m_planes[5] = XMPlaneNormalize(m_planes[5]);


}

bool FrustumClass::CheckPoint(float x, float y, float z)
{
	int i;

	// Check if the point is inside all six planes of the view frustum.
	for (i = 0; i < 6; i++)
	{
		if ( XMPlaneDotCoord(m_planes[i], Vector3(x,y,z)).m128_f32[0] < 0.0f ) // D3DXPlaneDotCoord(&m_planes[i], &Vector3(x, y, z)) < 0.f
		{
			return false;
		}
	}

	return true;
}

bool FrustumClass::CheckBox(Vector3 vmin, Vector3 vmax)
{
	int i;

	// Check if any one point of the box is in the view frustum.
	for (i = 0; i < 6; i++)
	{
		if (XMPlaneDotCoord(m_planes[i], Vector3( vmin.x, vmin.y, vmin.z)).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3( vmax.x, vmin.y, vmin.z)).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3( vmin.x, vmax.y, vmin.z)).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3( vmax.x, vmax.y, vmin.z)).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3( vmin.x, vmin.y, vmax.z)).m128_f32[0] >= 0.0f)
		{
			continue; 
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3( vmax.x, vmin.y, vmax.z)).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3( vmin.x, vmax.y, vmax.z)).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3( vmax.x, vmax.y, vmax.z)).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}

bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;


	// Check if any one point of the cube is in the view frustum.
	for (i = 0; i < 6; i++)
	{
		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter - radius), (yCenter - radius), (zCenter - radius))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter + radius), (yCenter - radius), (zCenter - radius))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter - radius), (yCenter + radius), (zCenter - radius))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter + radius), (yCenter + radius), (zCenter - radius))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter - radius), (yCenter - radius), (zCenter + radius))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter + radius), (yCenter - radius), (zCenter + radius))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter - radius), (yCenter + radius), (zCenter + radius))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter + radius), (yCenter + radius), (zCenter + radius))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}

bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;


	// Check if the radius of the sphere is inside the view frustum.
	for (i = 0; i < 6; i++)
	{
		if (XMPlaneDotCoord(m_planes[i], Vector3(xCenter, yCenter, zCenter)).m128_f32[0] < -radius)
		{
			return false;
		}
	}

	return true;
}

bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	int i;


	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for (i = 0; i < 6; i++)
	{
		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter - xSize), (yCenter - ySize), (zCenter - zSize))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter + xSize), (yCenter - ySize), (zCenter - zSize))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter - xSize), (yCenter + ySize), (zCenter - zSize))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter - xSize), (yCenter - ySize), (zCenter + zSize))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter + xSize), (yCenter + ySize), (zCenter - zSize))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter + xSize), (yCenter - ySize), (zCenter + zSize))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter - xSize), (yCenter + ySize), (zCenter + zSize))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(m_planes[i], Vector3((xCenter + xSize), (yCenter + ySize), (zCenter + zSize))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}










