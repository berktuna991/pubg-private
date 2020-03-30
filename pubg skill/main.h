#pragma once
#include <iostream>
#include <dwmapi.h>
#include <comdef.h> 
#include <d3d9.h>
#include <d3dx9.h>
#include "xor.hpp"
#include "defs.h"
#include <list>
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

#pragma warning( disable : 4244)
#pragma warning( disable : 4267)
#pragma warning( disable : 4305)
#pragma warning( disable : 4553)

#define M_PI 3.14159265358979323846264338327950288419716939937510

#define ctl_read                CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0721, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define ctl_base                CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0722, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

static int aimkey;
static int hitbox;
static int aimkeypos = 0;
static int hitboxpos = 0;

HANDLE DriverHandle;
uint64_t base_address;
DWORD processID;
HWND hwnd = NULL;

int Width = GetSystemMetrics(SM_CXSCREEN);
int Height = GetSystemMetrics(SM_CYSCREEN);
const MARGINS Margin = { -1 };
DWORD ScreenCenterX;
DWORD ScreenCenterY;

HRESULT DirectXInit(HWND hWnd);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
void SetupWindow();
WPARAM MainLoop();
void CleanuoD3D();

int isTopwin();
void SetWindowToTarget();

#define TopWindowGame 11
#define TopWindowMvoe 22

IDirect3D9Ex* p_Object = NULL;
IDirect3DDevice9Ex* p_Device = NULL;
D3DPRESENT_PARAMETERS p_Params = { NULL };

#define M_Name L" "
HWND MyWnd = NULL;
RECT GameRect = { NULL };
MSG Message = { NULL };

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

DWORD_PTR Uworld;
DWORD_PTR localplayer;
DWORD_PTR LocalPawn;
DWORD_PTR LocalMesh;
DWORD_PTR Ulevel;
DWORD_PTR actors;
DWORD_PTR PlayerController;
DWORD_PTR PlayerCameraManager;
DWORD_PTR AnimScript;

#define offs_uworld 0x747B768 //48 89 05 ? ? ? ? 48 83 C4 28 C3 B8 ? ? ? ? 48 C7 44 24 ? ? ? ? ? 66 C1 C8 08
#define offs_localplayer 0x7213710 //48 89 3d ? ? ? ? f2 0f 10 05 ? ? ? ?
#define offs_gnames 0x736A248
#define offs_decrypt 0x503D028
#define offs_chunk 0x4174

#define offs_PersistentLevel          0x228 //E8 ? ? ? ? 39 5C 24 48 
#define offs_ActorsArray              0x180 //E8 ? ? ? ? 39 5C 24 48 7E 29
#define offs_PlayerControllers        0x38
#define Offs_LocalPawn				  0x488 //8B 90 ? ? ? ? C1 EA 04 41 84 D4
#define offs_PlayerCameraManager      0x4A8 //41 ff 90 ? ? ? ? ? 8b ? ? ? ? ? 48 85 c9 74 ? 48 8b 01 4c 8d
#define offs_Actor_ObjID			  0xC
#define offs_Actor_Health			  0xFB8 //0F 57 C0 0F 2F 81 ? ? ? ? 72 0C
#define offs_Actor_RootComponent	  0x240 //E8 ? ? ? ? B0 01 48 8B 4D 00 
#define offs_Actor_TeamNumber		  0x1508
#define offs_Mesh					  0x478 //48 8B 99 ? ? ? ? 48 ? ? 48 ? ? 0F 84 ? ? ? ? 0F 10
#define offs_Mesh_BoneArray		      0xAD0 //F3 0F 10 81 ?? ?? ?? ?? F3 0F 10 0D ?? ?? ?? ?? 0F 2F C1
#define offs_Mesh_ComponentToWorld    0x330 //0F 8D ? ? ? ? 0F 10 9F ? ? ? ?
#define offs_relativepos			  0x364
#define offs_absolutepos			  0x340
#define offs_CameraRotation		      0x1720 //f2 0f 10 81 ? ? ? ? f2 41 0f 11 ? 8b 81 ? ? ? ? 41 89 ? ? c3
#define offs_CameraLocation		      0x1704 //E8 ? ? ? ? 44 8B 8F ? ? ? ? 0F 28 D6
#define offs_CameraFov			      0x1714 //77 08 F3 0F 10 81 ? ? ? ? C3 CC CC CC CC CC CC CC 48 89 5C 24 ?
#define offs_UANIMINSTANCE			  0xC78
#define offs_CONTROL_ROTATION_CP      0x6A8
#define offs_RECOIL_ADS_ROTATION_CP   0xB48
#define offs_LastRenderTimeOnScreen   0x0778

