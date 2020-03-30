#include "main.h"

//uncomment for debugprint
//#define DEBUG

bool ShowMenu = false;
bool Esp = true;
bool Esp_box = true;
bool Esp_line = true;
bool Esp_Skeleton = false;
bool Aimbot = true;
bool prediction = true;
bool compensate = true;
bool ItemEsp = false;
bool CareEsp = false;
bool DeathDrop = false;

static int VisDist = 500;
static int ItemDist = 200;

bool DrawAmmo = false;
bool DrawSniper = true;
bool DrawRifles = true;
bool DrawHeal = true;
bool DrawAttachments = false;
bool DrawWearables = true;

float AimFOV = 100;

uint64_t TargetPawn;

D3DXVECTOR4 Rect;
uint64_t GNamesAddress;
uint64_t Tmpadd;

#define DecryptData(argv)    fnDecryptFunctoin(Tmpadd, argv)

typedef int64_t(__fastcall* DecryptFunctoin)(uintptr_t key, uintptr_t argv);
DecryptFunctoin fnDecryptFunctoin = NULL;

void Decrypt() {
	int64_t DecryptPtr = read<uint64_t>(DriverHandle, processID, base_address + offs_decrypt);

	while (!DecryptPtr)
	{
		DecryptPtr = read<uint64_t>(DriverHandle, processID, base_address + offs_decrypt);
		Sleep(1000);
	}

	int32_t Tmp1Add = read<uint32_t>(DriverHandle, processID, DecryptPtr + 3);
	Tmpadd = Tmp1Add + DecryptPtr + 7;

	unsigned char ShellcodeBuff[1024] = { NULL };
	ShellcodeBuff[0] = 0x90;
	ShellcodeBuff[1] = 0x90;

	info_t blyat;
	blyat.pid = processID;
	blyat.address = DecryptPtr;
	blyat.value = &ShellcodeBuff[2];
	blyat.size = sizeof(ShellcodeBuff) - 2;

	unsigned long int asd;
	DeviceIoControl(DriverHandle, ctl_read, &blyat, sizeof blyat, &blyat, sizeof blyat, &asd, nullptr);

	ShellcodeBuff[2] = 0x48;
	ShellcodeBuff[3] = 0x8B;
	ShellcodeBuff[4] = 0xC1;
	ShellcodeBuff[5] = 0x90;
	ShellcodeBuff[6] = 0x90;
	ShellcodeBuff[7] = 0x90;
	ShellcodeBuff[8] = 0x90;

	fnDecryptFunctoin = (DecryptFunctoin)VirtualAlloc(NULL, sizeof(ShellcodeBuff) + 4, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	RtlCopyMemory((LPVOID)fnDecryptFunctoin, (LPVOID)ShellcodeBuff, sizeof(ShellcodeBuff));
}

DWORD DecryptCIndex(DWORD value)
{
	return __ROL4__(value ^ 0x286953BD, 0x7) ^ (__ROL4__(value ^ 0x286953BD, 0x7) << 0x10) ^ 0xCBB9628D;
}

std::string GetGNamesByObjID(int32_t ObjectID)
{
	int64_t fNamePtr = read<uint64_t>(DriverHandle, processID, GNamesAddress + int(ObjectID / offs_chunk) * 0x8);
	int64_t fName = read<uint64_t>(DriverHandle, processID, fNamePtr + int(ObjectID % offs_chunk) * 0x8);

	char pBuffer[64] = { NULL };

	info_t blyat;
	blyat.pid = processID;
	blyat.address = fName + 0x10;
	blyat.value = &pBuffer;
	blyat.size = sizeof(pBuffer);

	unsigned long int asd;
	DeviceIoControl(DriverHandle, ctl_read, &blyat, sizeof blyat, &blyat, sizeof blyat, &asd, nullptr);

	return std::string(pBuffer);
}

float GetActorHealth(uint64_t Actor)
{
	return read<float>(DriverHandle, processID, Actor + offs_Actor_Health);
}

Vector3 GetActorPos(DWORD_PTR entity)
{
	Vector3 pos;

	auto rootcomp = DecryptData(read<DWORD_PTR>(DriverHandle, processID, entity + offs_Actor_RootComponent));
	pos = read<Vector3>(DriverHandle, processID, rootcomp + offs_relativepos);

	return pos;
}

DWORD Menuthread(LPVOID in)
{
	while (1)
	{
		if (GetAsyncKeyState(VK_INSERT) & 1) {
			ShowMenu = !ShowMenu;
		}
		Sleep(2);
	}
}

int main()
{
	DriverHandle = CreateFileW(_xor_(L"\\\\.\\fsdfsdfrweaq4r312q4rsdfvsdfgsd").c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (DriverHandle == INVALID_HANDLE_VALUE)
	{
		XorS(L, "Load Driver first, exiting...\n");

		printf(L.decrypt());
		Sleep(2000);
		exit(0);
	}

	while (hwnd == NULL)
	{
		XorS(windowname, "PLAYERUNKNOWN'S BATTLEGROUNDS ");
		hwnd = FindWindowA(0, windowname.decrypt());

		XorS(looking, "Looking for process...\n");
		printf(looking.decrypt());
		Sleep(100);
	}
	GetWindowThreadProcessId(hwnd, &processID);

	info_t Input_Output_Data;
	Input_Output_Data.pid = processID;
	unsigned long int Readed_Bytes_Amount;

	DeviceIoControl(DriverHandle, ctl_base, &Input_Output_Data, sizeof Input_Output_Data, &Input_Output_Data, sizeof Input_Output_Data, &Readed_Bytes_Amount, nullptr);
	base_address = (unsigned long long int)Input_Output_Data.data;

	XorS(base, "Process base address: %p.\n");
	printf(base.decrypt(), (void*)base_address);

	Decrypt();

	CreateThread(NULL, NULL, Menuthread, NULL, NULL, NULL);

#ifdef DEBUG
	printf(_xor_("Gnames: %p.\n").c_str(), (void*)GNamesAddress);

	uint64_t pUWorld = DecryptData(read<uint64_t>(DriverHandle, processID, base_address + offs_uworld));
	printf(_xor_("pUWorld: %p.\n").c_str(), (void*)pUWorld);

	uint64_t PersistentLevel = DecryptData(read<uint64_t>(DriverHandle, processID, pUWorld + offs_PersistentLevel));
	printf(_xor_("PersistentLevel: %p.\n").c_str(), (void*)PersistentLevel);

	uint64_t ActorsArray = DecryptData(read<uint64_t>(DriverHandle, processID, PersistentLevel + offs_ActorsArray));
	printf(_xor_("ActorsArray: %p.\n").c_str(), (void*)ActorsArray);

	int32_t ActorCount = read<uint32_t>(DriverHandle, processID, ActorsArray + 0x8);
	printf(_xor_("ActorCount: %p.\n").c_str(), (void*)ActorCount);

	localplayer = read<uint64_t>(DriverHandle, processID, base_address + offs_localplayer);
	printf(_xor_("Localplayer: %p.\n").c_str(), (void*)localplayer);

	PlayerController = DecryptData(read<DWORD_PTR>(DriverHandle, processID, localplayer + offs_PlayerControllers));
	printf(_xor_("PlayerController: %p.\n").c_str(), (void*)PlayerController);

	LocalPawn = DecryptData(read<DWORD_PTR>(DriverHandle, processID, PlayerController + Offs_LocalPawn));
	printf(_xor_("LocalPawn: %p.\n").c_str(), (void*)LocalPawn);

	LocalMesh = read<DWORD_PTR>(DriverHandle, processID, LocalPawn + offs_Mesh);
	printf(_xor_("LocalMesh: %p.\n").c_str(), (void*)LocalMesh);

	PlayerCameraManager = read<DWORD_PTR>(DriverHandle, processID, PlayerController + offs_PlayerCameraManager);
	printf(_xor_("PlayerCameraManager: %p.\n").c_str(), (void*)PlayerCameraManager);
	//system("PAUSE");
#endif // DEBUG

	while (TRUE)
	{
		SetupWindow();
		DirectXInit(MyWnd);
		MainLoop();
	}
}

std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

void DrawStrokeText(int x, int y, const char* str)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(255, 255, 255, 255)), utf_8_2.c_str());
}

