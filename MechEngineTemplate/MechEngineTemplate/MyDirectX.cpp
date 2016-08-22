#ifndef MY_DIRECT_X_H
#define MY_DIRECT_X_H
#include "MyDirectX.h"
#endif

#ifndef MECH_H
#define MECH_H
#include "Mech.h"
#endif


// Direct3D variables
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;
LPDIRECT3DSURFACE9 backbuffer = NULL;

LPD3DXSPRITE spriteobj = NULL;

// DirectInput variables
LPDIRECTINPUT8 dinput = NULL;
LPDIRECTINPUTDEVICE8 dimouse = NULL;
LPDIRECTINPUTDEVICE8 dikeyboard = NULL;
DIMOUSESTATE mouse_state;
char keys[256];
XINPUT_GAMEPAD controllers[4];

// primary DirectSound Object
CSoundManager *dsound = NULL;

// Direct3D initialization
bool Direct3D_Init(HWND window, int width, int height, bool fullscreen)
{
	// initialize Direct3D
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d) return false;
	// set Direct3D presentation parameters
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = (!fullscreen);
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // changed from D3DSWAPEFFECT_COPY for sampling
	d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;

	// account for fullscreen
	if (fullscreen)
	{
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);
		d3dpp.BackBufferWidth = desktop.right;
		d3dpp.BackBufferHeight = desktop.bottom;
	}
	else
	{
		d3dpp.BackBufferWidth = width;
		d3dpp.BackBufferHeight = height;
	}

	d3dpp.hDeviceWindow = window;
	// Allows for zbuffering
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// create Direct3D device
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	if (!d3ddev) return false;

	// get a pointer to the back buffer surface
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	// create sprite object
	D3DXCreateSprite(d3ddev, &spriteobj);

	return true;
}

// Direct3D shutdown
void Direct3D_Shutdown()
{
	if (spriteobj) spriteobj->Release();
	if (d3ddev) d3ddev->Release();
	if (d3d) d3d->Release();
}

// Draws a surface to the screen using StretchRect
void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source)
{
	// get width/height from source surface
	D3DSURFACE_DESC desc;
	source->GetDesc(&desc);

	// create rects for drawing
	RECT source_rect = { 0, 0,
		(long)desc.Width, (long)desc.Height };
	RECT dest_rect = { (long)x, (long)y,
		(long)x + desc.Width, (long)y + desc.Height };

	// draw the source surface onto the dest
	d3ddev->StretchRect(source, &source_rect, dest,
		&dest_rect, D3DTEXF_NONE);
}

// Loads a bitmap file into a surface
LPDIRECT3DSURFACE9 LoadSurface(std::string filename)
{
	LPDIRECT3DSURFACE9 image = NULL;

	// get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result != D3D_OK) return NULL;

	// create surface
	result = d3ddev->CreateOffscreenPlainSurface(
		info.Width,				// width of the surface
		info.Height,			// height of the surface
		D3DFMT_X8R8G8B8,		// surface format
		D3DPOOL_DEFAULT,		// memory pool to use
		&image,					// pointer to the surface
		NULL);					// reserved (always NULL)

	if (result != D3D_OK) return NULL;

	// load surface from file into newly created surface
	result = D3DXLoadSurfaceFromFile(
		image,					// destination surface
		NULL,					// destination palette
		NULL,					// destination rectangle
		filename.c_str(),		// sourface filename
		NULL,					// source rectangle
		D3DX_DEFAULT,			// controls how image is filtered
		D3DCOLOR_XRGB(0, 0, 0), // for transparency (0 for none)
		NULL);					// source image info (usually NULL)

								// make sure file was loaded okay
	if (result != D3D_OK) return NULL;

	return image;
}

// Loads a texture
LPDIRECT3DTEXTURE9 LoadTexture(std::string filename, D3DCOLOR transcolor)
{
	LPDIRECT3DTEXTURE9 texture = NULL;

	// get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result != D3D_OK) return NULL;

	// create the new texture by loading a bitmap image file
	D3DXCreateTextureFromFileEx(
		d3ddev,				// Direct3D device object
		filename.c_str(),	// bitmap filename
		info.Width,			// bitmap image width
		info.Height,		// bitmap image height
		1,					// mip-map levels(1 for no chain)
		D3DPOOL_DEFAULT,	// the type of surface (standard)
		D3DFMT_UNKNOWN,		// surface format (default)
		D3DPOOL_DEFAULT,	// memory class for the texture
		D3DX_DEFAULT,		// image filter
		D3DX_DEFAULT,		// mip filter
		transcolor,			// color key for transparency
		&info,				// bitmap file info (from loaded file)
		NULL,				// color palette
		&texture);			// destination texture

							// make sure the bitmap texture was loaded correctly
	if (result != D3D_OK) return NULL;

	return texture;
}

// Gets the size of a bitmap
D3DXVECTOR2 GetBitmapSize(std::string filename)
{
	D3DXIMAGE_INFO info;
	D3DXVECTOR2 size = D3DXVECTOR2(0.0f, 0.0f);
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result == D3D_OK)
		size = D3DXVECTOR2((float)info.Width, (float)info.Height);
	else
		size = D3DXVECTOR2((float)info.Width, (float)info.Height);

	return size;
}