//items
#define offs_DroppedItem              0x158 
#define offs_DroppedItemGroup_Count offs_DroppedItem + 0x8
#define offs_DroppedItemGroup_UItem   0x660 //0F B6 F8 48 85 D2 74 13 4C 8B CE 4C 8B C5 49 8B CE ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?
#define offs_UITEM_ID				  0x270
#define offs_ITEM_PACKAGE             0x530 //48 8B 8F ? ? ? ? 48 63 D3 48 8B 14 D1 48

//prediction
#define offs_WEAPONPROCESSOR 0xFC8
#define offs_EquippedWeapons 0x2B8
#define offs_WEAPONTRAJECTORYDATA 0xF48
#define offs_TRAJECTORYCONFIG 0xB8
#define offs_COMPONENT_VELOCITY 0x20C

static const char* aimkeys[]
{
	"Left Mouse Button",
	"Right Mouse Button",
	"Middle Mouse Button",
	"Mouse Side 1",
	"Mouse Side 2",
	"Control-Break Processing",
	"Backspace",
	"Tab",
	"Clear",
	"Enter",
	"SHIFT",
	"CTRL",
	"ALT",
	"Caps Lock",
	"Esc",
	"Space",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"Numpad 0",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
	"Multiply"
};

static const char* hitboxes[]
{
	"Head",
	"Neck",
	"Chest",
	"Pelvis"
};

typedef struct info_t {
	int pid = 0;
	DWORD_PTR address;
	void* value;
	SIZE_T size;
	void* data;

    //string shit
    void* bufferAddress;
}info, *p_info;

template <typename Type>
Type read(void* DriverHandle, unsigned long int Process_Identifier, unsigned long long int Address)
{
	info_t Input_Output_Data;

	Input_Output_Data.pid = Process_Identifier;

	Input_Output_Data.address = Address;

	Type Return_Value;

	Input_Output_Data.value = &Return_Value;

	Input_Output_Data.size = sizeof(Type);

	unsigned long int Readed_Bytes_Amount;

	DeviceIoControl(DriverHandle, ctl_read, &Input_Output_Data, sizeof Input_Output_Data, &Input_Output_Data, sizeof Input_Output_Data, &Readed_Bytes_Amount, nullptr);

	return *(Type*)&Return_Value;
}

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	float x;
	float y;
	float z;

	inline float Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline float Distance(Vector3 v)
	{
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(float number) const {
		return Vector3(x * number, y * number, z * number);
	}
};

struct FQuat
{
	float x;
	float y;
	float z;
	float w;
};

struct FTransform
{
	FQuat rot;
	Vector3 translation;
	char pad[4];
	Vector3 scale;
	char pad1[4];

	D3DMATRIX ToMatrixWithScale()
	{
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

D3DXMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

Vector3 WorldToScreen(Vector3 WorldLocation)
{
	Vector3 Screenlocation = Vector3(0, 0, 0);

	Vector3 Rotation = read<Vector3>(DriverHandle, processID, PlayerCameraManager + offs_CameraRotation);
	D3DMATRIX tempMatrix = Matrix(Rotation);

	Vector3 vAxisX, vAxisY, vAxisZ;

	vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - read<Vector3>(DriverHandle, processID, PlayerCameraManager + offs_CameraLocation);
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float FovAngle = read<float>(DriverHandle, processID, PlayerCameraManager + offs_CameraFov);
	float ScreenCenterX = Width / 2;
	float ScreenCenterY = Height / 2;

	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

	return Screenlocation;
}

Vector3 W2SAim(Vector3 WorldLocation, Vector3 Rot)
{
	Vector3 Screenlocation = Vector3(0, 0, 0);

	Vector3 Rotation = Rot;
	D3DMATRIX tempMatrix = Matrix(Rotation);

	Vector3 vAxisX, vAxisY, vAxisZ;

	vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - read<Vector3>(DriverHandle, processID, PlayerCameraManager + offs_CameraLocation);
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float FovAngle = read<float>(DriverHandle, processID, PlayerCameraManager + offs_CameraFov);
	float ScreenCenterX = Width / 2;
	float ScreenCenterY = Height / 2;

	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

	return Screenlocation;
}

D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}

FTransform GetBoneIndex(DWORD_PTR mesh, int index)
{
	DWORD_PTR bonearray = read<DWORD_PTR>(DriverHandle, processID, mesh + offs_Mesh_BoneArray);
	return read<FTransform>(DriverHandle, processID, bonearray + (index * 0x30));
}

Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = read<FTransform>(DriverHandle, processID, mesh + offs_Mesh_ComponentToWorld);
	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

typedef struct
{
	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
}RGBA;

class Color
{
public:

