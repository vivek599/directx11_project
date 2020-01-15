#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WindowsX.h>

#include "BaseClass.h"

#include "inputclass.h"
#include "graphicsclass.h"
#include "CpuUsageClass.h"


class SystemClass : BaseClass
{
public:
	SystemClass();
	SystemClass(const SystemClass& obj);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();

	void ProcessKeyboardAndMouseEvents();

	void InitializeWindows(int& screenWidth, int& screenHeight);
	void ShutdownWindows();

	LPCWSTR m_AppName;
	HINSTANCE m_hInstance;
	HWND m_hwnd;

	InputClass* m_Input;
	GraphicClass* m_Graphics;

	LARGE_INTEGER frequency;
	float deltaTime;
	float m_typingDelay;

	CpuUsageClass* m_cpuUsage;


};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass* appHandle = 0;