// Drawing and animating sprites
void Sprite_Draw_Frame(LPDIRECT3DTEXTURE9 texture, int destx, int desty,
	int framew, int frameh, int framenum, int columns)
{
	D3DXVECTOR3 position((float)destx, (float)desty, 0);
	D3DCOLOR white = D3DCOLOR_XRGB(255, 255, 255);

	RECT rect;
	rect.left = (framenum % columns) * framew;
	rect.top = (framenum / columns) * frameh;
	rect.right = rect.left + framew;
	rect.bottom = rect.top + frameh;

	spriteobj->Draw(texture, &rect, NULL, &position, white);
}

void Sprite_Animate(int &frame, int startframe, int endframe,
	int direction, int &starttime, int delay)
{
	if ((int)GetTickCount() > starttime + delay)
	{
		starttime = GetTickCount();

		frame += direction;
		if (frame > endframe) frame = startframe;
		if (frame < startframe) frame = endframe;
	}
}

void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y,
	int width, int height, int frame, int columns,
	float rotation, float scaleW, float scaleH, D3DCOLOR color)
{
	// create a scale vecctor
	D3DXVECTOR2 scale(scaleW, scaleH);

	// create a translate vector
	D3DXVECTOR2 trans((float)x, (float)y);

	// set center by dividing width and height by two
	D3DXVECTOR2 center((float)(width * scaleW) / 2, (float)(height * scaleH) / 2);

	// create 2D transformation matrix
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat, NULL, 0, &scale, &center, rotation, &trans);

	// tell sprite object to use the transform
	spriteobj->SetTransform(&mat);

	// calculate frame location in source image
	int fx = (frame % columns) * width;
	int fy = (frame / columns) * height;
	RECT srcRect = { fx, fy, fx + width, fy + height };

	// draw the sprite frame
	spriteobj->Draw(image, &srcRect, NULL, NULL, color);

	// added in chapter 14
	D3DXMatrixIdentity(&mat);
	spriteobj->SetTransform(&mat);
}

void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y,
	int width, int height, int frame, int columns,
	float rotation, float scaling, D3DCOLOR color)
{
	Sprite_Transform_Draw(image, x, y, width, height, frame, columns, rotation, scaling, scaling, color);
}

// DirectInput initialization
bool DirectInput_Init(HWND hwnd)
{
	// initialize DirectInput object
	HRESULT result = DirectInput8Create(
		GetModuleHandle(NULL),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&dinput,
		NULL);

	// initialize the keyboard
	dinput->CreateDevice(GUID_SysKeyboard, &dikeyboard, NULL);
	dikeyboard->SetDataFormat(&c_dfDIKeyboard);
	dikeyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dikeyboard->Acquire();

	// initialize the mouse
	dinput->CreateDevice(GUID_SysMouse, &dimouse, NULL);
	dimouse->SetDataFormat(&c_dfDIMouse);
	dimouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dimouse->Acquire();
	d3ddev->ShowCursor(false);

	return true;
}

// DirectInput update
void DirectInput_Update()
{
	// update mouse
	dimouse->GetDeviceState(sizeof(mouse_state), (LPVOID)&mouse_state);

	// update keyboard
	dikeyboard->GetDeviceState(sizeof(keys), (LPVOID)&keys);

	// update controllers
	for (int i = 0; i < 4; i++)
	{
		ZeroMemory(&controllers[i], sizeof(XINPUT_STATE));

		// get the state of the controller
		XINPUT_STATE state;
		DWORD result = XInputGetState(i, &state);

		// store state in global controllers array
		if (result == 0) controllers[i] = state.Gamepad;
	}
}

// Return mouse x movement
int Mouse_X()
{
	return mouse_state.lX;
}

// Return mouse y movement
int Mouse_Y()
{
	return mouse_state.lY;
}

// Return mouse button state
int Mouse_Button(int button)
{
	return mouse_state.rgbButtons[button] & 0x80;
}

// Return key press state
int Key_Down(int key)
{
	return (keys[key] & 0x80);
}

// DirectInput shutdown
void DirectInput_Shutdown()
{
	if (dikeyboard)
	{
		dikeyboard->Unacquire();
		dikeyboard->Release();
		dikeyboard = NULL;
	}
	if (dimouse)
	{
		dimouse->Unacquire();
		dimouse->Release();
		dimouse = NULL;
	}
}

// Returns true if controller is plugged in
bool XInput_Controller_Found()
{
	XINPUT_CAPABILITIES caps;
	ZeroMemory(&caps, sizeof(XINPUT_CAPABILITIES));
	XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &caps);
	if (caps.Type != 0) return false;

	return true;
}

// Vibrates the controller
void XInput_Vibrate(int contNum, int amount)
{
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = amount;
	vibration.wRightMotorSpeed = amount;
	XInputSetState(contNum, &vibration);
}

// bounding box collision detection
int Collision(SPRITE sprite1, SPRITE sprite2)
{
	RECT rect1;
	rect1.left = (long)sprite1.x;
	rect1.top = (long)sprite1.y;
	rect1.right = (long)sprite1.x + sprite1.width * sprite1.scaling;
	rect1.bottom = (long)sprite1.y + sprite1.height * sprite1.scaling;

	RECT rect2;
	rect2.left = (long)sprite2.x;
	rect2.top = (long)sprite2.y;
	rect2.right = (long)sprite2.x + sprite2.width * sprite2.scaling;
	rect2.bottom = (long)sprite2.y + sprite2.height * sprite2.scaling;

	RECT dest; // ignored
	return IntersectRect(&dest, &rect1, &rect2);
}