	RGBA NiggaGreen = { 128, 224, 0, 200 };
	RGBA red = { 255,0,0,255 };
	RGBA Magenta = { 255,0,255,255 };
	RGBA yellow = { 255,255,0,255 };
	RGBA grayblue = { 128,128,255,255 };
	RGBA green = { 128,224,0,255 };
	RGBA darkgreen = { 0,224,128,255 };
	RGBA brown = { 192,96,0,255 };
	RGBA pink = { 255,168,255,255 };
	RGBA DarkYellow = { 216,216,0,255 };
	RGBA SilverWhite = { 236,236,236,255 };
	RGBA purple = { 144,0,255,255 };
	RGBA Navy = { 88,48,224,255 };
	RGBA skyblue = { 0,136,255,255 };
	RGBA graygreen = { 128,160,128,255 };
	RGBA blue = { 0,96,192,255 };
	RGBA orange = { 255,128,0,255 };
	RGBA peachred = { 255,80,128,255 };
	RGBA reds = { 255,128,192,255 };
	RGBA darkgray = { 96,96,96,255 };
	RGBA Navys = { 0,0,128,255 };
	RGBA darkgreens = { 0,128,0,255 };
	RGBA darkblue = { 0,128,128,255 };
	RGBA redbrown = { 128,0,0,255 };
	RGBA purplered = { 128,0,128,255 };
	RGBA greens = { 0,255,0,255 };
	RGBA envy = { 0,255,255,255 };
	RGBA black = { 0,0,0,255 };
	RGBA gray = { 128,128,128,255 };
	RGBA white = { 255,255,255,255 };
	RGBA blues = { 30,144,255,255 };
	RGBA lightblue = { 135,206,250,160 };
	RGBA Scarlet = { 220, 20, 60, 160 };
	RGBA white_ = { 255,255,255,200 };
	RGBA gray_ = { 128,128,128,200 };
	RGBA black_ = { 0,0,0,200 };
	RGBA red_ = { 255,0,0,200 };
	RGBA Magenta_ = { 255,0,255,200 };
	RGBA yellow_ = { 255,255,0,200 };
	RGBA grayblue_ = { 128,128,255,200 };
	RGBA green_ = { 128,224,0,200 };
	RGBA darkgreen_ = { 0,224,128,200 };
	RGBA brown_ = { 192,96,0,200 };
	RGBA pink_ = { 255,168,255,200 };
	RGBA darkyellow_ = { 216,216,0,200 };
	RGBA silverwhite_ = { 236,236,236,200 };
	RGBA purple_ = { 144,0,255,200 };
	RGBA Blue_ = { 88,48,224,200 };
	RGBA skyblue_ = { 0,136,255,200 };
	RGBA graygreen_ = { 128,160,128,200 };
	RGBA blue_ = { 0,96,192,200 };
	RGBA orange_ = { 255,128,0,200 };
	RGBA pinks_ = { 255,80,128,200 };
	RGBA Fuhong_ = { 255,128,192,200 };
	RGBA darkgray_ = { 96,96,96,200 };
	RGBA Navy_ = { 0,0,128,200 };
	RGBA darkgreens_ = { 0,128,0,200 };
	RGBA darkblue_ = { 0,128,128,200 };
	RGBA redbrown_ = { 128,0,0,200 };
	RGBA purplered_ = { 128,0,128,200 };
	RGBA greens_ = { 0,255,0,200 };
	RGBA envy_ = { 0,255,255,200 };
	RGBA glassblack = { 0, 0, 0, 160 };
	RGBA GlassBlue = { 65,105,225,80 };
	RGBA glassyellow = { 255,255,0,160 };
	RGBA glass = { 200,200,200,60 };
	RGBA Plum = { 221,160,221,160 };

};

Color Col;

enum EBoneIndex : int32
{
	root = 0,
	pelvis = 1,
	spine_01 = 2,
	spine_02 = 3,
	spine_03 = 4,
	neck_01 = 5,
	Head = 6,
	face_root = 7,
	eyebrows_pos_root = 8,
	eyebrows_root = 9,
	eyebrows_r = 10,
	eyebrows_l = 11,
	eyebrow_l = 12,
	eyebrow_r = 13,
	forehead_root = 14,
	forehead = 15,
	jaw_pos_root = 16,
	jaw_root = 17,
	jaw = 18,
	mouth_down_pos_root = 19,
	mouth_down_root = 20,
	lip_bm_01 = 21,
	lip_bm_02 = 22,
	lip_br = 23,
	lip_bl = 24,
	jaw_01 = 25,
	jaw_02 = 26,
	cheek_pos_root = 27,
	cheek_root = 28,
	cheek_r = 29,
	cheek_l = 30,
	nose_side_root = 31,
	nose_side_r_01 = 32,
	nose_side_r_02 = 33,
	nose_side_l_01 = 34,
	nose_side_l_02 = 35,
	eye_pos_r_root = 36,
	eye_r_root = 37,
	eye_rot_r_root = 38,
	eye_lid_u_r = 39,
	eye_r = 40,
	eye_lid_b_r = 41,
	eye_pos_l_root = 42,
	eye_l_root = 43,
	eye_rot_l_root = 44,
	eye_lid_u_l = 45,
	eye_l = 46,
	eye_lid_b_l = 47,
	nose_pos_root = 48,
	nose = 49,
	mouth_up_pos_root = 50,
	mouth_up_root = 51,
	lip_ul = 52,
	lip_um_01 = 53,
	lip_um_02 = 54,
	lip_ur = 55,
	lip_l = 56,
	lip_r = 57,
	hair_root = 58,
	hair_b_01 = 59,
	hair_b_02 = 60,
	hair_l_01 = 61,
	hair_l_02 = 62,
	hair_r_01 = 63,
	hair_r_02 = 64,
	hair_f_02 = 65,
	hair_f_01 = 66,
	hair_b_pt_01 = 67,
	hair_b_pt_02 = 68,
	hair_b_pt_03 = 69,
	hair_b_pt_04 = 70,
	hair_b_pt_05 = 71,
	hair_l_pt_01 = 72,
	hair_l_pt_02 = 73,
	hair_l_pt_03 = 74,
	hair_l_pt_04 = 75,
	hair_l_pt_05 = 76,
	hair_r_pt_01 = 77,
	hair_r_pt_02 = 78,
	hair_r_pt_03 = 79,
	hair_r_pt_04 = 80,
	hair_r_pt_05 = 81,
	camera_fpp = 82,
	GunReferencePoint = 83,
	GunRef = 84,
	breast_l = 85,
	breast_r = 86,
	clavicle_l = 87,
	upperarm_l = 88,
	lowerarm_l = 89,
	hand_l = 90,
	thumb_01_l = 91,
	thumb_02_l = 92,
	thumb_03_l = 93,
	thumb_04_l_MBONLY = 94,
	index_01_l = 95,
	index_02_l = 96,
	index_03_l = 97,
	index_04_l_MBONLY = 98,
	middle_01_l = 99,
	middle_02_l = 100,
	middle_03_l = 101,
	middle_04_l_MBONLY = 102,
	ring_01_l = 103,
	ring_02_l = 104,
	ring_03_l = 105,
	ring_04_l_MBONLY = 106,
	pinky_01_l = 107,
	pinky_02_l = 108,
	pinky_03_l = 109,
	pinky_04_l_MBONLY = 110,
	item_l = 111,
	lowerarm_twist_01_l = 112,
	upperarm_twist_01_l = 113,
	clavicle_r = 114,
	upperarm_r = 115,
	lowerarm_r = 116,
	hand_r = 117,
	thumb_01_r = 118,
	thumb_02_r = 119,
	thumb_03_r = 120,
	thumb_04_r_MBONLY = 121,
	index_01_r = 122,
	index_02_r = 123,
	index_03_r = 124,
	index_04_r_MBONLY = 125,
	middle_01_r = 126,
	middle_02_r = 127,
	middle_03_r = 128,
	middle_04_r_MBONLY = 129,
	ring_01_r = 130,
	ring_02_r = 131,
	ring_03_r = 132,
	ring_04_r_MBONLY = 133,
	pinky_01_r = 134,
	pinky_02_r = 135,
	pinky_03_r = 136,
	pinky_04_r_MBONLY = 137,
	item_r = 138,
	lowerarm_twist_01_r = 139,
	upperarm_twist_01_r = 140,
	Backpack = 141,
	backpack_01 = 142,
	backpack_02 = 143,
	slot_primary = 144,
	slot_secondary = 145,
	slot_melee = 146,
	slot_throwable = 147,
	coat_l_01 = 148,
	coat_l_02 = 149,
	coat_l_03 = 150,
	coat_l_04 = 151,
	coat_fl_01 = 152,
	coat_fl_02 = 153,
	coat_fl_03 = 154,
	coat_fl_04 = 155,
	coat_b_01 = 156,
	coat_b_02 = 157,
	coat_b_03 = 158,
	coat_b_04 = 159,
	coat_r_01 = 160,
	coat_r_02 = 161,
	coat_r_03 = 162,
	coat_r_04 = 163,
	coat_fr_01 = 164,
	coat_fr_02 = 165,
	coat_fr_03 = 166,
	coat_fr_04 = 167,
	thigh_l = 168,
	calf_l = 169,
	foot_l = 170,
	ball_l = 171,
	calf_twist_01_l = 172,
	thigh_twist_01_l = 173,
	thigh_r = 174,
	calf_r = 175,
	foot_r = 176,
	ball_r = 177,
	calf_twist_01_r = 178,
	thigh_twist_01_r = 179,
	slot_sidearm = 180,
	skirt_l_01 = 181,
	skirt_l_02 = 182,
	skirt_l_03 = 183,
	skirt_f_01 = 184,
	skirt_f_02 = 185,
	skirt_f_03 = 186,
	skirt_b_01 = 187,
	skirt_b_02 = 188,
	skirt_b_03 = 189,
	skirt_r_01 = 190,
	skirt_r_02 = 191,
	skirt_r_03 = 192,
	ik_hand_root = 193,
	ik_hand_gun = 194,
	ik_hand_r = 195,
	ik_hand_l = 196,
	ik_aim_root = 197,
	ik_aim_l = 198,
	ik_aim_r = 199,
	ik_foot_root = 200,
	ik_foot_l = 201,
	ik_foot_r = 202,
	camera_tpp = 203,
	ik_target_root = 204,
	ik_target_l = 205,
	ik_target_r = 206,
	VB_spine_03_spine_03 = 207,
	VB_upperarm_r_lowerarm_r = 208,
	VB_GunRef_spine_03 = 209,
	VB_camera_fpp_hand_l = 210,
	VB_camera_fpp_hand_r = 211,
	VB_camera_fpp_clavicle_l = 212,
	VB_camera_fpp_clavicle_r = 213,
	VB_lowerarm_l_hand_l = 214,
};

std::list<int> upper_part = { EBoneIndex::neck_01, EBoneIndex::Head, EBoneIndex::forehead };
std::list<int> right_arm = { EBoneIndex::neck_01, EBoneIndex::upperarm_r, EBoneIndex::lowerarm_r, EBoneIndex::hand_r };
std::list<int> left_arm = { EBoneIndex::neck_01, EBoneIndex::upperarm_l, EBoneIndex::lowerarm_l, EBoneIndex::hand_l };
std::list<int> spine = { EBoneIndex::neck_01, EBoneIndex::spine_01, EBoneIndex::spine_02, EBoneIndex::pelvis };
std::list<int> lower_right = { EBoneIndex::pelvis, EBoneIndex::thigh_r, EBoneIndex::calf_r, EBoneIndex::foot_r };
std::list<int> lower_left = { EBoneIndex::pelvis, EBoneIndex::thigh_l, EBoneIndex::calf_l, EBoneIndex::foot_l };
std::list<std::list<int>> skeleton = { upper_part, right_arm, left_arm, spine, lower_right, lower_left };