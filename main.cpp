

#include "WindowClass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR psCmdLine, int iCmdShow)
{
	bool result;
 
	unique_ptr<WindowClass> system; system.reset( new WindowClass());
	MYASSERT(system);


	result = system->Initialize();

	if (result)
	{
		system->Run();
	}



	return 0;

}


