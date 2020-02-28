#pragma once

#include "RawMouse.h"
#include "RawKeyboard.h"


class BaseClass
{
public:
	BaseClass();
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
	static bool m_pauseGameLoop;
	static RawMouse m_rawMouse;

	static bool KeyUp(int key);
	static bool KeyDown(int key);
	static RawKeyboard m_rawKeyboard;
};

