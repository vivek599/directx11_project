
#define NOWARNINGS 1

#if NOWARNINGS

#pragma warning(disable : 4005 4244 4267 4100 )

#endif 

#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR psCmdLine, int iCmdShow)
{
	bool result;
 
	unique_ptr<SystemClass> system; system.reset( new SystemClass());
	assert(system);


	result = system->Initialize();

	if (result)
	{
		system->Run();
	}



	return 0;

}


