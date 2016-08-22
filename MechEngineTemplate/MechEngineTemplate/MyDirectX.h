#pragma once

// header files
#define WIN32_EXTRA_LEAN
#define DIRECTINPUT_VERSION 0x0800
#define D3D_DEBUG_INFO
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <xinput.h>
#include <d3dx9anim.h>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <io.h>
#include <algorithm>
#include <vector>
#include "DirectSound.h"

// libraries
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "XInput.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxerr.lib")

// macro to detect key presses
#define KEY_DOWN(vk_code)((GetAsyncKeyState(vk_code) & 0x8000) ? 1:0)

// program values
extern const std::string APPTITLE;
extern const int SCREENW;
extern const int SCREENH;
extern const float NEAR_LENGTH; 
extern const float FAR_LENGTH;
extern const float FOV;
extern float SCREEN_ASPECT;
extern const bool FULLSCREEN;
extern bool gameover;

// timing variables
extern DWORD refresh;
extern DWORD screentime;
extern double screenfps;
extern double screencount;
extern DWORD coretime;
extern double corefps;
extern double corecount;
extern DWORD currenttime;

// primary DirectSound object
extern CSoundManager *dsound;

class CAMERA {
public:
	float fov, aspectRatio, nearRange, farRange;
	D3DXVECTOR3 updir, position, target;
	D3DXMATRIX m_projectionMatrix, m_viewMatrix, m_worldMatrix;

	void SetCamera(float posx, float posy, float posz,
		float lookx = 0.0f, float looky = 0.0f, float lookz = 0.0f);

	// moves the camera based on cursor position
	void translateCam(D3DXVECTOR3 camOffset);
};

// sprite structure
struct SPRITE
{
	float x, y;
	int frame, columns;
	int width, height;
	float scaling, rotation;
	int startframe, endframe;
	int starttime, delay;
	int direction;
	float velx, vely;
	D3DCOLOR color;

	SPRITE()
	{
		frame = 0;
		columns = 1;
		width = height = 0;
		scaling = 1.0f;
		rotation = 0.0f;
		startframe = endframe = 0;
		direction = 1;
		starttime = delay = 0;
		velx = vely = 0.0f;
		color = D3DCOLOR_XRGB(255, 255, 255);
	}
};

// altering D3DXFRAME to better suit animation
typedef struct _D3DXFRAME_DERIVED : public D3DXFRAME
{
	D3DXMATRIX matCombined; // Combined transformation matrix
}FRAME, *LPFRAME;

// also altering D3DXMESHCONTAINER
typedef struct _D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER
{
	// Mesh variables
	LPDIRECT3DTEXTURE9 *ppTextures;			// textures of the mesh
	D3DMATERIAL9 *pMaterials9;				// Use the DirectX 9 material type

	// skinned mesh variables
	LPD3DXMESH pSkinMesh;					// the skin mesh
	LPD3DXMATRIX pBoneOffsets;				// The bone matrix offsets
	LPD3DXMATRIX *ppFrameMatrices;			// pointer to the frame matrix

	// attribute table stuff
	LPD3DXATTRIBUTERANGE pAttributeTable;	// the attriute table
	DWORD NumAttributeGroups;				// the number of attribute groups
}MESHCONTAINER, *LPMESHCONTAINER;

// altering LPD3DXALLOCATEHIERARCHY
class CAllocateHierarchy : public ID3DXAllocateHierarchy
{
public:
	// Create a frame
	STDMETHOD(CreateFrame)(THIS_ LPCTSTR Name,  // name of frame
		LPD3DXFRAME *ppNewFrame);				// output new frame

	// Create a Mesh Container
	STDMETHOD(CreateMeshContainer)(THIS_ LPCTSTR Name,  // name of mesh
		const D3DXMESHDATA *pMeshData,					// mesh data
		const D3DXMATERIAL *pMaterials,					// materials of the mesh
		const D3DXEFFECTINSTANCE *pEffectInstances,		// effects on the mesh
		DWORD NumMaterials,								// number of materials in the mesh
		const DWORD *pAdjacency,						// adjacency array for the mesh
		LPD3DXSKININFO pSkinInfo,						// skin information for the mesh
		LPD3DXMESHCONTAINER *ppNewMeshContainer);		// output mesh container

	// Destroy a frame
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree); // frame to delete

	// Destroy a mesh container
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase); // container to destroy
};

// define the MODEL class
class MODEL {
protected:
	// Model
	LPMESHCONTAINER m_pFirstMesh;			// First mesh in hierarchy
	LPD3DXFRAME		m_pFrameRoot;			// Frame hierarchy of the model
	LPD3DXMATRIX	m_pBoneMatrices;		// Used when calculating bone position
	D3DXVECTOR3		m_vecCenter;			// Center of bounding sphere of object
	float			m_fRadius;				// Radius of bounding sphere of object
	UINT			m_uMaxBones;			// Max number of bones for the model
	// Animation
	DWORD			m_dwCurrentAnimation;	// current animation
	DWORD			m_dwAnimationSetCount;	// number of animation sets
	LPD3DXANIMATIONCONTROLLER	m_pAnimController; // Controller for the animations