// radial collision detection
bool CollisionD(SPRITE sprite1, SPRITE sprite2)
{
	double radius1, radius2;

	// calculate radius 1
	if (sprite1.width > sprite1.height)
		radius1 = (sprite1.width * sprite1.scaling) / 2.0;
	else
		radius1 = (sprite1.height * sprite1.scaling) / 2.0;

	// center point 1
	double x1 = sprite1.x + radius1;
	double y1 = sprite1.y + radius1;
	D3DXVECTOR2 vector1(x1, y1);

	// calculate radius 2
	if (sprite2.width > sprite2.height)
		radius2 = (sprite2.width * sprite2.scaling) / 2.0;
	else
		radius2 = (sprite2.height * sprite2.scaling) / 2.0;

	// center point 2
	double x2 = sprite2.x + radius2;
	double y2 = sprite2.y + radius2;
	D3DXVECTOR2 vector2(x2, y2);

	// calculate distance
	double deltax = vector2.x - vector1.x;
	double deltay = vector2.y - vector1.y;
	double dist = sqrt((deltax * deltax) + (deltay * deltay));

	// return distance comparison
	return (dist < radius1 + radius2);
}

// font functions
LPD3DXFONT MakeFont(std::string name, int size)
{
	LPD3DXFONT font = NULL;
	D3DXFONT_DESC desc = {
		size,					// height
		0,						// width
		0,						// weight
		0,						// miplevels
		false,					// italic
		DEFAULT_CHARSET,		// charset
		OUT_TT_PRECIS,			// output precision
		CLIP_DEFAULT_PRECIS,	// quality
		DEFAULT_PITCH,			// pitch and family
		"" };					// font name
	strcpy_s(desc.FaceName, name.c_str());
	D3DXCreateFontIndirect(d3ddev, &desc, &font);
	return font;
}

void FontPrint(LPD3DXFONT font, int x, int y, std::string text, D3DCOLOR color)
{
	// figure out the text boundary
	RECT rect = { x, y, 0, 0 };
	font->DrawText(NULL, text.c_str(), text.length(), &rect, DT_CALCRECT, color);

	// print the text
	font->DrawText(spriteobj, text.c_str(), text.length(), &rect, DT_LEFT, color);
}

// DirectSound functions
bool DirectSound_Init(HWND hwnd)
{
	// create DirectSound manager object
	dsound = new CSoundManager();

	// initialize DirectSound
	HRESULT result;
	result = dsound->Initialize(hwnd, DSSCL_PRIORITY);
	if (result != DS_OK) return false;

	// set the primary buffer format
	result = dsound->SetPrimaryBufferFormat(2, 22050, 16);
	if (result != DS_OK) return false;

	// return success
	return true;
}

void DirectSound_Shutdown()
{
	if (dsound) delete dsound;
}

CSound *LoadSound(std::string filename)
{
	HRESULT result;

	// create local reference to wave data
	CSound *wave = NULL;

	// attempt to load wave file
	char s[255];
	sprintf_s(s, "%s", filename.c_str());
	result = dsound->Create(&wave, s);
	if (result != DS_OK) wave = NULL;

	// return the wave
	return wave;
}

void PlaySound(CSound *sound)
{
	sound->Play();
}

void LoopSound(CSound *sound)
{
	sound->Play(0, DSBPLAY_LOOPING);
}

void StopSound(CSound *sound)
{
	sound->Stop();
}

// 3D mesh functions
void CAMERA::SetCamera(float posx, float posy, float posz,
	float lookx, float looky, float lookz)
{
	fov = FOV;
	aspectRatio = SCREEN_ASPECT;
	nearRange = NEAR_LENGTH;
	farRange = FAR_LENGTH;
	updir = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	position = D3DXVECTOR3(posx, posy, posz);
	target = D3DXVECTOR3(lookx, looky, lookz);

	// set perspective
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fov, aspectRatio,
		nearRange, farRange);
	d3ddev->SetTransform(D3DTS_PROJECTION, &m_projectionMatrix);

	// set up the camera view matrix
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &target, &updir);
	d3ddev->SetTransform(D3DTS_VIEW, &m_viewMatrix);

	// set the world matrix
	D3DXMatrixIdentity(&m_worldMatrix);
	d3ddev->SetTransform(D3DTS_WORLD, &m_worldMatrix);
}

void CAMERA::translateCam(D3DXVECTOR3 camOffset)
{
	position += camOffset;
	target += camOffset;

	// set up the camera view matrix
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &target, &updir);
	d3ddev->SetTransform(D3DTS_VIEW, &m_viewMatrix);

	// set the world matrix
	D3DXMatrixIdentity(&m_worldMatrix);
	d3ddev->SetTransform(D3DTS_WORLD, &m_worldMatrix);
}

