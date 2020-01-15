#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include <WICTextureLoader.h>

#include <SimpleMath.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib") 
#pragma comment(lib, "D3DCompiler.lib") 


#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <vector>
#include <string>
#include <fstream>
#include <assert.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

typedef Matrix Mat4;

const float DEG_TO_RAD = 0.0174532925f;

#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;

#define DXASSERT(a) assert(SUCCEEDED(a))

#include <vld.h>