void DrawText1(int x, int y, const char* str, RGBA* color)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);

	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
}

void DrawLine(int x1, int y1, int x2, int y2, RGBA* color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), thickness);
}

void DrawCircle(int x, int y, int radius, RGBA* color, int segments)
{
	ImGui::GetOverlayDrawList()->AddCircle(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), segments);
}

void DrawBox(float X, float Y, float W, float H, RGBA* color)
{
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(X + 1, Y + 1), ImVec2(((X + W) - 1), ((Y + H) - 1)), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(X, Y), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
}

Vector3 velocity;

Vector3 CalculatePrediction(Vector3 TargetPosition, uint64_t actor)
{
	float InitialSpeed = 0.0f;
	uint64_t weapProcessor = read<uint64_t>(DriverHandle, processID, LocalPawn + offs_WEAPONPROCESSOR);

#ifdef DEBUG
	printf(_xor_("weapProcessor: %p.\n").c_str(), (void*)weapProcessor);
#endif // DEBUG

	uint64_t equippedWeap = read<uint64_t>(DriverHandle, processID, weapProcessor + offs_EquippedWeapons);

#ifdef DEBUG
	printf(_xor_("equippedWeap: %p.\n").c_str(), (void*)equippedWeap);
#endif // DEBUG

	BYTE CurrentWeaponIndex = read<BYTE>(DriverHandle, processID, weapProcessor + offs_EquippedWeapons + 0x20 + 0x1);

	uint64_t pWeapon = 0;

	if (CurrentWeaponIndex >= 0 && CurrentWeaponIndex < 3)
	{
		pWeapon = read<uint64_t>(DriverHandle, processID, equippedWeap + (CurrentWeaponIndex * 0x8));


#ifdef DEBUG
		printf(_xor_("pWeapon: %p.\n").c_str(), (void*)pWeapon);
#endif // DEBUG

		if (pWeapon > 0)
		{
			uint64_t WeaponData = read<uint64_t>(DriverHandle, processID, pWeapon + offs_WEAPONTRAJECTORYDATA);


#ifdef DEBUG
			printf(_xor_("WeaponData: %p.\n").c_str(), (void*)WeaponData);
#endif // DEBUG

			
			InitialSpeed = read<float>(DriverHandle, processID, WeaponData + offs_TRAJECTORYCONFIG);
		}
	}

	if (InitialSpeed == 0.0f)
	{
		InitialSpeed = 700.f;
	}

	uint64_t ActorRootcomp = DecryptData(read<uint64_t>(DriverHandle, processID, actor + offs_Actor_RootComponent));
	velocity = read<Vector3>(DriverHandle, processID, ActorRootcomp + offs_COMPONENT_VELOCITY);

	auto local_location = read<Vector3>(DriverHandle, processID, PlayerCameraManager + offs_CameraLocation);
	float Distance = local_location.Distance(TargetPosition) / 100.f;

	float temp = 1.23f;

	if (Distance < 50.f) 
		temp = 1.8f;
	else if (Distance < 100.f)
		temp = 1.72f;
	else if (Distance < 150.f) 
		temp = 1.23f;
	else if (Distance < 200.f) 
		temp = 1.24f;
	else if (Distance < 250.f)
		temp = 1.25f;
	else if (Distance < 300.f) 
		temp = 1.26f;
	else if (Distance < 350.f)
		temp = 1.27f;
	else if (Distance < 400.f) 
		temp = 1.28f;
	else if (Distance < 450.f)
		temp = 1.29f;
	else if (Distance < 500.f)
		temp = 1.30f;
	else temp = 1.35f;

	float Gravity = 9.72f;
	float baseFlyTime = Distance / InitialSpeed;
	float secFlyTime = baseFlyTime * temp;

	Vector3 PredictedShift = Vector3(velocity.x * secFlyTime, velocity.y * secFlyTime, velocity.z * secFlyTime);

	if (Distance > 100.f && pWeapon > 0)
	{
		int pObjectID = DecryptCIndex(read<int>(DriverHandle, processID, pWeapon + offs_Actor_ObjID));


#ifdef DEBUG
		std::cout << "pObjectID = " << pObjectID << std::endl;
#endif // DEBUG

		std::string Weap = GetGNamesByObjID(pObjectID);

#ifdef DEBUG
		printf(_xor_("Weapon [%s]\n").c_str(), Weap.c_str());
#endif // DEBUG

		float down = 45.f;

		if (Weap == (_xor_("Weapon_AK47").c_str()))
			down = 54.f;
		else if (Weap == (_xor_("WeapAUG_C").c_str()))
			down = 52.f;
		else if (Weap == (_xor_("Weapon_QBZ95").c_str()))
			down = 21.f;
		else if (Weap == (_xor_("Weapon_Kar98k").c_str()))
			down = 40.f;
		else if (Weap == (_xor_("Weapon_M24").c_str()))
			down = 35.f;
		else if (Weap == (_xor_("Weapon_SKS").c_str()) || Weap == (_xor_("Weapon_FNFal").c_str()))
			down = 46.f;
		else if (Weap == (_xor_("Weapon_SCAR-L").c_str()) || Weap == (_xor_("WeapGroza_C").c_str()))
			down = 48.f;
		else if (Weap == (_xor_("Weapon_HK416").c_str()))
			down = 50.f;
		else if (Weap == (_xor_("Weapon_M16A4").c_str()))
			down = 48.f;
		else if (Weap == (_xor_("WeapAWM_C").c_str()))
			down = 20.f;
		else if (Weap == (_xor_("Weapon_Mini14").c_str()) || Weap == (_xor_("Weapon_QBU88_C").c_str()))
			down = 45.f;
		else if (Weap == (_xor_("WeapMk14_C").c_str()))
			down = 38.f;
		else if (Weap == (_xor_("Weapon_G36C").c_str()))
			down = 21.f;
		else if (Weap == (_xor_("Weapon_BerylM762").c_str()))
			down = 55.f;
		else if (Weap == (_xor_("Weapon_Mk47Mutant").c_str()))
			down = 46.f;
		else down = 50.f;

		PredictedShift.z += 0.5 * Gravity * baseFlyTime * baseFlyTime * down;
	}

	PredictedShift.x += TargetPosition.x;
	PredictedShift.y += TargetPosition.y;
	PredictedShift.z += TargetPosition.z;

	return PredictedShift;
}