void SplitPath(const std::string &inputPath, std::string *pathOnly, std::string *filenameOnly)
{
	std::string fullPath = inputPath;
	replace(fullPath.begin(), fullPath.end(), '\\', '/');
	std::string::size_type lastSlashPos = fullPath.find_last_of('/');

	// check for there being no path element in the input
	if (lastSlashPos == std::string::npos)
	{
		*pathOnly = "";
		*filenameOnly = fullPath;
	}
	else
	{
		if (pathOnly)
		{
			*pathOnly = fullPath.substr(
				lastSlashPos + 1,
				fullPath.size() - lastSlashPos - 1);
		}
	}
}

bool DoesFileExist(const std::string &filename)
{
	return (_access(filename.c_str(), 0) != -1);
}

bool FindFile(std::string *filename)
{
	if (!filename) return false;

	// look for file using original filename and path
	if (DoesFileExist(*filename)) return true;

	// since the file was not found, try removing the path
	std::string pathOnly;
	std::string filenameOnly;
	SplitPath(*filename, &pathOnly, &filenameOnly);

	// is file found in current folder, without the path?
	if (DoesFileExist(filenameOnly))
	{
		*filename = filenameOnly;
		return true;
	}

	// not found
	return false;
}

HRESULT CAllocateHierarchy::CreateFrame(LPCTSTR Name, LPD3DXFRAME *ppNewFrame)
{
	// Create a frame
	// Using derived struct here
	LPFRAME pFrame = new FRAME;
	ZeroMemory(pFrame, sizeof(FRAME));

	// Inicilize the passed in frame
	*ppNewFrame = NULL;

	// Put the name in the frame
	if (Name)
	{
		int nNameSize = strlen(Name) + 1;
		pFrame->Name = new char[nNameSize];
		memcpy(pFrame->Name, Name, nNameSize * sizeof(char));
	}
	else
		pFrame->Name = NULL;

	// Inicilize the rest of the frame
	pFrame->pFrameFirstChild = NULL;
	pFrame->pFrameSibling = NULL;
	pFrame->pMeshContainer = NULL;
	D3DXMatrixIdentity(&pFrame->matCombined);
	D3DXMatrixIdentity(&pFrame->TransformationMatrix);

	// Set the output frame to the one that we have
	*ppNewFrame = (LPD3DXFRAME)pFrame;

	// It no longer points to the frame
	pFrame = NULL;

	// returns an HRESULT so give it the AOk result
	return S_OK;
}

HRESULT CAllocateHierarchy::CreateMeshContainer(LPCTSTR Name, const D3DXMESHDATA *pMeshData,
	const D3DXMATERIAL *pMaterials, const D3DXEFFECTINSTANCE *pEffectinstances,
	DWORD NumMaterials, const DWORD *pAdjacency,
	LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER *ppNewMeshContainer)
{
	// Create a temp mesh container using derived struct
	LPMESHCONTAINER pMeshContainer = new MESHCONTAINER;
	ZeroMemory(pMeshContainer, sizeof(MESHCONTAINER));

	// Incilialize pass in container
	*ppNewMeshContainer = NULL;

	if (Name)
	{
		// put in the name
		int nNameSize = strlen(Name) + 1;
		pMeshContainer->Name = new char[nNameSize];
		memcpy(pMeshContainer->Name, Name, nNameSize * sizeof(char));
	}
	else
		pMeshContainer->Name = NULL;

	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	// get the number of faces for adjacency
	DWORD dwFaces = pMeshData->pMesh->GetNumFaces();

	// get initcilize all the other data
	pMeshContainer->NumMaterials = NumMaterials;

	// create the arrays for the materials and the textures
	pMeshContainer->pMaterials9 = new D3DMATERIAL9[pMeshContainer->NumMaterials];

	// multiply by 3 because there are three adjacent triangles
	pMeshContainer->pAdjacency = new DWORD[dwFaces * 3];
	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * dwFaces * 3);

	// get the device to use
	LPDIRECT3DDEVICE9 pd3dDevice = NULL; // Direct3D rendering device
	pMeshData->pMesh->GetDevice(&pd3dDevice);

	pMeshData->pMesh->CloneMeshFVF(D3DXMESH_MANAGED,
		pMeshData->pMesh->GetFVF(), pd3dDevice,
		&pMeshContainer->MeshData.pMesh);

	pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[NumMaterials];
	for (DWORD dw = 0; dw < NumMaterials; ++dw)
	{
		pMeshContainer->ppTextures[dw] = NULL;

		if (pMaterials[dw].pTextureFilename && strlen(pMaterials[dw].pTextureFilename) > 0)
		{
			if (FAILED(D3DXCreateTextureFromFile(pd3dDevice,
				pMaterials[dw].pTextureFilename, &pMeshContainer->ppTextures[dw])))
				pMeshContainer->ppTextures[dw] = NULL;
		}
	}

	// release the device
	// SAFE_RELEASE(pd3dDevice);

	if (pd3dDevice != NULL) // this is what SAFE_RELEASE actually does
	{
		pd3dDevice->Release();
		pd3dDevice = NULL;
	}

	if (pSkinInfo)
	{
		// first save off the SkinInfo and original mesh data
		pMeshContainer->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();

		// Will need an array of offset matrices to move the vertices from
		// the figure space to the bone's space
		UINT uBones = pSkinInfo->GetNumBones();
		pMeshContainer->pBoneOffsets = new D3DXMATRIX[uBones];

		// create the arrays for the bones and the frame matrices
		pMeshContainer->ppFrameMatrices = new D3DXMATRIX*[uBones];

		// get each of the bone offset matrices so that we don't need to
		// get them later
		for (UINT i = 0; i < uBones; i++)
			pMeshContainer->pBoneOffsets[i] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i));
	}
	else
	{
		pMeshContainer->pSkinInfo = NULL;
		pMeshContainer->pBoneOffsets = NULL;
		pMeshContainer->pSkinMesh = NULL;
		pMeshContainer->ppFrameMatrices = NULL;
	}

	pMeshContainer->pMaterials = NULL;
	pMeshContainer->pEffects = NULL;

	// set the output mesh container to the temp one
	*ppNewMeshContainer = pMeshContainer;
	pMeshContainer = NULL;

	// returns an HRESULT so give it the AOk result
	return S_OK;
}

