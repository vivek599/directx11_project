#pragma once

#include "RawMouse.h"

#include "inputclass.h"
#include "graphicsclass.h"
#include "CpuUsageClass.h"


class WindowClass : BaseClass
{
public:
	WindowClass();
	~WindowClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();

	void ShowFps();

	void ProcessKeyboardAndMouseEvents();

	void InitializeWindows(int& screenWidth, int& screenHeight);
	void ShutdownWindows();

	LPCWSTR m_AppName;
	HINSTANCE m_hInstance;
	HWND m_hwnd;

	unique_ptr<InputClass> m_Input;
	unique_ptr<GraphicClass> m_Graphics;

	LARGE_INTEGER frequency;
	float m_deltaTime;
	float m_typingDelay;

	unique_ptr < CpuUsageClass > m_cpuUsage;
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static WindowClass* appHandle = 0;