void aimbot(float x, float y)
{
	float ScreenCenterX = (Width / 2);
	float ScreenCenterY = (Height / 2);
	int AimSpeed = 1.0f;
	float TargetX = 0;
	float TargetY = 0;

	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);

	return;
}

Vector3 rootHeadOut;

void AimAt(DWORD_PTR entity)
{
	uint64_t currentactormesh = read<uint64_t>(DriverHandle, processID, entity + offs_Mesh);
	auto rootHead = GetBoneWithRotation(currentactormesh, hitbox);

	if (prediction)
	{
		if (compensate)
		{
			Vector3 sway = read<Vector3>(DriverHandle, processID, AnimScript + offs_CONTROL_ROTATION_CP);
			Vector3 recoil = read<Vector3>(DriverHandle, processID, AnimScript + offs_RECOIL_ADS_ROTATION_CP);

			sway.x += recoil.x;
			sway.y += recoil.y;
			sway.z += recoil.z;

			rootHeadOut = W2SAim(CalculatePrediction(rootHead, entity), sway);
		}
		else
		{
			rootHeadOut = WorldToScreen(CalculatePrediction(rootHead, entity));
		}
	}
	else
	{
		if (compensate)
		{
			Vector3 sway = read<Vector3>(DriverHandle, processID, AnimScript + offs_CONTROL_ROTATION_CP);
			Vector3 recoil = read<Vector3>(DriverHandle, processID, AnimScript + offs_RECOIL_ADS_ROTATION_CP);

			sway.x += recoil.x;
			sway.y += recoil.y;
			sway.z += recoil.z;

			rootHeadOut = W2SAim(rootHead, sway);
		}
		else
		{
			rootHeadOut = WorldToScreen(rootHead);
		}
	}

	if (rootHeadOut.y != 0 || rootHeadOut.y != 0)
	{
		aimbot(rootHeadOut.x, rootHeadOut.y);
	}
}

std::string ItemNameList(std::string pObjName)
{
	if (DrawRifles)
	{
		XorS(ak, "Item_Weapon_AK47_C");
		XorS(ak1, "AKM");
		XorS(hk416, "Item_Weapon_HK416_C");
		XorS(hk4161, "M416");
		XorS(m16, "Item_Weapon_M16A4_C");
		XorS(m161, "M16A4");
		XorS(scar, "Item_Weapon_SCAR-L_C");
		XorS(scar1, "SCAR-L");
		XorS(g36, "Item_Weapon_G36C_C");
		XorS(g361, "G36_C");
		XorS(ump, "Item_Weapon_UMP_C");
		XorS(ump1, "UMP");

		if (!strcmp(pObjName.c_str(), (ak.decrypt())))
		{
			return ak1.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (hk416.decrypt())))
		{
			return hk4161.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (m16.decrypt())))
		{
			return m161.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (scar.decrypt())))
		{
			return scar1.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (g36.decrypt())))
		{
			return g361.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (ump.decrypt())))
		{
			return ump1.decrypt();
		}
	}

	if (DrawSniper)
	{
		XorS(kar, "Item_Weapon_Kar98k_C");
		XorS(kar1, "Kar98k");
		XorS(m24, "Item_Weapon_M24_C");
		XorS(m241, "M24");
		XorS(vss, "Item_Weapon_VSS_C");
		XorS(vss1, "VSS");

		if (!strcmp(pObjName.c_str(), (kar.decrypt())))
		{
			return kar1.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (m24.decrypt())))
		{
			return m241.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (vss.decrypt())))
		{
			return vss1.decrypt();
		}
	}

	if (DrawWearables)
	{
		XorS(arm, "Item_Armor_D_01_Lv2_C");
		XorS(arm1, "Armor Lv2");
		XorS(arm3, "Item_Armor_C_01_Lv3_C");
		XorS(arm33, "Armor Lv3");

		XorS(head, "Item_Head_F_01_Lv2_C");
		XorS(head1, "Item_Head_F_02_Lv2_C");
		XorS(head3, "Item_Head_G_01_Lv3_C");

		XorS(hel, "Helmet Lv2");
		XorS(hel1, "Helmet Lv3");

		XorS(back, "Item_Back_F_01_Lv2_C");
		XorS(back1, "Item_Back_F_02_Lv2_C");
		XorS(back2, "Item_Back_C_01_Lv3_C");
		XorS(back3, "Item_Back_C_02_Lv3_C");

		XorS(bag, "Bag Lv2");
		XorS(bag1, "Bag Lv3");

		if (!strcmp(pObjName.c_str(), (arm.decrypt())))
		{
			return arm1.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (arm3.decrypt())))
		{
			return arm33.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (head.decrypt())) || !strcmp(pObjName.c_str(), (head1.decrypt())))
		{
			return hel.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (head3.decrypt())))
		{
			return hel1.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (back.decrypt())) || !strcmp(pObjName.c_str(), (back1.decrypt())))
		{
			return bag.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (back2.decrypt())) || !strcmp(pObjName.c_str(), (back3.decrypt())))
		{
			return bag1.decrypt();
		}
	}

	if (DrawHeal)
	{
		XorS(hlt, "Item_Heal_FirstAid_C");
		XorS(hlt1, "FirstAid");

		XorS(hlt2, "Item_Heal_MedKit_C");
		XorS(hlt3, "MedKit");

		XorS(hlt4, "Item_Boost_EnergyDrink_C");
		XorS(hlt5, "Soda");

		XorS(hlt6, "Item_Boost_PainKiller_C");
		XorS(hlt7, "PainKiller");

		if (!strcmp(pObjName.c_str(), (hlt.decrypt())))
		{
			return hlt1.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (hlt2.decrypt())))
		{
			return hlt3.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (hlt4.decrypt())))
		{
			return hlt5.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (hlt6.decrypt())))
		{
			return hlt7.decrypt();
		}
	}

	if (DrawAttachments)
	{
		XorS(att, "Item_Attach_Weapon_Upper_Scope3x_C");
		XorS(att1, "Scope 3x");

		XorS(att2, "Item_Attach_Weapon_Upper_Scope4x_C");
		XorS(att3, "Scope 4x");

		XorS(att4, "Item_Attach_Weapon_Upper_Scope6x_C");
		XorS(att5, "Scope 6x");

		XorS(att6, "Item_Attach_Weapon_Magazine_ExtendedQuickDraw_Large_C");
		XorS(att7, "QuickDraw");

		XorS(att8, "Item_Attach_Weapon_Magazine_Extended_Large_C");
		XorS(att9, "Extended Mag");

		XorS(att10, "Item_Attach_Weapon_Magazine_ExtendedQuickDraw_SniperRifle_C");
		XorS(att11, "Extended QuickDraw");

		XorS(att12, "Item_Attach_Weapon_Muzzle_Suppressor_Large_C");
		XorS(att13, "Suppressor");

		XorS(att14, "Item_Attach_Weapon_Lower_AngledForeGrip_C");
		XorS(att15, "Angled ForeGrip");

		XorS(att16, "Item_Attach_Weapon_Stock_AR_Composite_C");
		XorS(att17, "Stock AR");

		XorS(att18, "Item_Attach_Weapon_Muzzle_Suppressor_SniperRifle_C");
		XorS(att19, "Sniper Suppressor");

		XorS(att20, "Item_Attach_Weapon_Stock_SniperRifle_CheekPad_C");
		XorS(att21, "Sniper CheekPad");

		XorS(att22, "Item_Attach_Weapon_Magazine_ExtendedQuickDraw_SniperRifle_C");
		XorS(att23, "Sniper Extended QuickDraw Mag");

		XorS(att24, "Item_Attach_Weapon_Magazine_Extended_SniperRifle_C");
		XorS(att25, "Sniper Extended Mag");

		XorS(att26, "Item_Attach_Weapon_Stock_SniperRifle_BulletLoops_C");
		XorS(att27, "Sniper FruitLoops");

		if (!strcmp(pObjName.c_str(), (att.decrypt())))
		{
			return att1.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (att2.decrypt())))
		{
			return att3.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (att4.decrypt())))
		{
			return att5.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (att6.decrypt())))
		{
			return att7.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (att8.decrypt())))
		{
			return att9.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (att10.decrypt())))
		{
			return att11.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (att12.decrypt())))
		{
			return att13.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (att14.decrypt())))
		{
			return att15.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (att16.decrypt())))
		{
			return att17.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (att18.decrypt())))
		{
			return att19.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (att20.decrypt())))
		{
			return att21.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (att22.decrypt())))
		{
			return att23.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (att24.decrypt())))
		{
			return att25.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (att26.decrypt())))
		{
			return att27.decrypt();
		}
	}

	if (DrawAmmo)
	{
		XorS(amm1, "Item_Ammo_556mm_C");
		XorS(amm2, "5.56mm");

		XorS(amm3, "Item_Ammo_762mm_C");
		XorS(amm4, "7.62mm");

		XorS(amm5, "Item_Ammo_45ACP_C");
		XorS(amm6, ".45 Acp");

		if (!strcmp(pObjName.c_str(), (amm1.decrypt())))
		{
			return amm2.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (amm3.decrypt())))
		{
			return amm4.decrypt();
		}
		else if (!strcmp(pObjName.c_str(), (amm5.decrypt())))
		{
			return amm6.decrypt();
		}
	}

	return "";
}