HRESULT CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	// conver the frame
	LPFRAME pFrame = (LPFRAME)pFrameToFree;

	// delete the name
	if (pFrame->Name)
	{
		delete[] pFrame->Name;
		pFrame->Name = NULL;
	}

	// delete the frame
	if (pFrame)
	{
		delete pFrame;
		pFrame = NULL;
	}

	// returns an HRESULT so give it the AOk result
	return S_OK;
}

HRESULT CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	// convert to my derived struct type
	LPMESHCONTAINER pMeshContainer = (LPMESHCONTAINER)pMeshContainerBase;

	// if there is a name
	if (pMeshContainer->Name)
	{
		delete[] pMeshContainer->Name;
		pMeshContainer->Name = NULL;
	}

	// if there are materials
	if (pMeshContainer->pMaterials9)
	{
		delete[] pMeshContainer->pMaterials9;
		pMeshContainer->pMaterials9 = NULL;
	}

	// release the textures
	if (pMeshContainer->ppTextures)
	{
		for (UINT i = 0; i < pMeshContainer->NumMaterials; ++i)
		{
			pMeshContainer->ppTextures[i]->Release();
			pMeshContainer->ppTextures[i] = NULL;
		}
	}

	// if there is a adjacency data
	if (pMeshContainer->pAdjacency)
	{
		delete[] pMeshContainer->pAdjacency;
		pMeshContainer->pAdjacency = NULL;
	}

	// if there are bone parts
	if (pMeshContainer->pBoneOffsets)
	{
		delete[] pMeshContainer->pBoneOffsets;
		pMeshContainer->pBoneOffsets = NULL;
	}

	// if there are frame matrices
	if (pMeshContainer->ppFrameMatrices)
	{
		delete[] pMeshContainer->ppFrameMatrices;
		pMeshContainer->ppFrameMatrices = NULL;
	}

	if (pMeshContainer->pAttributeTable)
	{
		delete[] pMeshContainer->pAttributeTable;
		pMeshContainer->pAttributeTable = NULL;
	}

	// if there is a copy of the mesh here
	if (pMeshContainer->pSkinMesh)
	{
		pMeshContainer->pSkinMesh->Release();
		pMeshContainer->pSkinMesh = NULL;
	}

	// if there is a mesh
	if (pMeshContainer->MeshData.pMesh)
	{
		pMeshContainer->MeshData.pMesh->Release();
		pMeshContainer->MeshData.pMesh = NULL;
	}

	// if there is skin information
	if (pMeshContainer->pSkinInfo)
	{
		pMeshContainer->pSkinInfo->Release();
		pMeshContainer->pSkinInfo = NULL;
	}

	// delete the mesh container
	if (pMeshContainer)
	{
		delete pMeshContainer;
		pMeshContainer = NULL;
	}

	// return an HRESULT so give it the AOk result
	return S_OK;
}

void MODEL::setModel()
{
	material_count = 0;
	mesh = NULL;
	boundingBox = NULL;
	materials = NULL;
	textures = NULL;
	translate = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	rotate = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
}

MODEL *LoadModel(std::string filename)
{
	MODEL *model = (MODEL*)malloc(sizeof(MODEL));
	model->setModel();
	LPD3DXBUFFER matbuffer;
	HRESULT result;

	// load mesh from the specified file
	result = D3DXLoadMeshFromX(
		filename.c_str(),		// filename
		D3DXMESH_SYSTEMMEM,		// mesh options
		d3ddev,					// Direct3D device
		NULL,					// adjacency buffer
		&matbuffer,				// material buffer
		NULL,					// special effects
		&model->material_count, // number of materials
		&model->mesh);			// resulting mesh

	if (result != D3D_OK)
	{
		MessageBox(0, "Error loading model file", APPTITLE.c_str(), 0);
		return NULL;
	}
	// extract material properties and texture names from material buffer
	LPD3DXMATERIAL d3dxMaterials = (LPD3DXMATERIAL)matbuffer->GetBufferPointer();
	model->materials = new D3DMATERIAL9[model->material_count];
	model->textures = new LPDIRECT3DTEXTURE9[model->material_count];

	// create the materials and textures
	for (DWORD i = 0; i < model->material_count; i++)
	{
		// grab the material
		model->materials[i] = d3dxMaterials[i].MatD3D;

		// set ambient color for material
		model->materials[i].Ambient = model->materials[i].Diffuse;

		model->textures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename != NULL)
		{
			std::string filename = d3dxMaterials[i].pTextureFilename;
			if (FindFile(&filename))
			{
				result = D3DXCreateTextureFromFile(
					d3ddev, filename.c_str(), &model->textures[i]);
				if (result != D3D_OK)
				{
					MessageBox(0, "Could not find texture",
						APPTITLE.c_str(), 0);
					return false;
				}
			}
		}
	}
	// done using material buffer
	matbuffer->Release();

	return model;
}

