// Mech Engine Template
#ifndef MY_DIRECT_X_H
#define MY_DIRECT_X_H
#include "MyDirectX.h"
#endif

#ifndef MECH_H
#define MECH_H
#include "Mech.h"
#endif

// program values
const std::string APPTITLE = "Mech Engine"; 
const int SCREENW = 768;
const int SCREENH = 576; 
const float NEAR_LENGTH = 1.0f; 
const float FAR_LENGTH = 100.0f; 
const float FOV = D3DX_PI / 4.0f;
float SCREEN_ASPECT;
const bool FULLSCREEN = false; // overrides SCREENW and SCREENH
D3DVIEWPORT9 m_mainViewport;
CAMERA camObj;
float m_screenWidth, m_screenHeight;
float screenAspect;
D3DXVECTOR3 rayIntersectPos;

// 3D meshes
MODEL *floorMesh;
MODEL *brickMesh;

// Units
UNIT tinyUnit;
UNIT zombieUnit;

// vector that holds all units
std::vector<UNIT*> allUnits;

// current selected unit
std::string currentSU;
// selection id
DWORD selectID = NULL;

// controller vibration variables
int vibrating = 0;
int vibration = 100;

// timing variables
DWORD refresh = 0;
DWORD screentime = 0;
double screenfps = 0.0;
double screencount = 0.0;
DWORD coretime = 0;
double corefps = 0.0;
double corecount = 0.0;
DWORD currenttime = 0;

// timing variables for CModel
double startTime;
LARGE_INTEGER nowTime;
LONGLONG ticks;

LARGE_INTEGER ourTime;

// font variables
LPD3DXFONT debugText = NULL;

// mouse variables
POINT cursPt; // cursor location

void Vibrate(int contnum, int amount, int length)
{
	vibrating = 1;
	vibration = length;
	XInput_Vibrate(contnum, amount);
}

// allows us to create a user-interface
void Draw_HUD()
{
	// debug text
	FontPrint(debugText, 10, 10, "Mouse currently at (" + ToString(cursPt.x) + ", " + ToString(cursPt.y) + ")");
	FontPrint(debugText, 10, 30, "Ray intersection at (" +
		ToString(rayIntersectPos.x) + ", " +
		ToString(rayIntersectPos.y) + ", " +
		ToString(rayIntersectPos.z) + ")");
	FontPrint(debugText, 10, 50, "The current selected unit is: " +
		currentSU);
	FontPrint(debugText, 10, 70, "Tiny current animation is: " + 
		ToString(tinyUnit.GetCurrentAnimation()) +
		" out of " +
		ToString(tinyUnit.GetMaxAnimations()));
	FontPrint(debugText, 10, 90, "Zombie current animation is: " + 
		ToString(zombieUnit.GetCurrentAnimation()) +
		" out of " +
		ToString(zombieUnit.GetMaxAnimations()));
}

bool Game_Init(HWND window)
{
	// initialize Direct3D
	if (!Direct3D_Init(window, SCREENW, SCREENH, FULLSCREEN))
	{
		MessageBox(0, "Error initializing Direct3D", "ERROR", 0);
		return false;
	}

	// initialize DirectInput
	if (!DirectInput_Init(window))
	{
		MessageBox(0, "Error initializing DirectInput", "ERROR", 0);
		return false;
	}

	// initialize DirectSound
	if (!DirectSound_Init(window))
	{
		MessageBox(window, "Error initializing DirectSound", APPTITLE.c_str(), 0);
		return false;
	}

	QueryPerformanceCounter(&ourTime);
	startTime = (double)ourTime.QuadPart;

	QueryPerformanceFrequency(&ourTime);
	ticks = ourTime.QuadPart;

	// set screen width and height from viewport
	d3ddev->GetViewport(&m_mainViewport);
	m_screenWidth = m_mainViewport.Width;
	m_screenHeight = m_mainViewport.Height;
	SCREEN_ASPECT = m_screenWidth / m_screenHeight;

	// set up the camera
	camObj.SetCamera(0.0f, 10.7f, -7.5f); // ~55 degree angle

	// set renderstates
	SetRenderstate(d3ddev, false, true);

	// set fonts
	debugText = MakeFont("Arial", 24);

	// load models
	floorMesh = LoadModel("floor.x");
	if (floorMesh == NULL)
	{
		MessageBox(window, "Error loading floorMesh!", APPTITLE.c_str(), MB_OK);
		return 0;
	}

	// set the plane
	floorMesh->translate.x = 3.0f;
	floorMesh->translate.y = -1.0f;
	floorMesh->translate.z = 2.0f;

	brickMesh = LoadModel("brickWall.x");
	if (brickMesh == NULL)
	{
		MessageBox(window, "Error loading brickMesh!", APPTITLE.c_str(), MB_OK);
		return 0;
	}

	// set the wall
	brickMesh->translate.x = 3.0f;
	brickMesh->translate.y = -1.0f;
	brickMesh->translate.z = 2.0f;

	tinyUnit.setUnit("tiny.x", "Tiny", 0.05f);
	tinyUnit.scale = D3DXVECTOR3(0.005f, 0.005f, 0.005f);
	tinyUnit.rotate.y = -90.0f;
	tinyUnit.rotate.z = 180.0f;
	// using CModel functions to set up unit
	tinyUnit.LoadXFile("tiny.x");
	allUnits.push_back(&tinyUnit);
	// set its animation
	tinyUnit.SetCurrentAnimation(tinyUnit.GetCurrentAnimation() + 1);

	zombieUnit.setUnit("zombieAnim.x", "Zombie", 0.02f);
	zombieUnit.LoadXFile("zombieAnim.x");
	allUnits.push_back(&zombieUnit);
	zombieUnit.SetCurrentAnimation(zombieUnit.GetCurrentAnimation() + 2);

	return true;
}