std::string ItemNameListAirdrop(std::string pObjName)
{
	XorS(awm, "Item_Weapon_AWM_C");
	XorS(awm1, "AWM");

	XorS(mk14, "Item_Weapon_Mk14_C");
	XorS(mk141, "Mk14 EBR");

	XorS(aug, "Item_Weapon_AUG_C");
	XorS(aug1, "Aug");

	XorS(groza, "Item_Weapon_Groza_C");
	XorS(groza1, "Groza");

	XorS(pancer, "Item_Weapon_PanzerFaust100M_C");
	XorS(pancer1, "Rpg");

	XorS(arm3, "Item_Armor_C_01_Lv3_C");
	XorS(arm33, "Armor Lv3");

	XorS(head3, "Item_Head_G_01_Lv3_C");
	XorS(hel1, "Helmet Lv3");

	XorS(back2, "Item_Back_C_01_Lv3_C");
	XorS(bag1, "Bag Lv3");

	XorS(hlt2, "Item_Heal_MedKit_C");
	XorS(hlt3, "MedKit");

	XorS(hlt, "Item_Heal_FirstAid_C");
	XorS(hlt1, "FirstAid");

	XorS(adrenaline, "Item_Boost_AdrenalineSyringe_C");
	XorS(adrenaline1, "Adrenaline Syringe");

	XorS(supp, "Item_Attach_Weapon_Muzzle_Suppressor_Large_C");
	XorS(supp1, "Suppressor");

	XorS(supp2, "Item_Attach_Weapon_Muzzle_Suppressor_SniperRifle");
	XorS(supp3, "Sniper Suppressor");

	XorS(stock, "Item_Attach_Weapon_Stock_AR_Composite_C");
	XorS(stock1, "Stock AR");

	XorS(amm3, "Item_Ammo_762mm_C");
	XorS(amm4, "7.62mm");
	
	XorS(amm5, "Item_Ammo_300Magnum_C");
	XorS(amm6, "300Magnum");
	
	XorS(amm7, "Item_Ammo_556mm_C");
	XorS(amm8, "5.56mm");

	if (!strcmp(pObjName.c_str(), (awm.decrypt())))
	{
		return awm1.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (mk14.decrypt())))
	{
		return mk141.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (groza.decrypt())))
	{
		return groza1.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (pancer.decrypt())))
	{
		return pancer1.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (aug.decrypt())))
	{
		return aug1.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (arm3.decrypt())))
	{
		return arm33.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (adrenaline.decrypt())))
	{
		return adrenaline1.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (head3.decrypt())))
	{
		return hel1.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (back2.decrypt())))
	{
		return bag1.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (hlt.decrypt())))
	{
		return hlt1.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (hlt2.decrypt())))
	{
		return hlt3.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (supp.decrypt())))
	{
		return supp1.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (supp2.decrypt())))
	{
		return supp3.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (stock.decrypt())))
	{
		return stock1.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (amm3.decrypt())))
	{
		return amm4.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (amm5.decrypt())))
	{
		return amm6.decrypt();
	}
	else if (!strcmp(pObjName.c_str(), (amm7.decrypt())))
	{
		return amm8.decrypt();
	}

	return pObjName;
}

#define TYPE_PLAYER 1
#define TYPE_ITEM 2
#define TYPE_CARE 3
#define TYPE_DEATH 4

struct Entity
{
	DWORD_PTR addr;
	DWORD_PTR bone;
	DWORD_PTR type;
};

int EntityCount;
Entity EntityList[1250];

void UpdateAddress()
{
	Uworld = DecryptData(read<DWORD_PTR>(DriverHandle, processID, base_address + offs_uworld));
	Ulevel = DecryptData(read<DWORD_PTR>(DriverHandle, processID, Uworld + offs_PersistentLevel));
	localplayer = read<DWORD_PTR>(DriverHandle, processID, base_address + offs_localplayer);
	PlayerController = DecryptData(read<DWORD_PTR>(DriverHandle, processID, localplayer + offs_PlayerControllers));
	GNamesAddress = DecryptData(read<DWORD_PTR>(DriverHandle, processID, DecryptData(read<DWORD_PTR>(DriverHandle, processID, base_address + offs_gnames - 0x20))));
}

