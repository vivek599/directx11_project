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

void CameraClass::GetViewMatrix(Mat4& viewmatrix)
{
	viewmatrix = m_ViewMatrix;
}

void CameraClass::Fly(float deltaTime)
{
	static float pitch = 0.0f;
	static float yaw = 0.0f;

	pitch += float(m_rawMouse.GetDeltaY()) * MOUSESPEED * deltaTime;
	yaw += float(m_rawMouse.GetDeltaX()) * MOUSESPEED * deltaTime;

	pitch = pitch > 80.0f ? 80.0f : (pitch < -80.0f ? -80.0f : pitch);

	SetRotation(pitch, yaw, 0.f);

	float distance = MOVEMENTSPEED * deltaTime;

	Vector3 forward = m_lookAt - m_Position;
	forward.Normalize();

	m_lookAt = m_Position + forward;

	m_lookAt.x += sin(XMConvertToRadians(m_Yaw));
	m_lookAt.z += cos(XMConvertToRadians(m_Yaw));
	m_lookAt.y -= sin(XMConvertToRadians(m_Pitch));

	Vector3 right = m_up.Cross(forward);
	right.Normalize();

	if (BaseClass::m_moveCameraForward)
	{
		m_Position += distance * forward;
		m_lookAt += distance * forward;
	}

	if (BaseClass::m_moveCameraBackward)
	{
		m_Position -= distance * forward;
		m_lookAt -= distance * forward;
	}

	if (BaseClass::m_moveCameraLeft)
	{
		m_Position -= distance * right;
		m_lookAt -= distance * right;
	}

	if (BaseClass::m_moveCameraRight)
	{
		m_Position += distance * right;
		m_lookAt += distance * right;
	}

	if (BaseClass::m_moveCameraUp)
	{
		m_Position.y += distance;
		m_lookAt.y += distance;
	}

	if (BaseClass::m_moveCameraDown)
	{
		m_Position.y -= distance;
		m_lookAt.y -= distance;
	}
}
