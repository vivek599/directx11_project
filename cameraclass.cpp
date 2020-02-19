#include "cameraclass.h"


CameraClass::CameraClass()
{
	m_PositionX = 0.f;
	m_PositionY = 0.f;
	m_PositionZ = 0.f;

	m_Pitch = 0.f;
	m_Yaw = 0.f;
	m_Roll = 0.f;

	m_forwardSpeed = 0.f;
	m_upwardSpeed = 0.f;
	m_LeftSpeed = 0.f;
	m_rightSpeed = 0.f;
	m_downwardSpeed = 0.f;
	m_backwardSpeed = 0.f;

	m_lookAt = Vector3(0.f, 0.f, 1.f);
	m_up = Vector3(0.f, 1.f, 0.f);

}

CameraClass::CameraClass(const CameraClass& obj)
{
	UNREFERENCED_PARAMETER(obj);
}

CameraClass::~CameraClass()
{

}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_PositionX = x;
	m_PositionY = y;
	m_PositionZ = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_Pitch = x;
	m_Yaw	= y;
	m_Roll	= z;
}

Vector3 CameraClass::GetPosition()
{
	return Vector3(m_PositionX, m_PositionY, m_PositionZ );
}

Vector3 CameraClass::GetRotation()
{
	return Vector3(m_Pitch, m_Yaw, m_Roll);
}

void CameraClass::Update(float deltaTime)
{
	POINT p = {0};
	GetCursorPos(&p);

	static POINT oldMouse = p;

	static float pitch = 0.0f;
	static float yaw = 0.0f;

	pitch += (p.y - oldMouse.y) * 0.2f;
	yaw += (p.x - oldMouse.x) * 0.2f;
	SetRotation(pitch, yaw, 0.f);

	oldMouse = p;
 






	MoveCameraForward(BaseClass::m_moveCameraForward, deltaTime);
	MoveCameraBackward(BaseClass::m_moveCameraBackward, deltaTime);
	MoveCameraLeft(BaseClass::m_moveCameraLeft, deltaTime);
	MoveCameraRight(BaseClass::m_moveCameraRight, deltaTime);
	MoveCameraUpward(BaseClass::m_moveCameraUp, deltaTime);
	MoveCameraDownward(BaseClass::m_moveCameraDown, deltaTime);
}

void CameraClass::MoveCameraForward(bool keydown, float deltaTime)
{
	if (keydown)
	{
		m_forwardSpeed += deltaTime * MOVEMENTSPEED;

		if (m_forwardSpeed > (deltaTime * MOVEMENTSPEED * 2))
		{
			m_forwardSpeed = deltaTime * MOVEMENTSPEED * 2;
		}
	}
	else
	{
		m_forwardSpeed -= deltaTime * MOVEMENTSPEED;

		if (m_forwardSpeed < 0.0f)
		{
			m_forwardSpeed = 0.0f;
		}
	}

	Vector3 dir = m_ViewMatrix.Backward();
	dir.Normalize();

	// Update the position.
	m_PositionX += m_forwardSpeed * dir.x;
	m_PositionY += m_forwardSpeed * dir.y * -1.0f;
	m_PositionZ += m_forwardSpeed * dir.z;
	//m_lookAt += m_forwardSpeed * m_ViewMatrix.Forward() * -1.0f;
}

void CameraClass::MoveCameraBackward(bool keydown, float deltaTime)
{
	// Update the backward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		m_backwardSpeed += deltaTime * MOVEMENTSPEED;

		if (m_backwardSpeed > (deltaTime * MOVEMENTSPEED * 2))
		{
			m_backwardSpeed = deltaTime * MOVEMENTSPEED * 2;
		}
	}
	else
	{
		m_backwardSpeed -= deltaTime * MOVEMENTSPEED;

		if (m_backwardSpeed < 0.0f)
		{
			m_backwardSpeed = 0.0f;
		}
	}

	Vector3 dir = m_ViewMatrix.Backward();
	dir.Normalize();

	// Update the position.
	m_PositionX -= m_backwardSpeed * dir.x;
	m_PositionY -= m_backwardSpeed * dir.y * -1.0f;
	m_PositionZ -= m_backwardSpeed * dir.z;
	//m_lookAt -= m_backwardSpeed * m_ViewMatrix.Forward() * -1.0f;
}

