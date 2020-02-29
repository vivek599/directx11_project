#include "BaseClass.h"


string BaseClass::userInputString;
bool BaseClass::m_moveCameraForward;
bool BaseClass::m_moveCameraBackward;
bool BaseClass::m_moveCameraLeft;
bool BaseClass::m_moveCameraRight;
bool BaseClass::m_moveCameraUp;
bool BaseClass::m_moveCameraDown;

bool BaseClass::m_bExitApp;
int BaseClass::xMousePos;
int BaseClass::yMousePos;
bool BaseClass::m_pauseGameLoop;
RawMouse BaseClass::m_rawMouse;

bool BaseClass::KeyUp(int key)
{
	if (key == VK_SPACE)
	{
		return m_rawKeyboard.IsSpaceKeyUp();
	}

	return false;
}

bool BaseClass::KeyDown(int key)
{
	int uKey = key & ~32;
	if ( uKey >= 'A' && uKey <= 'Z' )
	{
		return m_rawKeyboard.IsAlphabeticKeyDown(uKey);
	}

	if (key == VK_ESCAPE)
	{
		return m_rawKeyboard.IsEscapeKeyDown();
	}

	if (key == VK_SPACE)
	{
		return m_rawKeyboard.IsSpaceKeyDown();
	}

	return false;
}

bool BaseClass::ALTF4Down()
{
	return m_rawKeyboard.IsAltF4();
}

RawKeyboard BaseClass::m_rawKeyboard;

BaseClass::BaseClass()
{
	userInputString = "";
	m_bExitApp = false;
	m_moveCameraForward = false;
	m_moveCameraBackward = false;
	m_moveCameraLeft = false;
	m_moveCameraRight = false;
	m_moveCameraUp = false;
	m_moveCameraDown = false;
	xMousePos = 0;
	yMousePos = 0;
	m_pauseGameLoop = false;
	m_rawMouse = {};
	m_rawKeyboard = {};

}

BaseClass::~BaseClass()
{

}

wstring BaseClass::GetOpenFilePathFromDialog()
{
	wstring filePathstr;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
		if (SUCCEEDED(hr))
		{
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pfilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pfilePath);
					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						filePathstr = wstring(pfilePath);
						//wcstombs_s( NULL, filePathstr, 260, pfilePath, 260 );
						//MessageBox(NULL, (LPCWSTR)filePathstr, L"File Path", MB_OK);

						CoTaskMemFree(pfilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}

	return filePathstr;
}