void DeleteModel(MODEL *model)
{
	// remove materials from memory
	if (model->materials != NULL)
		delete[] model->materials;

	// remove textures from memory
	if (model->textures != NULL)
	{
		for (DWORD i = 0; i < model->material_count; i++)
		{
			if (model->textures[i] != NULL)
				model->textures[i]->Release();
		}
		delete[] model->textures;
	}

	// remove mesh from memory
	if (model->mesh != NULL)
		model->mesh->Release();

	// remove model struct from memory
	if (model != NULL)
		free(model);
}

// set transform and draw mesh
void MODEL::drawModel(CAMERA cam)
{
	D3DXMATRIX matWorld = cam.m_worldMatrix;
	D3DXMATRIX transMat, rotMat, scaleMat, tempMat;

	D3DXMatrixTranslation(&transMat, translate.x, translate.y, translate.z);

	// prevents degree rotation from going over 360
	// would like to find cleaner way if possible
	if (rotate.x > 360.0f)
		rotate.x -= 360.0f;
	else if (rotate.x < 0.0f)
		rotate.x += 360.0f;
	if (rotate.y > 360.0f)
		rotate.y -= 360.0f;
	else if (rotate.y < 0.0f)
		rotate.y += 360.0f;
	if (rotate.z > 360.0f)
		rotate.z -= 360.0f;
	else if (rotate.z < 0.0f)
		rotate.z += 360.0f;

	D3DXMatrixRotationYawPitchRoll(&rotMat, 
		(float)toRadians(rotate.x), 
		(float)toRadians(rotate.y), 
		(float)toRadians(rotate.z));

	D3DXMatrixScaling(&scaleMat, scale.x, scale.y, scale.z);

	D3DXMatrixMultiply(&tempMat, &scaleMat, &rotMat);
	D3DXMatrixMultiply(&matWorld, &tempMat, &transMat);

	d3ddev->SetTransform(D3DTS_WORLD, &matWorld);

	// any materials in this mesh?
	if (material_count == 0)
	{
		mesh->DrawSubset(0);
	}
	else
	{
		// draw each mesh subset
		for (DWORD i = 0; i < material_count; i++)
		{
			// Set the material and texture for this subset
			d3ddev->SetMaterial(&materials[i]);
			if (textures[i])
			{
				if (textures[i]->GetType() == D3DRTYPE_TEXTURE)
				{
					D3DSURFACE_DESC desc;
					textures[i]->GetLevelDesc(0, &desc);
					if (desc.Width > 0)
						d3ddev->SetTexture(0, textures[i]);
				}
			}

			// Draw the mesh subset
			mesh->DrawSubset(i);
		}
	}
}

// load the model
void MODEL::loadModel(std::string filename)
{
	setModel();
	LPD3DXBUFFER matbuffer;
	HRESULT result;

	// load mesh from the specified file
	result = D3DXLoadMeshFromX(
		filename.c_str(),		// filename
		D3DXMESH_SYSTEMMEM,		// mesh options
		d3ddev,					// Direct3D device
		NULL,					// adjacency buffer
		&matbuffer,				// material buffer
		NULL,					// special effects
		&material_count,		// number of materials
		&mesh);					// resulting mesh

	if (result != D3D_OK)
	{
		MessageBox(0, "Error loading model file", APPTITLE.c_str(), 0);
	}
	// extract material properties and texture names from material buffer
	LPD3DXMATERIAL d3dxMaterials = (LPD3DXMATERIAL)matbuffer->GetBufferPointer();
	materials = new D3DMATERIAL9[material_count];
	textures = new LPDIRECT3DTEXTURE9[material_count];

	// create the materials and textures
	for (DWORD i = 0; i < material_count; i++)
	{
		// grab the material
		materials[i] = d3dxMaterials[i].MatD3D;

		// set ambient color for material
		materials[i].Ambient = materials[i].Diffuse;

		textures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename != NULL)
		{
			std::string filename = d3dxMaterials[i].pTextureFilename;
			if (FindFile(&filename))
			{
				result = D3DXCreateTextureFromFile(
					d3ddev, filename.c_str(), &textures[i]);
				if (result != D3D_OK)
				{
					MessageBox(0, "Could not find texture",
						APPTITLE.c_str(), 0);
					break;
				}
			}
		}
	}
	// done using material buffer
	matbuffer->Release();
}

MODEL::MODEL()
{
	m_pFrameRoot = NULL;
	m_pBoneMatrices = NULL;
	m_vecCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fRadius = 0.0f;
	m_dwCurrentAnimation = -1;
	m_dwAnimationSetCount = 0;
	m_uMaxBones = 0;
	m_pAnimController = NULL;
	m_pFirstMesh = NULL;
}

