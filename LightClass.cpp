#include "LightClass.h"



LightClass::LightClass()
{
}

LightClass::LightClass(const LightClass& obj)
{
	UNREFERENCED_PARAMETER(obj);

}

LightClass::~LightClass()
{
}

void LightClass::SetSpecularColor(float r, float g, float b, float a)
{
	m_specularColor = Vector4(r, g, b, a);
}

void LightClass::SetSpecularPower(float power)
{
	m_specularPower = power;
}

void LightClass::SetAmbientColor(float r, float g, float b, float a)
{
	m_ambientColor = Vector4(r, g, b, a);
}

void LightClass::SetDiffuseColor(float r, float g, float b, float a)
{
	m_diffuseColor = Vector4(r, g, b, a);
}

void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = Vector3(x, y, z);
}

void LightClass::SetPosition(float x, float y, float z)
{
	m_position = Vector3(x, y, z);
}

DirectX::SimpleMath::Vector3 LightClass::GetPosition()
{
	return m_position;
}

void LightClass::SetLookAt(float x, float y, float z)
{
	m_lookAt = Vector3(x, y, z);
}

void LightClass::GenerateViewMatrix()
{
	Vector3 up;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Create the view matrix from the three vectors.
	m_viewMatrix = XMMatrixLookAtLH( m_position, m_lookAt, up);
}

void LightClass::GenerateProjectionMatrix(float screenNear, float screenDepth)
{
	float fieldOfView, screenAspect;


	// Setup field of view and screen aspect for a square light source.
	fieldOfView = (float)XM_PI / 2.0f;
	screenAspect = 1.0f;

	// Create the projection matrix for the light.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

}

void LightClass::GetViewMatrix(Mat4& viewMatrix )
{
	viewMatrix = m_viewMatrix;
}

void LightClass::GetProjectionMatrix(Mat4& projMatrix)
{
	projMatrix = m_projectionMatrix;
}

Vector4 LightClass::GetAmbientColor()
{
	return m_ambientColor;
}

Vector4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}

Vector3 LightClass::GetDirection()
{
	return m_direction;
}

Vector4 LightClass::GetSpecularColor()
{
	return m_specularColor;
}

float LightClass::GetSpecularPower()
{
	return m_specularPower;
}
