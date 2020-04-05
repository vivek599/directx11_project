#include "graphicsclass.h"



GraphicClass::GraphicClass()
{
 
	m_Models.clear();
 
}

GraphicClass::~GraphicClass()
{
	Shutdown();
}

bool GraphicClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	 
	m_D3D.reset( new D3DClass());
	MYASSERT(m_D3D);


	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNCE_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR );
	
	MYASSERT(result);
 

	//write video card info to a file
	m_D3D->WriteVideoCardInfoToFile();

	m_Camera.reset(new CameraClass());
	MYASSERT(m_Camera);
 
	m_Camera->SetPosition(0.f, 0.f, -40.f);
	m_Camera->Render(0.f);
	m_ViewMatrix2D = m_Camera->GetViewMatrix();

	// Create the frustum object.
	m_frustum.reset( new FrustumClass());
	MYASSERT(m_frustum);
 

	m_Bitmap.reset(new BitmapClass());
	MYASSERT(m_Bitmap);


	result = m_Bitmap->Initialize(m_D3D->GetDevice().Get(), hwnd, screenWidth, screenHeight, (WCHAR*)L"../Textures/stone_wall1.jpg", 128, 128);
	MYASSERT(result);

	m_WorldTextQuad.reset(new ATexture( m_D3D.get() ));



	// Create the light object.
	m_Light.reset( new LightClass());
	MYASSERT(m_Light);


	// Initialize the light object.
	m_Light->SetAmbientColor(0.5f, 0.5f, 0.5f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 0.0f, 1.0f);
	Vector3 ldir = Vector3(0.f) - Vector3(1.0f, 1.0f, -1.0f);
	m_Light->SetDirection(ldir.x, ldir.y, ldir.z);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);
	m_Light->GenerateProjectionMatrix(SCREEN_NEAR, SCREEN_DEPTH);
	m_Light->GenerateOrthoMatrix(SCREEN_NEAR, SCREEN_DEPTH);

	// Create the render to texture object.
	m_renderTexture.reset( new RenderTextureClass());
	MYASSERT(m_renderTexture);


	// Initialize the render to texture object.
	result = m_renderTexture->Initialize(m_D3D->GetDevice().Get(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_NEAR, SCREEN_DEPTH);
	MYASSERT(result);


#if 1

	auto func = [=]( const char* modelfile, const wchar_t* texture, const wchar_t* normal) -> ModelClass* { return new ModelClass(m_D3D->GetDevice().Get(), modelfile, texture, normal); };
	
	DxTime t; t.Start();
	future<ModelClass*> future1 = async(launch::async, func, "../Models/greek_statue/Statue_v1_L2.fbx", L"../Models/greek_statue/DavidFixedDiff.jpg", L"../Models/greek_statue/Statue_v1_L2_n.jpg");
	future<ModelClass*> future2 = async(launch::async, func, "../Models/angel_lucy/alucy_lowpoly2.fbx", L"../Models/angel_lucy/Concrete_pink_1S.jpg", L"../Models/angel_lucy/RockSharp0036_1S_n.jpg");
	future<ModelClass*> future3 = async(launch::async, func, "../Models/greek_platform/plain_platform_123.fbx", L"../Models/greek_platform/plain_platform.jpg", L"../Models/greek_platform/greek_platform1_n.jpg");
	future<ModelClass*> future4 = async(launch::async, func, "../Models/voronoi/voronoi_box.fbx", L"../Models/voronoi/gray_tex1.jpg", L"../Models/voronoi/gray_tex1_n.jpg");
	future<ModelClass*> future5 = async(launch::async, func, "../Models/greek_platform/greek_platform1.fbx", L"../Models/greek_platform/greek_platform1.jpg", L"../Models/greek_platform/greek_platform1_n.jpg");
	
	m_Models.push_back(future1.get());
	m_Models.push_back(future2.get());
	m_Models.push_back(future3.get());
	m_Models.push_back(future4.get());
	m_Models.push_back(future5.get());
	DXLOG( "Time = %f",t.End("") );

#else

	DxTime t; t.Start();
	// Initialize the model object.
	m_Models.push_back(new ModelClass(m_D3D->GetDevice().Get(), "../Models/greek_statue/Statue_v1_L2.fbx", L"../Models/greek_statue/DavidFixedDiff.jpg", L"../Models/greek_statue/Statue_v1_L2_n.jpg"));
	m_Models.push_back(new ModelClass(m_D3D->GetDevice().Get(), "../Models/angel_lucy/alucy_lowpoly2.fbx", L"../Models/angel_lucy/Concrete_pink_1S.jpg", L"../Models/angel_lucy/RockSharp0036_1S_n.jpg"));
	//m_Models.push_back(new ModelClass(m_D3D->GetDevice().Get(), "Models/greek_platform/greek_platform1.fbx", L"../Models/greek_platform/greek_platform1.jpg", L"../Models/greek_platform/greek_platform1_n.jpg"));
	m_Models.push_back(new ModelClass(m_D3D->GetDevice().Get(), "../Models/greek_platform/plain_platform.fbx", L"../Models/greek_platform/plain_platform.jpg", L"../Models/greek_platform/greek_platform1_n.jpg"));
	m_Models.push_back(new ModelClass(m_D3D->GetDevice().Get(), "../Models/voronoi/voronoi_box.fbx", L"../Models/voronoi/gray_tex1.jpg", L"../Models/voronoi/gray_tex1_n.jpg"));
	DXLOG("Time = %f", t.End(""));

#endif
	/*int maxMushrooms = 20;
	for (int i = 0; i < maxMushrooms; i++)
	{
		ModelClass* tmp = new ModelClass(m_D3D->GetDevice(), "Models/MushroomButton/Mushroom_lp.obj", L"../Models/MushroomButton/Mushroom_lp.jpg");
		tmp->SetPosition(Vector3(rand() % maxMushrooms - maxMushrooms * 0.5f, rand() % maxMushrooms - maxMushrooms * 0.5f, rand() % maxMushrooms - maxMushrooms * 0.5f));
		m_Models.push_back(tmp);
	}*/
	
	for (int i = 0; i < m_Models.size(); i++)
	{
		MYASSERT(m_Models[i]);
// 		if (!m_Models[i])
// 		{
// 			return false;
// 		}
	}
	
	m_skybox.reset( new SkyboxClass());
	MYASSERT(m_skybox);

	result = m_skybox->Initialize(m_D3D->GetDevice().Get());
	MYASSERT(result);

	m_canvas2D.reset(new Canvas2D());
	MYASSERT(m_canvas2D);
	m_canvas2D->Initialize(m_D3D.get());
	m_canvas2D->InitializeWorldText( m_WorldTextQuad.get() );

	return true;
	
}

