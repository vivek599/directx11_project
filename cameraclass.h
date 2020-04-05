#pragma once

#include "commoninclude.h"
#include "BaseClass.h"
const float MOVEMENTSPEED = 25.f;
const float MOUSESPEED = 20.f;

class CameraClass : BaseClass
{
public:
	CameraClass();
	~CameraClass();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	Vector3 GetPosition();
	Vector3 GetRotation();

	void Update(float deltaTime);
	void Render(float deltaTime);
	Mat4 GetViewMatrix();

private:
	Vector3 m_Position;

	float m_Pitch;
	float m_Yaw;
	float m_Roll;

	Vector3 m_lookAt;
	Vector3 m_up;

	Mat4 m_ViewMatrix;
	void Fly(float deltaTime);
};