void UpdateEntity() {
	UpdateAddress();

	static int old_entitycount;

	DWORD_PTR enlist = DecryptData(read<DWORD_PTR>(DriverHandle, processID, Ulevel + offs_ActorsArray));
	int entitycount = read<int>(DriverHandle, processID, enlist + 0x8);
Reset:
	if (enlist <= 0 || entitycount <= 0) {
		UpdateAddress();

		old_entitycount = 0;
		enlist = DecryptData(read<DWORD_PTR>(DriverHandle, processID, Ulevel + offs_ActorsArray));
		entitycount = read<int>(DriverHandle, processID, enlist + 0x8);

		goto Update;
	}

	if ((entitycount == old_entitycount))
		return;

Update:
	int index = 0;

	for (int i = 0; i < entitycount; i++)
	{
		DWORD_PTR entity = read<DWORD_PTR>(DriverHandle, processID, read<DWORD_PTR>(DriverHandle, processID, enlist) + i * 0x8);
		
		if ((entity == 0))
			continue;
		
		int32_t actorid = DecryptCIndex(read<uint32_t>(DriverHandle, processID, entity + offs_Actor_ObjID));
		std::string name = GetGNamesByObjID(actorid);

		XorS(plr, "PlayerFemale_A");
		XorS(plr1, "PlayerFemale_A_C");
		XorS(plr2, "PlayerMale_A");
		XorS(plr3, "PlayerMale_A_C");
		XorS(drp, "DroppedItemGroup");
		XorS(care, "Carapackage_RedBox_C");
		XorS(death, "DeathDropItemPackage_C");

		if (name == plr.decrypt() || name == plr1.decrypt() || name == plr2.decrypt() || name == plr3.decrypt())
		{
			auto mesh = read<uint64_t>(DriverHandle, processID, entity + offs_Mesh);

			EntityList[index].addr = entity;
			EntityList[index].bone = mesh;
			EntityList[index].type = TYPE_PLAYER;

			if (index >= 1250) 
				return;

			index++;
		}
		else if (name == drp.decrypt())
		{
			EntityList[index].addr = entity;
			EntityList[index].type = TYPE_ITEM;
			if (index >= 1250)
				return;

			index++;
		}
		else if (name == care.decrypt())
		{
			EntityList[index].addr = entity;
			EntityList[index].type = TYPE_CARE;

			if (index >= 1250)
				return;

			index++;
		}
		else if (name == death.decrypt())
		{
			EntityList[index].addr = entity;
			EntityList[index].type = TYPE_DEATH;

			if (index >= 1250)
				return;

			index++;
		}
	}

	old_entitycount = entitycount;
	EntityCount = index;

	if ((EntityCount <= 0))
		goto Reset;
}

void DrawSkeleton(DWORD_PTR mesh)
{
	Vector3 neckpos = GetBoneWithRotation(mesh, EBoneIndex::neck_01);
	Vector3 pelvispos = GetBoneWithRotation(mesh, EBoneIndex::pelvis);

	Vector3 previous(0, 0, 0);
	Vector3 current, p1, c1;

	for (auto a : skeleton)
	{
		previous = Vector3(0, 0, 0);

		for (int bone : a)
		{
			current = bone == EBoneIndex::neck_01 ? neckpos : (bone == EBoneIndex::pelvis ? pelvispos : GetBoneWithRotation(mesh, bone));

			if (previous.x == 0.f)
			{
				previous = current;
				continue;
			}

			p1 = WorldToScreen(previous);
			c1 = WorldToScreen(current);

			DrawLine(p1.x, p1.y, c1.x, c1.y, &Col.red, 1.5);

			previous = current;
		}
	}
}

