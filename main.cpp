
#define NOWARNINGS 1

#if NOWARNINGS

#pragma warning(disable : 4005 4244 4267 4100 )

#endif 

#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR psCmdLine, int iCmdShow)
{
	bool result;
 
	SystemClass* system = new SystemClass;
	if (!system)
	{
		return 0;
	}

	result = system->Initialize();

	if (result)
	{
		system->Run();
	}

	system->Shutdown();
	delete system;
	system = 0;

	return 0;

}


