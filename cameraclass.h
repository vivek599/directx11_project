#pragma once

#include "commoninclude.h"
#include "BaseClass.h"
const float MOVEMENTSPEED = 25.f;

class CameraClass : BaseClass
{
public:
	CameraClass();
	~CameraClass();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	Vector3 GetPosition();
	Vector3 GetRotation();


	void MoveCameraForward(bool keydown, float deltaTime);
	void MoveCameraBackward(bool keydown, float deltaTime);
	void MoveCameraLeft(bool keydown, float deltaTime);
	void MoveCameraRight(bool keydown, float deltaTime);
	void MoveCameraUpward(bool keydown, float deltaTime);
	void MoveCameraDownward(bool keydown, float deltaTime);

	void Update(float deltaTime);
	void Render(float deltaTime);
	void GetViewMatrix(Mat4& viewmatrix);

private:
	Vector3 m_Position;

	float m_Pitch;
	float m_Yaw;
	float m_Roll;

	Vector3 m_lookAt;
	Vector3 m_up;

	Mat4 m_ViewMatrix;

	float m_forwardSpeed;
	float m_upwardSpeed;
	float m_LeftSpeed;
	float m_rightSpeed;
	float m_downwardSpeed;
	float m_backwardSpeed;
};