void CameraClass::MoveCameraUpward(bool keydown, float deltaTime)
{
	// Update the upward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		m_upwardSpeed += deltaTime * MOVEMENTSPEED;

		if (m_upwardSpeed > (deltaTime * MOVEMENTSPEED * 2))
		{
			m_upwardSpeed = deltaTime * MOVEMENTSPEED * 2;
		}
	}
	else
	{
		m_upwardSpeed -= deltaTime * MOVEMENTSPEED;

		if (m_upwardSpeed < 0.0f)
		{
			m_upwardSpeed = 0.0f;
		}
	}

	// Update the height position.
	m_PositionY += m_upwardSpeed;
}

void CameraClass::MoveCameraDownward(bool keydown, float deltaTime)
{
	// Update the downward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		m_downwardSpeed += deltaTime * MOVEMENTSPEED;

		if (m_downwardSpeed > (deltaTime * MOVEMENTSPEED * 2))
		{
			m_downwardSpeed = deltaTime * MOVEMENTSPEED * 2;
		}
	}
	else
	{
		m_downwardSpeed -= deltaTime * MOVEMENTSPEED;

		if (m_downwardSpeed < 0.0f)
		{
			m_downwardSpeed = 0.0f;
		}
	}

	// Update the height position.
	m_PositionY -= m_downwardSpeed;
}

void CameraClass::MoveCameraLeft(bool keydown, float deltaTime)
{
	if (keydown)
	{
		m_LeftSpeed += deltaTime * MOVEMENTSPEED;

		if (m_LeftSpeed > (deltaTime * MOVEMENTSPEED * 2))
		{
			m_LeftSpeed = deltaTime * MOVEMENTSPEED * 2;
		}
	}
	else
	{
		m_LeftSpeed -= deltaTime * MOVEMENTSPEED;

		if (m_LeftSpeed < 0.0f)
		{
			m_LeftSpeed = 0.0f;
		}
	}

	// Update the position.
	m_PositionX -= m_LeftSpeed;
}

void CameraClass::MoveCameraRight(bool keydown, float deltaTime)
{
	if (keydown)
	{
		m_rightSpeed += deltaTime * MOVEMENTSPEED;

		if (m_rightSpeed > (deltaTime * MOVEMENTSPEED * 2))
		{
			m_rightSpeed = deltaTime * MOVEMENTSPEED * 2;
		}
	}
	else
	{
		m_rightSpeed -= deltaTime * MOVEMENTSPEED;

		if (m_rightSpeed < 0.0f)
		{
			m_rightSpeed = 0.0f;
		}
	}

	// Update the position.
	m_PositionX += m_rightSpeed;
 
}

void CameraClass::Render(float deltaTime)
{
	Vector3 position = Vector3(m_PositionX, m_PositionY, m_PositionZ);

	float pitch = XMConvertToRadians(m_Pitch);
	float yaw = XMConvertToRadians(m_Yaw);
	float roll = XMConvertToRadians(m_Roll);
	
	Mat4 rotationMatrix = XMMatrixRotationRollPitchYaw( pitch, yaw, roll );
	Vector3 lookAtNew = XMVector3TransformCoord(m_lookAt, rotationMatrix);
	Vector3 upNew = XMVector3TransformCoord(m_up, rotationMatrix);
	 
	lookAtNew += position;

	m_ViewMatrix = XMMatrixLookAtLH(position, lookAtNew, upNew);
}

void CameraClass::GetViewMatrix(Mat4& viewmatrix)
{
	viewmatrix = m_ViewMatrix;
}