MODEL::~MODEL()
{
	// delete animation controller
	if (m_pAnimController)
	{
		m_pAnimController->Release();
		m_pAnimController = NULL;
	}

	// if there is a frame hierarchy
	if (m_pFrameRoot)
	{
		// allocation class
		CAllocateHierarchy Alloc;
		D3DXFrameDestroy(m_pFrameRoot, &Alloc);
		m_pFrameRoot = NULL;
	}

	// delete bones
	if (m_pBoneMatrices)
	{
		delete[] m_pBoneMatrices;
		m_pBoneMatrices = NULL;
	}
}

void MODEL::LoadXFile(std::string strFileName)
{
	// allocation class
	CAllocateHierarchy Alloc;

	HRESULT meshResult;

	// load mesh
	if (FAILED(meshResult = D3DXLoadMeshHierarchyFromX(strFileName.c_str(), // file load
		D3DXMESH_MANAGED,		// load options
		d3ddev,					// d3d device
		&Alloc,					// hierarchy allocation class
		NULL,					// no effects
		&m_pFrameRoot,			// frame hierarchy
		&m_pAnimController)))	// animation controller
	{
		MessageBox(NULL, strFileName.c_str(), "Model load error", MB_OK);
		if (meshResult == E_OUTOFMEMORY)
			MessageBox(NULL, "Out of memory", "HRESULT error", MB_OK);
		if (meshResult == D3DERR_INVALIDCALL)
			MessageBox(NULL, "Invalid call", "HRESULT error", MB_OK);
	}

	if (m_pAnimController)
		m_dwAnimationSetCount = m_pAnimController->GetMaxNumAnimationSets();

	if (m_pFrameRoot)
	{
		// set the bones up
		SetupBoneMatrices((LPFRAME)m_pFrameRoot, NULL);

		// set up the bone matrices array
		m_pBoneMatrices = new D3DXMATRIX[m_uMaxBones];
		ZeroMemory(m_pBoneMatrices, sizeof(D3DXMATRIX) * m_uMaxBones);

		// calculate the bounding sphere
		D3DXFrameCalculateBoundingSphere(m_pFrameRoot,
			&m_vecCenter, &m_fRadius);
	}
}

void MODEL::SetupBoneMatrices(LPFRAME pFrame, LPD3DXMATRIX pParentMatrix)
{
	LPMESHCONTAINER pMesh = (LPMESHCONTAINER)pFrame->pMeshContainer;

	// set up the bones on the mesh
	if (pMesh)
	{
		if (!m_pFirstMesh)
			m_pFirstMesh = pMesh;

		// if there is a skinmesh, then setup the bone matrices
		if (pMesh->pSkinInfo)
		{
			// create a copy of the mesh
			pMesh->MeshData.pMesh->CloneMeshFVF(D3DXMESH_MANAGED,
				pMesh->MeshData.pMesh->GetFVF(), d3ddev,
				&pMesh->pSkinMesh);

			if (m_uMaxBones < pMesh->pSkinInfo->GetNumBones())
			{
				// get the number of bones
				m_uMaxBones = pMesh->pSkinInfo->GetNumBones();
			}

			LPFRAME pTempFrame = NULL;

			// for each bone
			for (UINT i = 0; i < pMesh->pSkinInfo->GetNumBones(); i++)
			{
				// find the frame
				pTempFrame = (LPFRAME)D3DXFrameFind(m_pFrameRoot,
					pMesh->pSkinInfo->GetBoneName(i));

				// set the bone part
				pMesh->ppFrameMatrices[i] = &pTempFrame->matCombined;
			}
		}
	}

	// check your sister
	if (pFrame->pFrameSibling)
		SetupBoneMatrices((LPFRAME)pFrame->pFrameSibling, pParentMatrix);

	// check your son
	if (pFrame->pFrameFirstChild)
		SetupBoneMatrices((LPFRAME)pFrame->pFrameFirstChild, &pFrame->matCombined);
}

void MODEL::SetCurrentAnimation(DWORD dwAnimationFlag)
{
	// if the animation is not one that we already using
	// and the passed in flag is not bigger than the number of animations
	if (dwAnimationFlag != m_dwCurrentAnimation && dwAnimationFlag < m_dwAnimationSetCount)
	{
		m_dwCurrentAnimation = dwAnimationFlag;
		LPD3DXANIMATIONSET AnimSet = NULL;
		m_pAnimController->GetAnimationSet(m_dwCurrentAnimation, &AnimSet);
		m_pAnimController->SetTrackAnimationSet(0, AnimSet);
		
		// release the LPD3DXANIMATIONSET
		if (AnimSet)
		{
			AnimSet->Release();
			AnimSet = NULL;
		}
	}
}

