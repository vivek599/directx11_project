#include "WindowClass.h"


WindowClass::WindowClass()
{
	m_deltaTime = 0.0;
	m_typingDelay = 0.f;
	m_Input = 0;
	m_Graphics = 0;
	m_cpuUsage = 0;
}

WindowClass::~WindowClass()
{
	Shutdown();
}

bool WindowClass::Initialize()
{
	int screenWidth = 0, screenHeight = 0;
	bool result;
	QueryPerformanceFrequency(&frequency);

	InitializeWindows(screenWidth, screenHeight);

	m_cpuUsage.reset ( new CpuUsageClass());
	assert(m_cpuUsage);

	m_cpuUsage->Initialize();


	m_Input.reset(new InputClass());
	assert(m_Input);


	result = m_Input->Initialize(m_hInstance,m_hwnd, screenWidth, screenHeight );
	assert(result);


	m_Graphics.reset( new GraphicClass());

	assert(m_Graphics);


	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);

	assert(result);

	m_rawMouse.Initialize(m_hwnd);
	m_rawKeyboard.Initialize(m_hwnd);

	return true;



}

void WindowClass::Shutdown()
{
	ShutdownWindows();
}

void WindowClass::Run()
{
	MSG msg;
	bool result;
	m_bExitApp = false;
	ZeroMemory(&msg, sizeof(MSG));
	m_bExitApp = false;

	while (!m_bExitApp)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE ))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT) 
		{ 
			m_bExitApp = true; 
		} 
		else 
		{ 
			result = Frame(); 
			if (!result || BaseClass::KeyDown(VK_ESCAPE)) 
			{ 
				m_bExitApp = true; 
			}
		}

	}

}

LRESULT CALLBACK WindowClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		case WM_INPUT:
		{
			m_rawMouse.update(lparam);
			m_rawKeyboard.update(lparam);
			return 0;
		}

		//case WM_CHAR:
		//{
		//	//if ((HIWORD(lparam) & KF_REPEAT) == 0)
		//	{
		//		m_Input->KeyDown((unsigned int)wparam);
		//	}
		//	//MessageBox( hwnd, L"WM_CHAR", L"down", MB_OK);
		//	return 0;
		//}

		//case WM_KEYUP:
		//{
		//	m_Input->KeyUp((unsigned int)wparam);
		//	//MessageBox(hwnd, LPCWSTR(to_wstring((unsigned int)wparam).c_str()), L"up", MB_OK);
		//	return 0;
		//}

		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}

}

bool WindowClass::Frame()
{
	bool result;
	DxTime t; t.Start(); 
	
	m_cpuUsage->Frame();

	result = m_Input->Frame(m_deltaTime);
	assert(result);


	if (!m_pauseGameLoop)
	{
		result = m_Graphics->Frame(m_deltaTime);
		assert(result);
	}

	m_deltaTime = t.End("")/1000.0f; // in seconds
	m_deltaTime = m_deltaTime <= 0.0f ? 0.016 : m_deltaTime;

	ShowFps();
	
	//m_mouseRaw = {};
	m_rawKeyboard.updateKeyData();
	m_rawMouse.Clear(); 

	return true;
}

void WindowClass::ShowFps()
{
	//SetWindowTextA(m_hwnd, userInputString.c_str());
	int fps = int(1.0f / m_deltaTime);
	const int fpsSampls = 50;
	static int avgFpsArray[fpsSampls] = { 0 };
	static int avgFpsArrIndex = 0;

	avgFpsArray[avgFpsArrIndex % fpsSampls] = fps;
	avgFpsArrIndex++;
	int avgFps = 0;

	for (int i = 0; i < fpsSampls; i++)
	{
		avgFps += avgFpsArray[i];
	}

	avgFps /= fpsSampls;

	userInputString = string("Cpu%: ") + to_string(m_cpuUsage->GetCpuPercentage()) + string("\nDeltaTime: ") + to_string(m_deltaTime) + string("\nFPS: ") + to_string(avgFps);
}

void WindowClass::ProcessKeyboardAndMouseEvents()
{
	
}

void WindowClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	appHandle = this;

	m_hInstance = GetModuleHandle(NULL);
	m_AppName = L"Directx11_Project";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_AppName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	//determine destop resolution
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);
	
	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned int)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned int)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = posY = 0;

	}
	else
	{
		screenWidth = 1280;
		screenHeight = 768;
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	}

	m_hwnd = CreateWindowEx(	WS_EX_APPWINDOW, 
								m_AppName, 
								m_AppName, 
								WS_OVERLAPPEDWINDOW/*WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_POPUP*/, 
								posX, 
								posY,
								screenWidth, 
								screenHeight, 
								NULL, 
								NULL, 
								m_hInstance, 
								NULL );

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
	
	ShowCursor(true);
	

}

void WindowClass::ShutdownWindows()
{
	ShowCursor(true);

	if (FULL_SCREEN	)
	{
		ChangeDisplaySettings(NULL, 0);

	}

	DestroyWindow(m_hwnd);
	m_hwnd = 0;

	UnregisterClass(m_AppName, m_hInstance);
	m_hInstance = 0;

	appHandle = NULL;


}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
			return appHandle->MessageHandler(hwnd, umsg, wparam, lparam);
	}

}
