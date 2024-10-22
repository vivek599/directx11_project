#include "cameraclass.h"


CameraClass::CameraClass()
{
	m_Pitch = 0.f;
	m_Yaw = 0.f;
	m_Roll = 0.f;

	m_lookAt = Vector3(0.f, 0.f, 1.f);
	m_up = Vector3(0.f, 1.f, 0.f);

}

CameraClass::~CameraClass()
{

}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_Pitch = x;
	m_Yaw	= y;
	m_Roll	= z;
}

Vector3 CameraClass::GetPosition()
{
	return m_Position;
}

Vector3 CameraClass::GetRotation()
{
	return Vector3(m_Pitch, m_Yaw, m_Roll);
}

void CameraClass::Update(float deltaTime)
{
	Fly(deltaTime);

}

void CameraClass::Render(float deltaTime)
{
	m_ViewMatrix = XMMatrixLookAtLH(m_Position, m_lookAt, m_up);
}

Mat4 CameraClass::GetViewMatrix()
{
	return m_ViewMatrix;
}

void CameraClass::Fly(float deltaTime)
{
	static float pitch = 0.0f;
	static float yaw = 0.0f;

	pitch += float(m_rawMouse.GetDeltaY()) * MOUSESPEED * deltaTime;
	yaw += float(m_rawMouse.GetDeltaX()) * MOUSESPEED * deltaTime;

	pitch = pitch > 80.0f ? 80.0f : (pitch < -80.0f ? -80.0f : pitch);
	
	SetRotation(pitch, yaw, 0.f);

	if (BaseClass::KeyDown('T'))
	{
		Vector3 pyr = PitchYawRollFromVector(Vector3( 0.0f, 0.0f, 1.0f));

		SetRotation(pitch = pyr.x, yaw = pyr.y, pyr.z );
	}

	float distance = MOVEMENTSPEED * deltaTime;

	Vector3 forward = m_lookAt - m_Position;
	forward.Normalize();

	m_lookAt = m_Position + forward;

	m_lookAt.x += sin(XMConvertToRadians(m_Yaw));
	m_lookAt.z += cos(XMConvertToRadians(m_Yaw));
	m_lookAt.y -= sin(XMConvertToRadians(m_Pitch));

	Vector3 right = m_up.Cross(forward);
	right.Normalize();

	if (BaseClass::KeyDown('W'))
	{
		m_Position += distance * forward;
		m_lookAt += distance * forward;
	}

	if (BaseClass::KeyDown('S'))
	{
		m_Position -= distance * forward;
		m_lookAt -= distance * forward;
	}

	if (BaseClass::KeyDown('A'))
	{
		m_Position -= distance * right;
		m_lookAt -= distance * right;
	}

	if (BaseClass::KeyDown('D'))
	{
		m_Position += distance * right;
		m_lookAt += distance * right;
	}

	if (BaseClass::KeyDown('Q'))
	{
		m_Position.y += distance;
		m_lookAt.y += distance;
	}

	if (BaseClass::KeyDown('E'))
	{
		m_Position.y -= distance;
		m_lookAt.y -= distance;
	}


}
