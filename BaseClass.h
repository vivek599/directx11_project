#pragma once

#include <Windows.h>
#include <shobjidl.h> //open file dialog

#include <string>
using namespace std;


class BaseClass
{
public:
	BaseClass();
	BaseClass(const BaseClass& obj);
	~BaseClass();

	wstring GetOpenFilePathFromDialog();
	static string userInputString;
	static bool m_moveCameraForward;
	static bool m_moveCameraBackward;
	static bool m_moveCameraLeft;
	static bool m_moveCameraRight;
	static bool m_moveCameraUp;
	static bool m_moveCameraDown;
	static bool done;
	static int xMousePos;
	static int yMousePos;
};