	void DrawFrame(LPFRAME pFrame);
	void SetupBoneMatrices(LPFRAME pFrame, LPD3DXMATRIX pParentMatrix);
	void UpdateFrameMatrices(LPFRAME pFrame, LPD3DXMATRIX pParentMatrix);

public:
	LPD3DXMESH mesh;
	LPD3DXMESH boundingBox; // stores bounding box info for collisions
	D3DMATERIAL9 *materials;
	LPDIRECT3DTEXTURE9 *textures;
	DWORD material_count;
	D3DXVECTOR3 translate;
	D3DXVECTOR3 rotate;
	D3DXVECTOR3 scale;

	void setModel();
	void drawModel(CAMERA cam);
	void loadModel(std::string filename);

	// added for CModel
	// construction and destruction
	MODEL();
	virtual ~MODEL();
	// MODEL &operator=(const MODEL&) {}
	// MODEL(const MODEL&){}

	// inline functions
	inline LPD3DXVECTOR3 GetBoundingSphereCenter()
	{ return &m_vecCenter; }

	inline float GetBoundingSphereRadius()
	{ return m_fRadius; }

	inline DWORD GetCurrentAnimation()
	{ return m_dwCurrentAnimation; }

	inline DWORD GetMaxAnimations()
	{ return m_dwAnimationSetCount; }

	// public functions
	void SetCurrentAnimation(DWORD dwAnimationFlag);
	void Draw(CAMERA cam);
	void LoadXFile(std::string strFileName);
	void Update(double dElapsedTime);
};

// Direct3D objects
extern LPDIRECT3D9 d3d;
extern LPDIRECT3DDEVICE9 d3ddev;
extern LPDIRECT3DSURFACE9 backbuffer;

extern LPD3DXSPRITE spriteobj;

// Direct3D functions
bool Direct3D_Init(HWND hwnd, int width, int height, bool fullscreen);
void Direct3D_Shutdown();
LPDIRECT3DSURFACE9 LoadSurface(std::string filename);
void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source);

D3DXVECTOR2 GetBitmapSize(std::string filename);

LPDIRECT3DTEXTURE9 LoadTexture(std::string filename, D3DCOLOR transcolor = D3DCOLOR_XRGB(0, 0, 0));

void Sprite_Draw_Frame(
	LPDIRECT3DTEXTURE9 texture = NULL,
	int destx = 0,
	int desty = 0,
	int framew = 64,
	int frameh = 64,
	int framenum = 0,
	int columns = 1);

void Sprite_Animate(int &frame, int startframe, int endframe,
	int direction, int &starttime, int delay);

void Sprite_Transform_Draw(
	LPDIRECT3DTEXTURE9 image,
	int x,
	int y,
	int width,
	int height,
	int frame = 0,
	int columns = 1,
	float rotation = 0.0f,
	float scaleW = 1.0f,
	float scaleH = 1.0f,
	D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));

void Sprite_Transform_Draw(
	LPDIRECT3DTEXTURE9 image,
	int x,
	int y,
	int width,
	int height,
	int frame = 0,
	int columns = 1,
	float rotation = 0.0f,
	float scaling = 1.0f,
	D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));

// DirectInput objects, devices, and states
extern LPDIRECTINPUT8 dinput;
extern LPDIRECTINPUTDEVICE8 dimouse;
extern LPDIRECTINPUTDEVICE8 dikeyboard;
extern DIMOUSESTATE mouse_state;
extern XINPUT_GAMEPAD controllers[4];

// DirectInput functions
bool DirectInput_Init(HWND);
void DirectInput_Update();
void DirectInput_Shutdown();
int Key_Down(int);
int Mouse_Button(int);
int Mouse_X();
int Mouse_Y();
void XInput_Vibrate(int contNum = 0, int amount = 65535);
bool XInput_Controller_Found();

// bounding box collision detection
int Collision(SPRITE sprite1, SPRITE sprite2);
// radial collision detection
bool CollisionD(SPRITE sprite1, SPRITE sprite2);

// font functions
LPD3DXFONT MakeFont(std::string name = "Arial", int size = 24);
void FontPrint(
	LPD3DXFONT font,
	int x,
	int y,
	std::string text,
	D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));

// DirectSound function protoypes
bool DirectSound_Init(HWND hwnd);
void DirectSound_Shutdown();
CSound *LoadSound(std::string filename);
void PlaySound(CSound *sound);
void LoopSound(CSound *sound);
void StopSound(CSound *sound);

// 3D mesh function prototypes
void DeleteModel(MODEL *model);
MODEL *LoadModel(std::string filename);
bool FindFile(std::string *filename);
bool DoesFileExist(const std::string &filename);
void SplitPath(const std::string &inputPath, std::string *pathOnly, std::string *filenameOnly);

// set render states
void SetRenderstate(LPDIRECT3DDEVICE9 d3ddev, bool ambient, bool zBuff);

// game functions
bool Game_Init(HWND window);
void Game_Run(HWND window);
void Game_End();