void DrawESP() {
	DrawCircle(ScreenCenterX, ScreenCenterY, AimFOV, &Col.NiggaGreen, 25);

	XorS(frame, "(%.1f FPS)\n");
	char dist[64];
	sprintf_s(dist, frame.decrypt(), ImGui::GetIO().Framerate);
	DrawText1(15, 15, dist, &Col.darkgreen_);

	if (hitboxpos == 0)
	{
		hitbox = 15; //head
	}
	else if (hitboxpos == 1)
	{
		hitbox = 5; //neck
	}
	else if (hitboxpos == 2)
	{
		hitbox = 4; //chest
	}
	else if (hitboxpos == 3)
	{
		hitbox = 1; //pelvis
	}

	if (aimkeypos == 0)
	{
		aimkey = 0x01;//left mouse button
	}
	else if (aimkeypos == 1)
	{
		aimkey = 0x02;//right mouse button
	}
	else if (aimkeypos == 2)
	{
		aimkey = 0x04;//middle mouse button
	}
	else if (aimkeypos == 3)
	{
		aimkey = 0x05;//x1 mouse button
	}
	else if (aimkeypos == 4)
	{
		aimkey = 0x06;//x2 mouse button
	}
	else if (aimkeypos == 5)
	{
		aimkey = 0x03;//control break processing
	}
	else if (aimkeypos == 6)
	{
		aimkey = 0x08;//backspace
	}
	else if (aimkeypos == 7)
	{
		aimkey = 0x09;//tab
	}
	else if (aimkeypos == 8)
	{
		aimkey = 0x0c;//clear
	}
	else if (aimkeypos == 9)
	{
		aimkey == 0x0D;//enter
	}
	else if (aimkeypos == 10)
	{
		aimkey = 0x10;//shift
	}
	else if (aimkeypos == 11)
	{
		aimkey = 0x11;//ctrl
	}
	else if (aimkeypos == 12)
	{
		aimkey == 0x12;//alt
	}
	else if (aimkeypos == 13)
	{
		aimkey == 0x14;//caps lock
	}
	else if (aimkeypos == 14)
	{
		aimkey == 0x1B;//esc
	}
	else if (aimkeypos == 15)
	{
		aimkey == 0x20;//space
	}
	else if (aimkeypos == 16)
	{
		aimkey == 0x30;//0
	}
	else if (aimkeypos == 17)
	{
		aimkey == 0x31;//1
	}
	else if (aimkeypos == 18)
	{
		aimkey == 0x32;//2
	}
	else if (aimkeypos == 19)
	{
		aimkey == 0x33;//3
	}
	else if (aimkeypos == 20)
	{
		aimkey == 0x34;//4
	}
	else if (aimkeypos == 21)
	{
		aimkey == 0x35;//5
	}
	else if (aimkeypos == 22)
	{
		aimkey == 0x36;//6
	}
	else if (aimkeypos == 23)
	{
		aimkey == 0x37;//7
	}
	else if (aimkeypos == 24)
	{
		aimkey == 0x38;//8
	}
	else if (aimkeypos == 25)
	{
		aimkey == 0x39;//9
	}
	else if (aimkeypos == 26)
	{
		aimkey == 0x41;//a
	}
	else if (aimkeypos == 27)
	{
		aimkey == 0x42;//b
	}
	else if (aimkeypos == 28)
	{
		aimkey == 0x43;//c
	}
	else if (aimkeypos == 29)
	{
		aimkey == 0x44;//d
	}
	else if (aimkeypos == 30)
	{
		aimkey == 0x45;//e
	}
	else if (aimkeypos == 31)
	{
		aimkey == 0x46;//f
	}
	else if (aimkeypos == 32)
	{
		aimkey == 0x47;//g
	}
	else if (aimkeypos == 33)
	{
		aimkey == 0x48;//h
	}
	else if (aimkeypos == 34)
	{
		aimkey == 0x49;//i
	}
	else if (aimkeypos == 35)
	{
		aimkey == 0x4A;//j
	}
	else if (aimkeypos == 36)
	{
		aimkey == 0x4B;//k
	}
	else if (aimkeypos == 37)
	{
		aimkey == 0x4C;//L
	}
	else if (aimkeypos == 38)
	{
		aimkey == 0x4D;//m
	}
	else if (aimkeypos == 39)
	{
		aimkey == 0x4E;//n
	}
	else if (aimkeypos == 40)
	{
		aimkey == 0x4F;//o
	}
	else if (aimkeypos == 41)
	{
		aimkey == 0x50;//p
	}
	else if (aimkeypos == 42)
	{
		aimkey == 0x51;//q
	}
	else if (aimkeypos == 43)
	{
		aimkey == 0x52;//r
	}
	else if (aimkeypos == 44)
	{
		aimkey == 0x53;//s
	}
	else if (aimkeypos == 45)
	{
		aimkey == 0x54;//t
	}
	else if (aimkeypos == 46)
	{
		aimkey == 0x55;//u
	}
	else if (aimkeypos == 47)
	{
		aimkey == 0x56;//v
	}
	else if (aimkeypos == 48)
	{
		aimkey == 0x57;//w
	}
	else if (aimkeypos == 49)
	{
		aimkey == 0x58;//x
	}
	else if (aimkeypos == 50)
	{
		aimkey == 0x59;//y
	}
	else if (aimkeypos == 51)
	{
		aimkey == 0x5A;//z
	}
	else if (aimkeypos == 52)
	{
		aimkey == 0x60;//numpad 0
	}
	else if (aimkeypos == 53)
	{
		aimkey == 0x61;//numpad 1
	}
	else if (aimkeypos == 54)
	{
		aimkey == 0x62;//numpad 2
	}
	else if (aimkeypos == 55)
	{
		aimkey == 0x63;//numpad 3
	}
	else if (aimkeypos == 56)
	{
		aimkey == 0x64;//numpad 4
	}
	else if (aimkeypos == 57)
	{
		aimkey == 0x65;//numpad 5
	}
	else if (aimkeypos == 58)
	{
		aimkey == 0x66;//numpad 6
	}
	else if (aimkeypos == 59)
	{
		aimkey == 0x67;//numpad 7
	}
	else if (aimkeypos == 60)
	{
		aimkey == 0x68;//numpad 8
	}
	else if (aimkeypos == 61)
	{
		aimkey == 0x69;//numpad 9
	}
	else if (aimkeypos == 62)
	{
		aimkey == 0x6A;//multiply
	}

	if (Esp)
	{
		UpdateEntity();

		LocalPawn = DecryptData(read<DWORD_PTR>(DriverHandle, processID, PlayerController + Offs_LocalPawn));
		LocalMesh = read<DWORD_PTR>(DriverHandle, processID, LocalPawn + offs_Mesh);
		AnimScript = read<DWORD_PTR>(DriverHandle, processID, LocalMesh + offs_UANIMINSTANCE);
		PlayerCameraManager = read<DWORD_PTR>(DriverHandle, processID, PlayerController + offs_PlayerCameraManager);

		DWORD_PTR entity, mesh;

		float closestDistance = FLT_MAX;
		uint64_t closestPawn = NULL;

		for (int i = 0; i < EntityCount; i++) 
		{
			entity = EntityList[i].addr;
			mesh = EntityList[i].bone;

			if (EntityList[i].type == TYPE_PLAYER)
			{
				int MyTeamId = read<int>(DriverHandle, processID, LocalPawn + offs_Actor_TeamNumber);
				int ActorTeamId = read<int>(DriverHandle, processID, entity + offs_Actor_TeamNumber);

				float health = GetActorHealth(entity);

				if (health > 0.0f)
				{
					if (MyTeamId != ActorTeamId)
					{
						Vector3 head = GetBoneWithRotation(mesh, 15);
						Vector3 w2shead = WorldToScreen(head);
						Vector3 localhead = GetBoneWithRotation(LocalMesh, 15);
						Vector3 HeadBox = WorldToScreen(Vector3(head.x, head.y, head.z + 15));
						Vector3 root = GetBoneWithRotation(mesh, 0);
						Vector3 rootw2s = WorldToScreen(root);

						/*if (rootw2s.x < 2.f)
								continue;

						if (rootw2s.y < 2.f)
							continue;

						float test = (float)Width - 1;

						if (rootw2s.x > test)
							continue;*/

						float distance = localhead.Distance(head) / 100.f;

						if (Esp_box)
						{
							if (distance < VisDist)
							{
								float Height1 = abs(HeadBox.y - rootw2s.y);
								float Width1 = Height1 * 0.65;

								DrawBox(HeadBox.x - (Width1 / 2), HeadBox.y, Width1, Height1, &Col.red);

								if (health >= 100)
									health = 100;

								XorS(dst, "Distance [%.fm]\n Health [%i]\n");

								char dist[64];
								sprintf_s(dist, dst.decrypt(), distance, (int)health);
								DrawStrokeText(rootw2s.x - (Width1 / 2), rootw2s.y, dist);
							}
						}

						if (Esp_line)
						{
							if (distance < VisDist)
							{
								DrawLine(Width / 2, Height, rootw2s.x, rootw2s.y, &Col.red, 1.5);
							}
						}

						if (Esp_Skeleton)
						{
							if (distance < VisDist)
							{
								DrawSkeleton(mesh);
							}
						}

						float localrendertime = read<float>(DriverHandle, processID, LocalMesh + offs_LastRenderTimeOnScreen);
						float actorrendertime = read<float>(DriverHandle, processID, mesh + offs_LastRenderTimeOnScreen);

						if (actorrendertime == localrendertime)
						{
							auto dx = w2shead.x - (Width / 2);
							auto dy = w2shead.y - (Height / 2);
							auto dist = sqrtf(dx * dx + dy * dy);

							if (dist < AimFOV && dist < closestDistance) {
								closestDistance = dist;
								closestPawn = entity;
							}
						}
					}
				}
			}
			else if (EntityList[i].type == TYPE_ITEM)
			{
				if (ItemEsp)
				{
					RGBA color;
					uint64_t pItemArray = read<uint64_t>(DriverHandle, processID, entity + offs_DroppedItem);
					int pItemCount = read<int>(DriverHandle, processID, entity + offs_DroppedItemGroup_Count);

					if (!pItemArray || !pItemCount || pItemCount > 100)
						continue;

					for (int n = 0; n < pItemCount; n++)
					{
						uint64_t pItemObjPtr = read<uint64_t>(DriverHandle, processID, pItemArray + (n * 0x10));

						if (!pItemObjPtr)
							continue;

						uint64_t pUItemAddress = read<uint64_t>(DriverHandle, processID, pItemObjPtr + offs_DroppedItemGroup_UItem);

						if (pUItemAddress == 0)
							continue;

						int pUItemID = read<int>(DriverHandle, processID, pUItemAddress + offs_UITEM_ID);

						if (pUItemID < 0 || pUItemID > 399999)
							continue;

						std::string ItemName = GetGNamesByObjID(pUItemID);

						XorS(nul, "NULL");

						if (ItemName == nul.decrypt())
							continue;

						XorS(amm1, "Item_Ammo_556mm_C");
						XorS(amm2, "Item_Ammo_762mm_C");
						XorS(amm3, "Item_Ammo_45ACP_C");

						if (ItemName == amm1.decrypt() || ItemName == amm2.decrypt() || ItemName == amm3.decrypt())
							color = Col.blue;

						XorS(kar, "Item_Weapon_Kar98k_C");
						XorS(m24, "Item_Weapon_M24_C");
						XorS(vss, "Item_Weapon_VSS_C");

						if (ItemName == kar.decrypt() || ItemName == m24.decrypt() || ItemName == vss.decrypt())
							color = Col.pink;

						XorS(hlt, "Item_Heal_FirstAid_C");
						XorS(hlt2, "Item_Heal_MedKit_C");
						XorS(hlt4, "Item_Boost_EnergyDrink_C");
						XorS(hlt6, "Item_Boost_PainKiller_C");

						if (ItemName == hlt.decrypt() || ItemName == hlt2.decrypt() || ItemName == hlt4.decrypt() || ItemName == hlt6.decrypt())
							color = Col.green;

						XorS(ak, "Item_Weapon_AK47_C");
						XorS(hk416, "Item_Weapon_HK416_C");
						XorS(m16, "Item_Weapon_M16A4_C");
						XorS(scar, "Item_Weapon_SCAR-L_C");
						XorS(g36, "Item_Weapon_G36C_C");
						XorS(ump, "Item_Weapon_UMP_C");

						if (ItemName == ak.decrypt() || ItemName == hk416.decrypt() || ItemName == m16.decrypt() || ItemName == scar.decrypt() || ItemName == g36.decrypt() || ItemName == ump.decrypt())
							color = Col.red;

						XorS(arm, "Item_Armor_D_01_Lv2_C");
						XorS(arm3, "Item_Armor_C_01_Lv3_C");
						XorS(head, "Item_Head_F_01_Lv2_C");
						XorS(head1, "Item_Head_F_02_Lv2_C");
						XorS(head3, "Item_Head_G_01_Lv3_C");
						XorS(back, "Item_Back_F_01_Lv2_C");
						XorS(back1, "Item_Back_F_02_Lv2_C");
						XorS(back2, "Item_Back_C_01_Lv3_C");
						XorS(back3, "Item_Back_C_02_Lv3_C");

						if (ItemName == arm.decrypt() || ItemName == arm3.decrypt() || ItemName == head.decrypt() || ItemName == head1.decrypt() || ItemName == head3.decrypt() || ItemName == back.decrypt() || ItemName == back1.decrypt() || ItemName == back2.decrypt() || ItemName == back3.decrypt())
							color = Col.white;

						XorS(att, "Item_Attach_Weapon_Upper_Scope3x_C");
						XorS(att1, "Item_Attach_Weapon_Upper_Scope4x_C");
						XorS(att2, "Item_Attach_Weapon_Upper_Scope6x_C");
						XorS(att3, "Item_Attach_Weapon_Magazine_ExtendedQuickDraw_Large_C");
						XorS(att4, "Item_Attach_Weapon_Magazine_Extended_Large_C");
						XorS(att5, "Item_Attach_Weapon_Magazine_ExtendedQuickDraw_SniperRifle_C");
						XorS(att6, "Item_Attach_Weapon_Muzzle_Suppressor_Large_C");
						XorS(att7, "Item_Attach_Weapon_Lower_AngledForeGrip_C");
						XorS(att8, "Item_Attach_Weapon_Stock_AR_Composite_C");
						XorS(att9, "Item_Attach_Weapon_Muzzle_Suppressor_SniperRifle_C");
						XorS(att10, "Item_Attach_Weapon_Stock_SniperRifle_CheekPad_C");
						XorS(att11, "Item_Attach_Weapon_Magazine_ExtendedQuickDraw_SniperRifle_C");
						XorS(att12, "Item_Attach_Weapon_Magazine_Extended_SniperRifle_C");
						XorS(att13, "Item_Attach_Weapon_Stock_SniperRifle_BulletLoops_C");

						if (ItemName == att.decrypt() || ItemName == att1.decrypt() || ItemName == att2.decrypt() || ItemName == att3.decrypt() || ItemName == att4.decrypt() || ItemName == att5.decrypt() || ItemName == att6.decrypt() || ItemName == att7.decrypt() || ItemName == att8.decrypt() || ItemName == att9.decrypt() || ItemName == att10.decrypt() || ItemName == att11.decrypt() || ItemName == att12.decrypt() || ItemName == att13.decrypt())
							color = Col.orange;

						std::string ItemNamelst = ItemNameList(ItemName);

						if (ItemNamelst == "")
							continue;

						Vector3 localhead = GetBoneWithRotation(LocalMesh, 15);
						Vector3 location = read<Vector3>(DriverHandle, processID, pItemObjPtr + offs_absolutepos);
						Vector3 w2sloc = WorldToScreen(location);

						float distance = localhead.Distance(location) / 100.f;

						if (distance < ItemDist)
						{
							DrawText1(w2sloc.x, w2sloc.y, ItemNamelst.c_str(), &color);
						}
					}
				}
			}
			else if (EntityList[i].type == TYPE_DEATH)
			{
				if (DeathDrop)
				{
					Vector3 pos = GetActorPos(entity);
					Vector3 W2SPos = WorldToScreen(pos);
					Vector3 localhead = GetBoneWithRotation(LocalMesh, 15);
					float distance = localhead.Distance(pos) / 100.f;

					auto deathdropitems = read<DWORD_PTR>(DriverHandle, processID, entity + offs_ITEM_PACKAGE);
					auto deathdropitemscount = read<int>(DriverHandle, processID, entity + offs_ITEM_PACKAGE + 0x8);

					if (deathdropitemscount != 0) {
						XorS(death, "DeathDrop: ");
						XorS(m, "m");

						auto temp = std::string(death.decrypt() + std::to_string((int)distance) + m.decrypt());

						for (size_t i = 0; i < deathdropitemscount; i++)
						{
							DWORD_PTR Item = read<DWORD_PTR>(DriverHandle, processID, deathdropitems + i * 0x8);
							int32_t itemid = read<int>(DriverHandle, processID, Item + offs_UITEM_ID);

							std::string name = ItemNameList(GetGNamesByObjID(itemid));

							temp += "\n";
							temp += name;
						}
						DrawText1(W2SPos.x, W2SPos.y, temp.c_str(), &Col.pink);
					}
				}
			}
			else if (EntityList[i].type == TYPE_CARE)
			{
				if (CareEsp)
				{
					auto AirdropItems = read<DWORD_PTR>(DriverHandle, processID, entity + offs_ITEM_PACKAGE);
					auto AirdropItemscount = read<int>(DriverHandle, processID, entity + offs_ITEM_PACKAGE + 0x8);

					if (AirdropItemscount != 0) {
						Vector3 pos = GetActorPos(entity);
						Vector3 W2SPos = WorldToScreen(pos);
						Vector3 localhead = GetBoneWithRotation(LocalMesh, 15);

						float distance = localhead.Distance(pos) / 100.f;

						XorS(air, "Airdrop: ");
						XorS(m, "m");

						auto temp = std::string(air.decrypt() + std::to_string((int)distance) + m.decrypt());

						for (size_t i = 0; i < AirdropItemscount; i++)
						{
							DWORD_PTR Item = read<DWORD_PTR>(DriverHandle, processID, AirdropItems + i * 0x8);
							int32_t itemid = read<int>(DriverHandle, processID, Item + offs_UITEM_ID);

							std::string name = ItemNameListAirdrop(GetGNamesByObjID(itemid));

							temp += "\n";
							temp += name;
						}
						DrawText1(W2SPos.x, W2SPos.y, temp.c_str(), &Col.pink);
					}
				}
			}
			else {
				UpdateEntity();
			}
		}

		if (Aimbot)
		{
			if (Aimbot && closestPawn && GetAsyncKeyState(aimkey) < 0) {
				AimAt(closestPawn);
			}
		}
	}
}