void GraphicClass::Shutdown()
{
 
	for (int i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i])
		{
			delete m_Models[i]; 
			m_Models[i] = 0;
		}
	}

	m_Models.clear();
	
}

bool GraphicClass::Frame(float deltaTime)
{
	bool result;
	static float lightPositioncntr = 0.f;

	// Update the position of the light each frame.
	lightPositioncntr += 1.f * deltaTime;
	
	// Update the position of the light.
	//m_Light->SetPosition( 15.f * sin(lightPositioncntr) , 8.0f, 15.f * cos(lightPositioncntr));
	Vector3 lpos = Vector3( -125.f * sin(lightPositioncntr), 125.0f, -60.f * cos(lightPositioncntr) /*)* (1 + 0.5f * sin(lightPositioncntr)*/);
	m_Light->SetPosition(lpos.x, lpos.y, lpos.z);

	result = Render(deltaTime);
	if (!result)
	{
		return false;
	}


	return true;
}

bool GraphicClass::Render(float deltaTime)
{
	bool result;

	Mat4 view = m_Camera->GetViewMatrix();
	Mat4 world = m_D3D->GetWorldMatrix();
	Mat4 projection = m_D3D->GetProjectionMatrix();
	Mat4 ortho = m_D3D->GetOrthoMatrix();

	//The first pass of our render is to a texture now.
	// Render the entire scene to the texture first.
	result = RenderToTexture(deltaTime);
	if (!result)
	{
		return false;
	}
	
	//The second pass of our render is to the back buffer as normal.
	// Clear the buffers to begin the scene.
	
	m_D3D->BeginScene(0.0f, 0.0f, 0.5f, 1.0f);

	// Render the scene as normal to the back buffer.
	result = RenderScene(deltaTime, false);
	if (!result)
	{
		return false;
	}

	m_WorldTextQuad->SetScale(1.0f);
	m_WorldTextQuad->SetPosition(Vector3(0.0f, 10.0f, 0.0f));

	m_WorldTextQuad->Update(world, view, projection);
	m_WorldTextQuad->Render(m_D3D.get());

	//turn off z buffer to begin 2d drawing
	m_D3D->TurnZBufferOff();

	result = m_Bitmap->Render(m_D3D->GetContext().Get(), world, m_ViewMatrix2D, ortho, 100, 100, m_renderTexture->GetShaderResourceView().Get());
	if (!result)
	{
		return false;
	}

	//turn z buffer back on for 3d rendering
	m_D3D->TurnZBufferOn();


	result = m_canvas2D->RenderUI2D(deltaTime);
	if (!result)
	{
		return false;
	}


	//present the rendered scene to screen
	m_D3D->EndScene();


	//*******************************

	 
	return true;
}

