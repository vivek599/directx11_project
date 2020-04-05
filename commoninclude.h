#pragma once

#define WIN32_LEAN_AND_MEAN

#define NOWARNINGS 1

#if NOWARNINGS

#pragma warning(disable : 4005 4244 4267 4100 4101 )

#endif 

#include <Windows.h>
#include <shobjidl.h> //open file dialog

#include <d2d1_3.h>
#include <dwrite.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <thread>
#include <future>

#include <WICTextureLoader.h>

#include <SimpleMath.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib") 
#pragma comment(lib, "D3DCompiler.lib") 
#pragma comment(lib, "d2d1.lib" )
#pragma comment(lib, "dwrite.lib" )


#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <vector>
#include <string>
#include <fstream>
#include <assert.h>
#include <atlstr.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include "Logger.h"


using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

typedef Matrix Mat4;


//#include <vld.h>

#include "common_def.h"
