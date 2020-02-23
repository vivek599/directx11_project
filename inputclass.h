#pragma once

#include "BaseClass.h"

#define DIRECTINPUT_VERSION 0x0800


class InputClass : BaseClass
{
	
public:
	InputClass();
	~InputClass();

	bool Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
	void Shutdown();
	bool Frame(float deltaTime);

	bool IsEscPressed();
	void GetMouseLocation();
	void CopyKeyData();

	bool IsKeyDown(unsigned int key);
	bool IsKeyDownOnce(unsigned int key);
	
	void KeyDown(unsigned int input);
	void KeyUp(unsigned int input);
	void ResetWinKeys();
	POINT GetMouseLoc();
private:

	int m_ScreenWidth, m_ScreenHeight, m_MouseX, m_MouseY;

	bool m_WinKeys[2][256];
};