HRESULT DirectXInit(HWND hWnd)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(3);

	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = hWnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = Width;
	p_Params.BackBufferHeight = Height;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
	{
		p_Object->Release();
		exit(4);
	}

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(p_Device);

	ImGui::StyleColorsClassic();
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	style->WindowTitleAlign.x = 0.50f;
	style->FrameRounding = 2.0f;

	p_Object->Release();

	return S_OK;
}

void SetupWindow()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetWindowToTarget, 0, 0, 0);

	WNDCLASSEX wClass =
	{
		sizeof(WNDCLASSEX),
		0,
		WinProc,
		0,
		0,
		nullptr,
		LoadIcon(nullptr, IDI_APPLICATION),
		LoadCursor(nullptr, IDC_ARROW),
		nullptr,
		nullptr,
		M_Name,
		LoadIcon(nullptr, IDI_APPLICATION)
	};

	if (!RegisterClassEx(&wClass))
		exit(1);

	if (hwnd)
	{
		GetClientRect(hwnd, &GameRect);
		POINT xy;
		ClientToScreen(hwnd, &xy);
		GameRect.left = xy.x;
		GameRect.top = xy.y;

		Width = GameRect.right;
		Height = GameRect.bottom;
	}
	else 
		exit(2);

	MyWnd = CreateWindowEx(NULL, M_Name, M_Name, WS_POPUP | WS_VISIBLE, GameRect.left, GameRect.top, Width, Height, NULL, NULL, 0, NULL);

	DwmExtendFrameIntoClientArea(MyWnd, &Margin);
	SetWindowLong(MyWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);

	ShowWindow(MyWnd, SW_SHOW);
	UpdateWindow(MyWnd);
}