bool GraphicClass::RenderToTexture(float deltaTime)
{
	bool result;


	// Set the render target to be the render to texture.
	m_renderTexture->SetRenderTarget(m_D3D->GetContext().Get());
	
	//Clear the render to texture background to blue so we can differentiate it from the rest of the normal scene.
	// Clear the render to texture.
	m_renderTexture->ClearRenderTarget(m_D3D->GetContext().Get(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Render the scene now and it will draw to the render to texture instead of the back buffer. /*depthpass - true*/
	result = RenderScene(deltaTime, true);
	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
	
	// Reset the viewport back to the original.
	m_D3D->ResetViewport();

	return true;
}

bool GraphicClass::RenderScene(float deltaTime, bool depthPass)
{
	bool result;
	static float yaw;
	static float angleCntr = 0;

	angleCntr += deltaTime * 1.f;
	yaw = 180.0f * 0.41678f * sinf(angleCntr);

	//clear buffer to begin scene
	m_D3D->TurnZBufferOn();

	m_Camera->Update(deltaTime);
	m_Camera->Render(deltaTime);

	Mat4 view = m_Camera->GetViewMatrix();
	Mat4 world = m_D3D->GetWorldMatrix();
	Mat4 projection = m_D3D->GetProjectionMatrix();
	Mat4 ortho = m_D3D->GetOrthoMatrix();
	
	// Generate the light view matrix based on the light's position.
	m_Light->GenerateViewMatrix();

	// Get the view and orthographic matrices from the light object.
	Mat4 lightViewMatrix = m_Light->GetViewMatrix();
	Mat4 lightProjectionMatrix = m_Light->GetProjectionMatrix(); 

	// Construct the frustum.
	m_frustum->ConstructFrustum(SCREEN_DEPTH, projection, view);

	if (!depthPass)
	{
		// Turn off back face culling.
		m_D3D->TurnOffCulling();

		// Turn off the Z buffer.
		m_D3D->TurnZBufferOff();
		// Render the sky dome using the sky dome shader.
		result = m_skybox->Render(m_D3D->GetContext().Get(), world, view, projection, m_Camera->GetPosition());
		if (!result)
		{
			return false;
		}

		// Turn the Z buffer back on.
		m_D3D->TurnZBufferOn();
		// Turn back face culling back on.
		m_D3D->TurnOnCulling();

	}


	m_Models[0]->SetRotation(Vector3(90.0f, yaw, 0.f));
	m_Models[0]->SetPosition(Vector3(-10.f, 0.f, 0.f));
	m_Models[0]->SetScale3D(Vector3(8.f));

	m_Models[1]->SetPosition(Vector3(10.f, 0.f, 0.f));
	m_Models[1]->SetRotation(Vector3(0.f, -90.0f + yaw, 0.f));
	m_Models[1]->SetScale3D(Vector3(2.f));

	m_Models[3]->SetPosition(Vector3(0.f, 2.f, 0.f));
	m_Models[3]->SetScale3D(Vector3(2.f));

	m_Models[4]->SetPosition(Vector3(0.f, -0.2f, 0.f));
	m_Models[4]->SetScale3D(Vector3(2.0f, 2.0f, 2.0f));

	for (int i = 0; i < m_Models.size(); i++)
	{
		Vector3 pos = m_Models[i]->GetPosition();
		if (m_frustum->CheckBox(m_Models[i]->GetBBox().min, m_Models[i]->GetBBox().max))
		{
			result = m_Models[i]->Render(m_D3D->GetContext().Get(), m_renderTexture.get(), m_Light.get(), m_Camera.get(), world, view, projection, lightViewMatrix, lightProjectionMatrix, depthPass);
			if (!result)
			{
				return false;
			}
		}

	}

	return true;
}
