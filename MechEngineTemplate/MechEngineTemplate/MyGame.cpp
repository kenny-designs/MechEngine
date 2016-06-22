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
const int SCREENW = 768; // switch to 1366 when going fullscreen
const int SCREENH = 512;
const float NEAR_LENGTH = 1.0f; 
const float FAR_LENGTH = 100.0f; 
const float FOV = D3DX_PI / 4.0f;
float SCREEN_ASPECT;
const bool FULLSCREEN = false;
D3DVIEWPORT9 m_mainViewport;
CAMERA camObj;
float m_screenWidth, m_screenHeight;
float screenAspect;
float scaleMod = 0.005f;
D3DXVECTOR3 rayIntersectPos;

// 3D meshes
MODEL *floorMesh;

// Units
UNIT munkeyUnit;
UNIT coneUnit;
UNIT mercyUnit;

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
DWORD currenttime;

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
	FontPrint(debugText, 10, 50, "FloorMesh translate at (" +
		ToString(floorMesh->translate.x) + ", " +
		ToString(floorMesh->translate.y) + ", " +
		ToString(floorMesh->translate.z) + ")");
	FontPrint(debugText, 10, 70, "FloorMesh rotation at (" +
		ToString(floorMesh->rotate.x) + ", " +
		ToString(floorMesh->rotate.y) + ", " +
		ToString(floorMesh->rotate.z) + ")");
	FontPrint(debugText, 10, 90, "FloorMesh scale at (" +
		ToString(floorMesh->scale.x) + ", " +
		ToString(floorMesh->scale.y) + ", " +
		ToString(floorMesh->scale.z) + ")");
	FontPrint(debugText, 10, 110, "The current selected unit is: " +
		currentSU);
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

	floorMesh = LoadModel("floor.x");
	if (floorMesh == NULL)
	{
		MessageBox(window, "Error loading floorMesh!", APPTITLE.c_str(), MB_OK);
		return 0;
	}
	 // load units
	munkeyUnit.setUnit("suzanneTextured.x", "Suzanne", 0.05f);
	coneUnit.setUnit("cone.x", "Cone", 0.05f);
	mercyUnit.setUnit("Mercy.x", "Mercy", 0.05f);

	allUnits.push_back(&munkeyUnit);
	allUnits.push_back(&coneUnit);
	allUnits.push_back(&mercyUnit);

	return true;
}

void Game_Run(HWND window)
{
	// make sure the Direct3D device is valid
	if (!d3ddev) return;

	// clear the scene
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0, 0, 100), 1.0f, 0);

	// get current ticks
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
	
	// convert screen coords to client coords
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
			allUnits[selectID]->endPosition = rayIntersectPos;
		}
	}

	// Moves screen depending on mouse position
	D3DXVECTOR3 camOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	if (cursPt.y <= 0)
		camOffset.z += 0.15f;
	if (cursPt.y >= m_screenHeight)
		camOffset.z -= 0.15f;
	if (cursPt.x >= m_screenWidth)
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
		// set the plane
		floorMesh->translate.x = 3.0f;
		floorMesh->translate.y = -1.0f;
		floorMesh->translate.z = 2.0f;

		// floorMesh->rotate.x += 1.0f;

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
		// monkeyFella->drawModel(camObj);
		floorMesh->drawModel(camObj);
		
		for (int i = 0; i < allUnits.size(); i++)
		{
			allUnits[i]->moveUnit(allUnits[i]->endPosition);
			allUnits[i]->drawModel(camObj);
		}
		

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