void render() {
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (ShowMenu)
	{
		XorS(menu, "Skill.dlc");
		XorS(es1, "Enable Esp");
		XorS(es3, "Box Esp");
		XorS(es4, "Snapline");
		XorS(es7, "Skeleton");
		XorS(care, "Draw CarePackage");
		XorS(death, "Draw DeathDrop");
		XorS(es5, "Max Visuals & Aimbot Distance");
		XorS(i1, "Item Esp");
		XorS(i2, "DrawRifles");
		XorS(i3, "DrawSniper");
		XorS(i4, "DrawWearable");
		XorS(i5, "DrawHeal");
		XorS(i6, "DrawAmmo");
		XorS(i7, "DrawAttachments");
		XorS(i8, "Item Distance");

		XorS(aim1, "Aimbot");
		XorS(aim2, "Aimbot Key");
		XorS(aim3, "Aimbot Bone");
		XorS(aim4, "Prediction");
		XorS(aim5, "NoSway & NoRecoil");
		XorS(aim6, "Aimbot FOV");

		ImGui::Begin(menu.decrypt(), &ShowMenu, ImVec2(430, 765), 1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::Checkbox(es1.decrypt(), &Esp);
		ImGui::Checkbox(es3.decrypt(), &Esp_box);
		ImGui::Checkbox(es4.decrypt(), &Esp_line);
		ImGui::Checkbox(es7.decrypt(), &Esp_Skeleton);
		ImGui::Checkbox(care.decrypt(), &CareEsp);
		ImGui::Checkbox(death.decrypt(), &DeathDrop);
		ImGui::Text(es5.decrypt());
		ImGui::SliderInt("", &VisDist, 50, 1000);
		ImGui::Separator();
		ImGui::Checkbox(i1.decrypt(), &ItemEsp);
		ImGui::Checkbox(i2.decrypt(), &DrawRifles);
		ImGui::Checkbox(i3.decrypt(), &DrawSniper);
		ImGui::Checkbox(i4.decrypt(), &DrawWearables);
		ImGui::Checkbox(i5.decrypt(), &DrawHeal);
		ImGui::Checkbox(i6.decrypt(), &DrawAmmo);
		ImGui::Checkbox(i7.decrypt(), &DrawAttachments);
		ImGui::SliderInt(i8.decrypt(), &ItemDist, 50, 400);
		ImGui::Separator();
		ImGui::Checkbox(aim1.decrypt(), &Aimbot);
		ImGui::Combo(aim2.decrypt(), &aimkeypos, aimkeys, sizeof(aimkeys) / sizeof(*aimkeys));
		ImGui::Combo(aim3.decrypt(), &hitboxpos, hitboxes, sizeof(hitboxes) / sizeof(*hitboxes));
		ImGui::Checkbox(aim4.decrypt(), &prediction);
		ImGui::Checkbox(aim5.decrypt(), &compensate);
		ImGui::SliderFloat(aim6.decrypt(), &AimFOV, 25.f, 250.f);
		ImGui::End();
	}
	
	DrawESP();

	ImGui::EndFrame();
	p_Device->SetRenderState(D3DRS_ZENABLE, false);
	p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	if (p_Device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		p_Device->EndScene();
	}
	HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);

	if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		p_Device->Reset(&p_Params);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}

WPARAM MainLoop()
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, MyWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();

		if (GetAsyncKeyState(0x23) & 1)
			exit(8);

		if (hwnd_active == hwnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(MyWnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(hwnd, &rc);
		ClientToScreen(hwnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = hwnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(VK_LBUTTON)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;

		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;

			Width = rc.right;
			Height = rc.bottom;

			p_Params.BackBufferWidth = Width;
			p_Params.BackBufferHeight = Height;
			SetWindowPos(MyWnd, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
			p_Device->Reset(&p_Params);
		}
		render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanuoD3D();
	DestroyWindow(MyWnd);

	return Message.wParam;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message)
	{
	case WM_DESTROY:
		CleanuoD3D();
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (p_Device != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_Params.BackBufferWidth = LOWORD(lParam);
			p_Params.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = p_Device->Reset(&p_Params);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void CleanuoD3D()
{
	if (p_Device != NULL)
	{
		p_Device->EndScene();
		p_Device->Release();
	}
	if (p_Object != NULL)
	{
		p_Object->Release();
	}
}

int isTopwin()
{
	HWND hWnd = GetForegroundWindow();

	if (hWnd == hwnd)
		return TopWindowGame;

	if (hWnd == MyWnd)
		return TopWindowMvoe;

	return 0;
}

void SetWindowToTarget()
{
	while (true)
	{
		if (hwnd)
		{
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetWindowRect(hwnd, &GameRect);
			Width = GameRect.right - GameRect.left;
			Height = GameRect.bottom - GameRect.top;
			DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);

			if (dwStyle & WS_BORDER)
			{
				GameRect.top += 32;
				Height -= 39;
			}
			ScreenCenterX = Width / 2;
			ScreenCenterY = Height / 2;
			MoveWindow(MyWnd, GameRect.left, GameRect.top, Width, Height, true);
		}
		else
		{
			MessageBox(0, L"", L"", MB_OK | MB_ICONERROR);
			exit(0);
		}
	}
}