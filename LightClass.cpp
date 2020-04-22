#include "LightClass.h"



LightClass::LightClass()
{
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

Vector3 LightClass::GetPosition()
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
	// Setup field of view and screen aspect for a square light source.
	float fieldOfView = XM_PI / 8.0f;
	float screenAspect = 1.0f;

	// Create the projection matrix for the light.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

}

void LightClass::GenerateOrthoMatrix(float screenNear, float screenDepth)
{
	// Create the projection matrix for the light.
	m_orthoMatrix = XMMatrixOrthographicLH( 100.0f, 100.0f, 0.1f, 300.f);
}

Mat4 LightClass::GetViewMatrix()
{
	return m_viewMatrix;
}

Mat4 LightClass::GetProjectionMatrix()
{
	return m_projectionMatrix;
}

Mat4 LightClass::GetOrthoMatrix()
{
	return m_orthoMatrix;
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
