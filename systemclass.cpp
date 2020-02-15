#include "systemclass.h"


SystemClass::SystemClass()
{
	deltaTime = 0.0;
	m_typingDelay = 0.f;
	m_Input = 0;
	m_Graphics = 0;
	m_cpuUsage = 0;
}

SystemClass::SystemClass(const SystemClass& obj)
{
	UNREFERENCED_PARAMETER(obj);

}

SystemClass::~SystemClass()
{
	Shutdown();
}

bool SystemClass::Initialize()
{
	int screenWidth = 0, screenHeight = 0;
	bool result;
	QueryPerformanceFrequency(&frequency);

	InitializeWindows(screenWidth, screenHeight);

	m_cpuUsage.reset ( new CpuUsageClass());
	if (!m_cpuUsage)
	{
		return false;
	}
	m_cpuUsage->Initialize();


	m_Input.reset(new InputClass());
	if (!m_Input)
	{
		return false;
	}

	result = m_Input->Initialize(m_hInstance,m_hwnd, screenWidth, screenHeight );
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the direct input object.", L"Error", MB_OK);
		return false;
	}

	m_Graphics.reset( new GraphicClass());

	if (!m_Graphics)
	{
		return false;
	}

	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);

	if (!result)
	{
		return false;
	}

	return true;



}

void SystemClass::Shutdown()
{
	ShutdownWindows();
}

void SystemClass::Run()
{
	MSG msg;
	bool result;
	done = false;
	ZeroMemory(&msg, sizeof(MSG));
	done = false;

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE ))
		{
			if (msg.message == WM_QUIT)
			{
				done = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			result = Frame();
			if (!result || m_Input->IsEscPressed())
			{
				done = true;
			}

		}

			
	}
	

}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		case WM_CHAR:
		{
			//if ((HIWORD(lparam) & KF_REPEAT) == 0)
			{
				m_Input->KeyDown((unsigned int)wparam);
			}
			//MessageBox( hwnd, L"WM_CHAR", L"down", MB_OK);
			return 0;
		}

		case WM_KEYUP:
		{
			m_Input->KeyUp((unsigned int)wparam);
			//MessageBox(hwnd, LPCWSTR(to_wstring((unsigned int)wparam).c_str()), L"up", MB_OK);
			return 0;
		}

		case WM_MOUSEMOVE:
		{
			xMousePos = GET_X_LPARAM(lparam);
			yMousePos = GET_Y_LPARAM(lparam);

			return 0;
		}

		default:
			return DefWindowProc(hwnd,umsg, wparam, lparam);
	}

}

bool SystemClass::Frame()
{
	bool result;

	LARGE_INTEGER t1, t2; 		
	QueryPerformanceCounter(&t1);//counter start
	
	m_cpuUsage->Frame();

	result = m_Input->Frame(deltaTime);
	if (!result)
	{
		return false;
	}
	int mouseX = int((xMousePos/1024.f)*360.f);//m_Input->GetMouseLoc().x;
	int mouseY = int((yMousePos/768.f) * 360.f);//m_Input->GetMouseLoc().y;
	
	result = m_Graphics->Frame(deltaTime);
	if (!result)
	{
		return false;
	}

	QueryPerformanceCounter(&t2);//counter end
	deltaTime = (float)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart; // in seconds
	//SetWindowTextA(m_hwnd, userInputString.c_str());
	int fps = int(1 / deltaTime);
	SetWindowTextA(m_hwnd, ( string("Cpu%: ") + to_string(m_cpuUsage->GetCpuPercentage()) + string(" DeltaTime: ") + to_string(deltaTime) + string(" FPS: ") + to_string(fps)
		+ string(" mX: ") + to_string(mouseX) + string(" mY: ") + to_string(mouseY) + string(" Input: ") + userInputString ).c_str());

	return true;
}

void SystemClass::ProcessKeyboardAndMouseEvents()
{
	
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
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

void SystemClass::ShutdownWindows()
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