void Game_Run(HWND window)
{
	// make sure the Direct3D device is valid
	if (!d3ddev) return;

	// clear the scene
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0, 0, 100), 1.0f, 0);

	currenttime = timeGetTime();

	// calculate core frame rate
	corecount += 1.0;
	if (currenttime > coretime + 1000)
	{
		corefps = corecount;
		corecount = 0.0;
		coretime = currenttime;
	}

	// get cursor position
	GetCursorPos(&cursPt);
	
	// if windowed, convert screen coords to client coords
	if(!FULLSCREEN)
		ScreenToClient(window, &cursPt);

	// left click selects a unit
	if (KEY_DOWN(VK_LBUTTON))
	{
		for (int i = 0; i < allUnits.size(); i++)
		{
			if (rayIntersect(cursPt.x, cursPt.y, *allUnits[i], camObj,
				m_screenWidth, m_screenHeight, rayIntersectPos))
			{
				currentSU = allUnits[i]->name;
				selectID = i;
				break;
			}
		}
	}

	// right click tells unit where to go
	if (KEY_DOWN(VK_RBUTTON))

	{
		if (rayIntersect(cursPt.x, cursPt.y, *floorMesh, camObj,
			m_screenWidth, m_screenHeight, rayIntersectPos))
		{
			if(selectID >= 0) allUnits[selectID]->endPosition = rayIntersectPos;
		}
	}

	// Moves screen depending on mouse position
	D3DXVECTOR3 camOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	if (cursPt.y <= 0)
		camOffset.z += 0.15f;
	if (cursPt.y >= m_screenHeight -1)
		camOffset.z -= 0.15f;
	if (cursPt.x >= m_screenWidth - 1)
		camOffset.x += 0.15f;
	if (cursPt.x <= 0)
		camOffset.x -= 0.15f;

	camObj.translateCam(camOffset);

	// run update at ~60hz
	if (currenttime > refresh + 16)
	{
		refresh = currenttime;

		DirectInput_Update();

		// update controller vibration
		if (vibrating > 0)
		{
			vibrating++;
			if (vibrating > vibration)
			{
				XInput_Vibrate(0, 0);
				vibrating = 0;
			}
		}

		// calculate screen frame rate
		screencount += 1.0;
		if (currenttime > screentime + 1000)
		{
			screenfps = screencount;
			screencount = 0.0;
			screentime = currenttime;
		}

		// escape key exits
		if (KEY_DOWN(VK_ESCAPE))
			gameover = true;

		// controller Back button also exits
		if (controllers[0].wButtons & XINPUT_GAMEPAD_BACK)
			gameover = true;
	}

	// begin rendering
	if (d3ddev->BeginScene())
	{
		floorMesh->drawModel(camObj);
		brickMesh->drawModel(camObj);
		
		QueryPerformanceCounter(&nowTime);
		double dTime = ((nowTime.QuadPart - startTime) / ticks);

		for (int i = 0; i < allUnits.size(); i++)
		{
			allUnits[i]->moveUnit(allUnits[i]->endPosition);
			allUnits[i]->Update(dTime);
			allUnits[i]->Draw(camObj);
		}

		startTime = (double)nowTime.QuadPart;

		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);

		Draw_HUD();

		spriteobj->End();
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}
}

void Game_End()
{
	// free memory and shutdown
	DeleteModel(floorMesh);
	debugText->Release();

	DirectSound_Shutdown();
	DirectInput_Shutdown();
	Direct3D_Shutdown();
}