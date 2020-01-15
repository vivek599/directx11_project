#include "cameraclass.h"


CameraClass::CameraClass()
{
	m_PositionX = 0.f;
	m_PositionY = 0.f;
	m_PositionZ = 0.f;

	m_RotationX = 0.f;
	m_RotationY = 0.f;
	m_RotationZ = 0.f;

	m_forwardSpeed = 0.f;
	m_upwardSpeed = 0.f;
	m_LeftSpeed = 0.f;
	m_rightSpeed = 0.f;
	m_downwardSpeed = 0.f;
	m_backwardSpeed = 0.f;

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
	m_RotationX = x;
	m_RotationY = y;
	m_RotationZ = z;
}

Vector3 CameraClass::GetPosition()
{
	return Vector3(m_PositionX, m_PositionY, m_PositionZ );
}

Vector3 CameraClass::GetRotation()
{
	return Vector3(m_RotationX, m_RotationY, m_RotationZ);
}

void CameraClass::Update(float deltaTime)
{
	SetRotation( (BaseClass::xMousePos/1024)*360.f, (BaseClass::yMousePos/768)*360.f, 0.f );
	MoveCameraForward(BaseClass::m_moveCameraForward, deltaTime);
	MoveCameraBackward(BaseClass::m_moveCameraBackward, deltaTime);
	MoveCameraLeft(BaseClass::m_moveCameraLeft, deltaTime);
	MoveCameraRight(BaseClass::m_moveCameraRight, deltaTime);
	MoveCameraUpward(BaseClass::m_moveCameraUp, deltaTime);
	MoveCameraDownward(BaseClass::m_moveCameraDown, deltaTime);
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

	// Update the position.
	m_PositionZ -= m_backwardSpeed;
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

	// Update the position.
	m_PositionZ += m_forwardSpeed;
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
	Vector3 up, position, lookAt;
	float yaw, pitch, roll;
	Mat4 rotationMatrix;

	up = Vector3(0.f, 1.f, 0.f);

	position = Vector3(m_PositionX, m_PositionY, m_PositionZ);

	lookAt = Vector3(0.f, 0.f, 1.f);

	pitch = m_RotationX * DEG_TO_RAD;
	yaw = m_RotationY * DEG_TO_RAD;
	roll = m_RotationZ * DEG_TO_RAD;

	XMVECTOR lookAtV = XMVECTOR(lookAt);
	XMVECTOR upV = XMVECTOR(up);
	XMVECTOR positionV = XMVECTOR(position);
	
	rotationMatrix = XMMatrixRotationRollPitchYaw( pitch, yaw, roll );
	lookAt = XMVector3TransformCoord( lookAtV, rotationMatrix);
	up = XMVector3TransformCoord(upV, rotationMatrix);

	lookAt = lookAt + position;

	m_ViewMatrix = XMMatrixLookAtLH(positionV, XMVECTOR(lookAt), XMVECTOR(up));
}

void CameraClass::GetViewMatrix(Mat4& viewmatrix)
{
	viewmatrix = m_ViewMatrix;
}