// draw funcitons for CModel related functions
void MODEL::Draw(CAMERA cam)
{
	D3DXMATRIX matWorld = cam.m_worldMatrix;
	D3DXMATRIX transMat, rotMat, scaleMat, tempMat;

	D3DXMatrixTranslation(&transMat, translate.x, translate.y, translate.z);

	// prevents degree rotation from going over 360
	// would like to find cleaner way if possible
	if (rotate.x > 360.0f)
		rotate.x -= 360.0f;
	else if (rotate.x < 0.0f)
		rotate.x += 360.0f;
	if (rotate.y > 360.0f)
		rotate.y -= 360.0f;
	else if (rotate.y < 0.0f)
		rotate.y += 360.0f;
	if (rotate.z > 360.0f)
		rotate.z -= 360.0f;
	else if (rotate.z < 0.0f)
		rotate.z += 360.0f;

	D3DXMatrixRotationYawPitchRoll(&rotMat,
		(float)toRadians(rotate.x),
		(float)toRadians(rotate.y),
		(float)toRadians(rotate.z));

	D3DXMatrixScaling(&scaleMat, scale.x, scale.y, scale.z);

	D3DXMatrixMultiply(&tempMat, &scaleMat, &rotMat);
	D3DXMatrixMultiply(&matWorld, &tempMat, &transMat);

	d3ddev->SetTransform(D3DTS_WORLD, &matWorld);

	LPMESHCONTAINER pMesh = m_pFirstMesh;

	// while there is a mesh try to draw it
	while (pMesh)
	{
		// select the mesh to draw
		LPD3DXMESH pDrawMesh = (pMesh->pSkinInfo) 
			? pMesh->pSkinMesh : pMesh->MeshData.pMesh;

		// draw each mesh subset with correct materials and texture
		for (DWORD i = 0; i < pMesh->NumMaterials; ++i)
		{
			d3ddev->SetMaterial(&pMesh->pMaterials9[i]);
			d3ddev->SetTexture(0, pMesh->ppTextures[i]);
			pDrawMesh->DrawSubset(i);
		}

		// go to the next one
		pMesh = (LPMESHCONTAINER)pMesh->pNextMeshContainer;
	}
}

void MODEL::DrawFrame(LPFRAME pFrame)
{
	LPMESHCONTAINER pMesh = (LPMESHCONTAINER)pFrame->pMeshContainer;

	// while there is a mesh try to draw it
	while (pMesh)
	{
		// select the mesh to draw
		LPD3DXMESH pDrawMesh = (pMesh->pSkinInfo)
			? pMesh->pSkinMesh : pMesh->MeshData.pMesh;

		// draw each mesh subset with correct materials and texture
		for (DWORD i = 0; i < pMesh->NumMaterials; ++i)
		{
			d3ddev->SetMaterial(&pMesh->pMaterials9[i]);
			d3ddev->SetTexture(0, pMesh->ppTextures[i]);
			pDrawMesh->DrawSubset(i);
		}

		// go to the next one 
		pMesh = (LPMESHCONTAINER)pMesh->pNextMeshContainer;
	}

	// check your sister
	if (pFrame->pFrameSibling)
		DrawFrame((LPFRAME)pFrame->pFrameSibling);

	// check your son
	if (pFrame->pFrameFirstChild)
		DrawFrame((LPFRAME)pFrame->pFrameFirstChild);
}

// update function from CModel
void MODEL::Update(double dElapsedTime)
{
	// temp callback handler for animation
	LPD3DXANIMATIONCALLBACKHANDLER animCallback = NULL;

	// set the time for animation
	if (m_pAnimController && m_dwCurrentAnimation != -1)
		m_pAnimController->AdvanceTime(dElapsedTime, animCallback);

	// update the frame hierarchy
	if (m_pFrameRoot)
	{
		UpdateFrameMatrices((LPFRAME)m_pFrameRoot, NULL);

		LPMESHCONTAINER pMesh = m_pFirstMesh;
		if (pMesh)
		{
			if (pMesh->pSkinInfo)
			{
				UINT Bones = pMesh->pSkinInfo->GetNumBones();
				for (UINT i = 0; i < Bones; ++i)
				{
					D3DXMatrixMultiply(
						&m_pBoneMatrices[i], // out
						&pMesh->pBoneOffsets[i],
						pMesh->ppFrameMatrices[i]
					);
				}

				// lock the meshes' vertex buffers
				void *SrcPtr, *DestPtr;
				pMesh->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&SrcPtr);
				pMesh->pSkinMesh->LockVertexBuffer(0, (void**)&DestPtr);

				// Update the skinned mesh using provided transformations
				pMesh->pSkinInfo->UpdateSkinnedMesh(m_pBoneMatrices, NULL, SrcPtr, DestPtr);

				// unlock the meshes vertex buffers
				pMesh->pSkinMesh->UnlockVertexBuffer();
				pMesh->MeshData.pMesh->UnlockVertexBuffer();
			}
		}
	}
}

void MODEL::UpdateFrameMatrices(LPFRAME pFrame, LPD3DXMATRIX pPartentMatrix)
{
	// parent check
	if (pPartentMatrix)
	{
		D3DXMatrixMultiply(&pFrame->matCombined,
			&pFrame->TransformationMatrix,
			pPartentMatrix);
	}
	else
		pFrame->matCombined = pFrame->TransformationMatrix;

	// do the kid too
	if (pFrame->pFrameSibling)
		UpdateFrameMatrices((LPFRAME)pFrame->pFrameSibling, pPartentMatrix);

	// make sure you get the first kid
	if (pFrame->pFrameFirstChild)
		UpdateFrameMatrices((LPFRAME)pFrame->pFrameFirstChild, &pFrame->matCombined);
}

// render state function
void SetRenderstate(LPDIRECT3DDEVICE9 d3ddev, bool ambient, bool zBuff)
{
	d3ddev->SetRenderState(D3DRS_LIGHTING, ambient);
	d3ddev->SetRenderState(D3DRS_ZENABLE, zBuff);
	d3ddev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, true);
}