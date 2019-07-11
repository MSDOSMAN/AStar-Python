// MIT License

// Copyright (c) 2019 Martin Slomczykowski

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <Python.h>
#include <Windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <tchar.h>
#include <sstream>
#include <string>

// IF YOU MAKE ANY CHANGE TO THIS, MAKE SURE TO BUILD IT AFTER!!!!!!!

using namespace std;

HANDLE GetProcessHandle() {
	HWND hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	if (hwnd == NULL) {
		cout << "Cannot find target application!" << endl;
		Sleep(3000);
		exit(-1);
	}
	else {
		DWORD procID;
		GetWindowThreadProcessId(hwnd, &procID);
		HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

		if (procID == NULL) {
			cout << "Cannot obtain process permission" << endl;
			Sleep(3000);
			exit(-1);
		}
		else {
			return handle;
		}
	}
}

DWORD GetProcessID() {
	HWND hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	if (hwnd == NULL) {
		cout << "Cannot find target application!" << endl;
		Sleep(3000);
		exit(-1);
	}
	else {
		DWORD procID;
		GetWindowThreadProcessId(hwnd, &procID);

		if (procID == NULL) {
			cout << "Cannot obtain process permission" << endl;
			Sleep(3000);
			exit(-1);
		}
		else {
			return procID;
		}
	}
}

DWORD GetModuleBaseAddress(DWORD ProcessID, const char *szModuleName)// <- modded from TCHAR to const char	// this was written by Rake on https://guidedhacking.com/threads/get-module-base-address-tutorial-dwgetmodulebaseaddress.5781/ - I have modified it slightly --- consider rewriting this according to https://msdn.microsoft.com/en-us/library/system.diagnostics.processmodule.baseaddress(v=vs.110).aspx?cs-save-lang=1&cs-lang=cpp#code-snippet-2
{
	DWORD_PTR dwModuleBaseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessID);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 ModuleEntry32;
		ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &ModuleEntry32))
		{
			do
			{
				if (_tcsicmp(ModuleEntry32.szModule, szModuleName) == 0)
				{
					dwModuleBaseAddress = (DWORD_PTR)ModuleEntry32.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnapshot, &ModuleEntry32));
		}
		CloseHandle(hSnapshot);
	}
	return dwModuleBaseAddress;
}

class Offsets {
public:
	// PlayerBase Offsets
	const DWORD oHealth = 0xFC;	// o means offset
	const DWORD oArmour = 0xB238;
	const DWORD oDefuser = 0xB248;
	const DWORD oHelmet = 0xB22C;
	const DWORD oActiveWeapon = 0x2EE8;
	const DWORD oTeamNum = 0xF0;
	const DWORD oVecOrigin = 0x134;
	const DWORD oCrosshairID = 0xB2A4;
	const DWORD oVecView = 0x104;
	const DWORD oDefusing = 0x3888;
	const DWORD oYaw = 0x012C;
	const DWORD oPitch = 0x0128; // this leads to a pointer so probs wont work in this state
	const DWORD oScoped = 0x387E;
	const DWORD oAccPen = 0x32B0;
	const DWORD oFlags = 0x100; // tells you if you are crouching, jump, standing still or combo 
	const DWORD oAccount = 0x2FA8; // I think this shows your money idk
	const DWORD oClip = 0x3204; // I think shows ammo in clip but Im not sure
	const DWORD oShotsFired = 0xA2B0; // This is shots fired in this burst - not total 
	const DWORD oVelocity = 0x110;
	const DWORD oPlayerInfo = 0x5240; // idk what this does 
	const DWORD oC4 = 0x161C; // has the bomb
	const DWORD oMoney = 0x26C8;
	const DWORD oAmmo = 0x04F97364;
	const DWORD oSpottedByMask = 0x97C; // Apparently used for checking if an entity is visible

										// Engine Offsets
	const DWORD oZcoordinate = 0x62DC04; // could be 0x62DC7C, or 0x63AA28
};

class Addresses {
public:
	const DWORD aLocalPlayer = 0xAAAAB4;	// a means address 
	const DWORD oEntityList = 0x4A8574C;
	const HANDLE csgoHandle = GetProcessHandle();
	const DWORD processID = GetProcessID();
	DWORD clientBaseAddress = GetModuleBaseAddress(processID, _T("client.dll")); // should not be an issue
	DWORD engineBaseAddress = GetModuleBaseAddress(processID, _T("engine.dll"));
	DWORD serverBaseAddress = GetModuleBaseAddress(processID, _T("server.dll"));
	DWORD accountBase; // This is only used for getting player money
	DWORD clipBase; // This is only used for getting the ammo in the current clip
	DWORD PlayerBase;
	DWORD EntityList[9]; // one of each player


	DWORD aPitch; // used for writing
	DWORD aYaw;

	Addresses() {
		ReadProcessMemory(csgoHandle, (PBYTE*)(clientBaseAddress + aLocalPlayer), &PlayerBase, sizeof(DWORD), 0);
		//ReadProcessMemory(csgoHandle, (PBYTE*)(clientBaseAddress + oEntityList + (2 * 0x10) - 0x10), &EntityList, sizeof(DWORD), 0);
		for (int i = 1; i < 11; i++) {
			DWORD temp;
			ReadProcessMemory(csgoHandle, (PBYTE*)(clientBaseAddress + oEntityList + (i * 0x10) - 0x10), &temp, sizeof(DWORD), 0);
			EntityList[i - 1] = temp;
		}
		ReadProcessMemory(csgoHandle, (PBYTE*)(0x5B92D98C), &accountBase, sizeof(DWORD), 0);
		ReadProcessMemory(csgoHandle, (PBYTE*)(engineBaseAddress + 0x57F84C), &aPitch, sizeof(DWORD), 0);
		ReadProcessMemory(csgoHandle, (PBYTE*)(engineBaseAddress + 0x57F84C), &aYaw, sizeof(DWORD), 0);
	}
};

struct PlayerPosition {
	float x;
	float y;
	float z;
};

struct VelocityStruct {
	float x; // 0/180 degree velocity 
	float y; // -90/90 degree velocity
	float z; // up/down velocity
			 // Getting total 2D velocity is easy, simply decompose the vectors using the yaw angle
			 // Getting total 3D veloicty is hard/ impossible because we dont know the angle of take off 
			 // when a player jumps. The main issue is training the bot to bhop, so we will have to somehow 
			 // just record the total velocity in a different way, or just estimate total velocity
};

struct AccountStrcut {
	int x;
	int y;
	int z;
	int a;
	int b;
	int c;
};

Addresses RaddObj;
Offsets RoffObj;

// Player Positions
PyObject* GetXPos(PyObject *self) { // PyObject *, PyObject* o <- Might need some sort of input
	PlayerPosition pos;	// this could just be a float and add the appropriate index to the offset i.e. 0, 1, 2 

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oVecOrigin), &pos, sizeof(PlayerPosition), 0);
	
	double output = pos.x;
	return PyFloat_FromDouble(output);
}
PyObject* GetYPos(PyObject *self) {
	PlayerPosition pos;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oVecOrigin), &pos, sizeof(PlayerPosition), 0);

	double output = pos.y;
	return PyFloat_FromDouble(output);
}
PyObject* GetZPos(PyObject *self) {
	float pos;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.engineBaseAddress + RoffObj.oZcoordinate), &pos, sizeof(float), 0);

	double output = pos;
	return PyFloat_FromDouble(output);
}
PyObject* GetZPos2(PyObject *self) { // cl_showpos 2 use this for aimbot
	PlayerPosition pos;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oVecOrigin), &pos, sizeof(PlayerPosition), 0);

	double output = pos.z;
	return PyFloat_FromDouble(output);
}

// Player Angles 
PyObject* GetPitchAngle(PyObject *self) {
	float Pitch;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oPitch), &Pitch, sizeof(float), 0);

	double output = Pitch;
	return PyFloat_FromDouble(output);
}
PyObject* GetYawAngle(PyObject *self) {
	float Yaw;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oYaw), &Yaw, sizeof(float), 0);

	double output = Yaw;
	return PyFloat_FromDouble(output);
}

// Player speed/ movement (flags not included)
PyObject* GetVelocityX(PyObject *self) {
	VelocityStruct Velo1;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oVelocity), &Velo1, sizeof(VelocityStruct), 0);
	
	double output = Velo1.x;
	return PyFloat_FromDouble(output);
}
PyObject* GetVelocityY(PyObject *self) {
	VelocityStruct Velo2;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oVelocity), &Velo2, sizeof(VelocityStruct), 0);

	double output = Velo2.y;
	return PyFloat_FromDouble(output);
}
PyObject* GetVelocityZ(PyObject *self) {
	VelocityStruct Velo3;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oVelocity), &Velo3, sizeof(VelocityStruct), 0);

	double output = Velo3.z;
	return PyFloat_FromDouble(output);
}

// Time info 
PyObject* GetStartOfRoundT(PyObject *self) { // This is very important, at the start of every round there is a buy period. This will turn 0 only for roughly 1 and a half seconds AT THE START OF EVERY PREROUND. From here you have to install a manual counter to track time
	int Start;
	DWORD stage1;
	DWORD stage2;
	DWORD stage3;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.clientBaseAddress + 0x04F09740), &stage1, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage1 + 0x144), &stage2, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage2 + 0x1F8), &stage3, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage3 + 0x718), &Start, sizeof(int), 0);

	return PyLong_FromLong(Start);
}
PyObject* GetStartOfRoundCT(PyObject *self) {	// This is actually buy time. It will be 1 if you can buy, 0 if you cant
							// We could use this for both sides. Remember that it also go to zero if you step outside of the buy zone
	int Start;
	DWORD stage1;
	DWORD stage2;
	DWORD stage3;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.clientBaseAddress + 0x04F977C4), &stage1, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage1 + 0x1164), &stage2, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage2 + 0x670), &stage3, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage3 + 0xF0), &Start, sizeof(int), 0);

	return PyLong_FromLong(Start);
}

// Hitpoints: Health, armour, helmet
PyObject* GetHealth(PyObject *self) {
	int Health;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oHealth), &Health, sizeof(int), 0);

	return PyLong_FromLong(Health);
}
PyObject* GetArmour(PyObject *self) {
	int Armour;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oArmour), &Armour, sizeof(int), 0);

	return PyLong_FromLong(Armour);
}
PyObject* GetHelmet(PyObject *self) {
	int Helmet;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oHelmet), &Helmet, sizeof(int), 0);

	return PyLong_FromLong(Helmet);
}

// Equipment: Defuser 
PyObject* GetDefuser(PyObject *self) {
	int Defuser;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oDefuser), &Defuser, sizeof(int), 0);

	return PyLong_FromLong(Defuser);
}
PyObject* GetActiveWeapon(PyObject *self) {
	int Weapon;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oActiveWeapon), &Weapon, sizeof(int), 0);

	return PyLong_FromLong(Weapon);
}
PyObject* GetC4(PyObject *self) {
	int C4;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oC4), &C4, sizeof(int), 0);

	return PyLong_FromLong(C4);
}


// Player Information
PyObject* GetAccount(PyObject *self) {
	int Account;
	DWORD stage1;
	DWORD stage2;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.clientBaseAddress + 0x04EBA434), &stage1, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage1 + 0x24), &stage2, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage2 + 0x54), &Account, sizeof(int), 0);

	return PyLong_FromLong(Account);
}
PyObject* GetTeam(PyObject *self) {
	int Team;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oTeamNum), &Team, sizeof(int), 0);

	return PyLong_FromLong(Team);
}


// Player Status
PyObject* GetFlags(PyObject *self) {
	int Flags;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oFlags), &Flags, sizeof(int), 0);

	return PyLong_FromLong(Flags);
}
PyObject* GetAccPen(PyObject *self) {
	int AccPen;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oAccPen), &AccPen, sizeof(int), 0);

	return PyLong_FromLong(AccPen);
}
PyObject* GetScoped(PyObject *self) {
	int Scoped;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oScoped), &Scoped, sizeof(int), 0);

	return PyLong_FromLong(Scoped);
}
PyObject* GetDefusing(PyObject *self) {
	int Defusing;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oDefusing), &Defusing, sizeof(int), 0);

	return PyLong_FromLong(Defusing);
}
PyObject* GetXHairID(PyObject *self) {	// This has different values depending on whether you are aiming at a CT or T. It also has a special value for when you look at each individual weapon.
	int ID;			// XHair does NOT see through smokes - good thing

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oCrosshairID), &ID, sizeof(int), 0);

	return PyLong_FromLong(ID);
}

// Ammo Related
PyObject* GetClip(PyObject *self) {
	int Clip;
	DWORD stage1;
	DWORD stage2;
	DWORD stage3;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.clientBaseAddress + 0x04FB4590), &stage1, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage1 + 0x1C4), &stage2, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage2 + 0xC), &stage3, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage3 + 0x94), &Clip, sizeof(int), 0);

	return PyLong_FromLong(Clip);
}
PyObject* GetReserve(PyObject *self) {
	int Reserve;
	DWORD stage1;
	DWORD stage2;
	DWORD stage3;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.clientBaseAddress + 0x04FB4590), &stage1, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage1 + 0x1C4), &stage2, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage2 + 0xC), &stage3, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage3 + 0x98), &Reserve, sizeof(int), 0);

	return PyLong_FromLong(Reserve);
}
PyObject* GetShotsFired(PyObject *self) {
	int ShotsFired;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.PlayerBase + RoffObj.oShotsFired), &ShotsFired, sizeof(int), 0);

	return PyLong_FromLong(ShotsFired);
}

// Server variables
PyObject* GetCTScore(PyObject *self) {
	int CTScore;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.serverBaseAddress + 0x99CAA8), &CTScore, sizeof(int), 0);

	return PyLong_FromLong(CTScore);
}
PyObject* GetTScore(PyObject *self) {
	int TScore;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.serverBaseAddress + 0x99CAAC), &TScore, sizeof(int), 0);

	return PyLong_FromLong(TScore);
}
PyObject* GetBombPlanted(PyObject *self) {
	int Bomb;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.serverBaseAddress + 0x99C5C4), &Bomb, sizeof(int), 0);

	return PyLong_FromLong(Bomb);
}
PyObject* GetBombXPosition(PyObject *self) {	// This will be 0 or just a random value UNLESS YOU ARE CLOSE TO/ EXPOSED TO THE BOMB!
							// Do NOT read this value until the bomb has been planted. Once it has been planted, unless the coordinate is on 
							// one of the bomb sites, dont treat it as real. This is because this coordinate will be read even if you see the 
							// bomb on someone's back. 
							// POTENTIALLY WRITE A FUNCTION TO WRITE INTO THE VALUE 0,  THIS WILL BE RUN CONSTANTLY UNLESS THE BOMB HAS BEEN PLANTED 
	float bombXpos;
	DWORD stage1;
	DWORD stage2;
	DWORD stage3;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.serverBaseAddress + 0x00940044), &stage1, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage1 + 0x4A8), &stage2, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage2 + 0x0), &stage3, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage3 + 0xA8), &bombXpos, sizeof(DWORD), 0);

	double output = bombXpos;
	return PyFloat_FromDouble(output);
}
PyObject* GetBombYPosition(PyObject *self) { // READ bomb X position
	float bombYpos;
	DWORD stage1;
	DWORD stage2;
	DWORD stage3;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.serverBaseAddress + 0x00940044), &stage1, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage1 + 0x4A8), &stage2, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage2 + 0x0), &stage3, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage3 + 0xAC), &bombYpos, sizeof(int), 0);

	double output = bombYpos;
	return PyFloat_FromDouble(output);
}

// Scoreboard 
PyObject* GetPlayerKills(PyObject *self) {
	int Kills;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.serverBaseAddress + 0x99CA88), &Kills, sizeof(int), 0);

	return PyLong_FromLong(Kills);
}
PyObject* GetPlayerAssists(PyObject *self) {
	int Assists;
	DWORD stage1;
	DWORD stage2;
	DWORD stage3;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.serverBaseAddress + 0x00A269F0), &stage1, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage1 + 0xF1C), &stage2, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage2 + 0x14), &stage3, sizeof(DWORD), 0);
	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(stage3 + 0x98), &Assists, sizeof(int), 0);

	return PyLong_FromLong(Assists);
}
PyObject* GetPlayerDeaths(PyObject *self) {
	int Deaths;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.serverBaseAddress + 0x99CA8C), &Deaths, sizeof(int), 0);

	return PyLong_FromLong(Deaths);
}

// Entity List
PyObject* GetPlayersSpotted(PyObject *self, PyObject *args) {
	int playerID;

	if (!PyArg_ParseTuple(args, "i", &playerID))
		return NULL;

	int Spotted;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.EntityList[playerID] + RoffObj.oSpottedByMask), &Spotted, sizeof(int), 0);

	return PyLong_FromLong(Spotted);
}
PyObject* GetPlayersTeams(PyObject *self, PyObject *args) {
	int playerID;

	if (!PyArg_ParseTuple(args, "i", &playerID))
		return NULL;

	int Team;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.EntityList[playerID] + RoffObj.oTeamNum), &Team, sizeof(int), 0);

	return PyLong_FromLong(Team);
}
PyObject* GetPlayersXpos(PyObject *self, PyObject *args) {
	int playerID;

	if (!PyArg_ParseTuple(args, "i", &playerID))
		return NULL;

	PlayerPosition pos;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.EntityList[playerID] + RoffObj.oVecOrigin), &pos, sizeof(PlayerPosition), 0);

	double output = pos.x;
	return PyFloat_FromDouble(output);
}
PyObject* GetPlayersYpos(PyObject *self, PyObject *args) {
	int playerID;

	if (!PyArg_ParseTuple(args, "i", &playerID))
		return NULL;

	PlayerPosition pos;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.EntityList[playerID] + RoffObj.oVecOrigin), &pos, sizeof(PlayerPosition), 0);

	double output = pos.y;
	return PyFloat_FromDouble(output);
}
PyObject* GetPlayersZpos2(PyObject *self, PyObject *args) {
	int playerID;

	if (!PyArg_ParseTuple(args, "i", &playerID))
		return NULL;

	PlayerPosition pos;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.EntityList[playerID] + RoffObj.oVecOrigin), &pos, sizeof(PlayerPosition), 0);

	double output = pos.z;
	return PyFloat_FromDouble(output);
}
PyObject* GetActiveWeapons(PyObject *self, PyObject *args) {
	int playerID;

	if (!PyArg_ParseTuple(args, "i", &playerID))
		return NULL;

	int Weapon;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.EntityList[playerID] + RoffObj.oActiveWeapon), &Weapon, sizeof(int), 0);

	return PyLong_FromLong(Weapon);
}
// ONLY USE pos if enemy has been spotted!
// ONLY USE weapon if enemy has been spotted!
PyObject* GetPlayersHealth(PyObject *self, PyObject *args) {
	int playerID;

	if (!PyArg_ParseTuple(args, "i", &playerID))
		return NULL;

	int Health;

	ReadProcessMemory(RaddObj.csgoHandle, (PBYTE*)(RaddObj.EntityList[playerID] + RoffObj.oHealth), &Health, sizeof(int), 0);

	return PyLong_FromLong(Health);
}

Addresses WaddObj;
Offsets WoffObj;

int FloatToInt(float Angle, int Negative) { // http://www.cplusplus.com/forum/general/63755/ JLBorges' code mostly
	static_assert(std::numeric_limits<float>::is_iec559,
		"native float must be an IEEE float");

	union { float fval; std::uint32_t ival; };
	fval = Angle;

	std::ostringstream stms;
	stms << std::hex << std::uppercase << ival;

	std::string hexString = stms.str();

	if (Negative == 1) {
		hexString[0] = 'C';
	}

	unsigned int output;

	std::istringstream converter(hexString);
	converter >> std::hex >> output;

	return output;

	// Learn how to use stringstream - this can be made so much better
}
PyObject* WritePitchAngle(PyObject *self, PyObject *args) {
	//float Angle = PyFloat_AsDouble(angle);
	//int Negative = PyLong_AsLong(negative);

	float Angle;
	int Negative;

	if (!PyArg_ParseTuple(args, "fi", &Angle, &Negative))
		return NULL;

	int newAngle;

	if (Negative == 1) { // Negative = true, IS NEGATIVE
						 // might not work
		newAngle = FloatToInt(Angle, 1); // int becasue thats how it works, not a float
	}
	else {				 // Negative = false, IS NOT NEGATIVE 
		newAngle = FloatToInt(Angle, 0);
	}
	WriteProcessMemory(WaddObj.csgoHandle, (PBYTE*)(WaddObj.aPitch + 0x4D10), &newAngle, sizeof(int), 0);

	Py_INCREF(Py_None);
	return Py_None;
}
PyObject* WriteYawAngle(PyObject *self, PyObject *args) {
	float Angle;
	int Negative;

	if (!PyArg_ParseTuple(args, "fi", &Angle, &Negative))
		return NULL;

	int newAngle;

	if (Negative == 1) {
		newAngle = FloatToInt(Angle, 1);
	}
	else {
		newAngle = FloatToInt(Angle, 0);
	}
	WriteProcessMemory(WaddObj.csgoHandle, (PBYTE*)(WaddObj.aYaw + 0x4D14), &newAngle, sizeof(int), 0);

	Py_INCREF(Py_None);
	return Py_None;
}



static PyMethodDef SubmarineMemoryManagement_methods[] = {
	// The first property is the name exposed to Python, fast_tanh, the second is the C++
	// function name that contains the implementation.
	{ "GetXPos", (PyCFunction)GetXPos, METH_O, nullptr },
	{ "GetYPos", (PyCFunction)GetYPos, METH_O, nullptr },
	{ "GetZPos", (PyCFunction)GetZPos, METH_O, nullptr },
	{ "GetZPos2", (PyCFunction)GetZPos2, METH_O, nullptr },
	{ "GetPitchAngle", (PyCFunction)GetPitchAngle, METH_O, nullptr },
	{ "GetYawAngle", (PyCFunction)GetYawAngle, METH_O, nullptr },
	{ "GetVelocityX", (PyCFunction)GetVelocityX, METH_O, nullptr },
	{ "GetVelocityY", (PyCFunction)GetVelocityY, METH_O, nullptr },
	{ "GetVelocityZ", (PyCFunction)GetVelocityZ, METH_O, nullptr },
	{ "GetStartOfRoundT", (PyCFunction)GetStartOfRoundT, METH_O, nullptr },
	{ "GetStartOfRoundCT", (PyCFunction)GetStartOfRoundCT, METH_O, nullptr },
	{ "GetHealth", (PyCFunction)GetHealth, METH_O, nullptr },
	{ "GetArmour", (PyCFunction)GetArmour, METH_O, nullptr },
	{ "GetHelmet", (PyCFunction)GetHelmet, METH_O, nullptr },
	{ "GetDefuser", (PyCFunction)GetDefuser, METH_O, nullptr },
	{ "GetActiveWeapon", (PyCFunction)GetActiveWeapon, METH_O, nullptr },
	{ "GetC4", (PyCFunction)GetC4, METH_O, nullptr },
	{ "GetAccount", (PyCFunction)GetAccount, METH_O, nullptr },
	{ "GetTeam", (PyCFunction)GetTeam, METH_O, nullptr },
	{ "GetFlags", (PyCFunction)GetFlags, METH_O, nullptr },
	{ "GetAccPen", (PyCFunction)GetAccPen, METH_O, nullptr },
	{ "GetScoped", (PyCFunction)GetScoped, METH_O, nullptr },
	{ "GetDefusing", (PyCFunction)GetDefusing, METH_O, nullptr },
	{ "GetXHairID", (PyCFunction)GetXHairID, METH_O, nullptr },
	{ "GetClip", (PyCFunction)GetClip, METH_O, nullptr },
	{ "GetReserve", (PyCFunction)GetReserve, METH_O, nullptr },
	{ "GetShotsFired", (PyCFunction)GetShotsFired, METH_O, nullptr },
	{ "GetCTScore", (PyCFunction)GetCTScore, METH_O, nullptr },
	{ "GetTScore", (PyCFunction)GetTScore, METH_O, nullptr },
	{ "GetBombPlanted", (PyCFunction)GetBombPlanted, METH_O, nullptr },
	{ "GetBombXPosition", (PyCFunction)GetBombXPosition, METH_O, nullptr },
	{ "GetBombYPosition", (PyCFunction)GetBombYPosition, METH_O, nullptr },
	{ "GetPlayerKills", (PyCFunction)GetPlayerKills, METH_O, nullptr },
	{ "GetPlayerAssists", (PyCFunction)GetPlayerAssists, METH_O, nullptr },
	{ "GetPlayerDeaths", (PyCFunction)GetPlayerDeaths, METH_O, nullptr },
	{ "GetPlayersSpotted", (PyCFunction)GetPlayersSpotted, METH_O, nullptr },
	{ "GetPlayersTeams", (PyCFunction)GetPlayersTeams, METH_O, nullptr },
	{ "GetPlayersXpos", (PyCFunction)GetPlayersXpos, METH_O, nullptr },
	{ "GetPlayersYpos", (PyCFunction)GetPlayersYpos, METH_O, nullptr },
	{ "GetPlayersZpos2", (PyCFunction)GetPlayersZpos2, METH_O, nullptr },
	{ "GetActiveWeapons", (PyCFunction)GetActiveWeapons, METH_O, nullptr },
	{ "GetPlayersHealth", (PyCFunction)GetPlayersHealth, METH_O, nullptr },
	{ "WritePitchAngle", (PyCFunction)WritePitchAngle, METH_O, nullptr },
	{ "WriteYawAngle", (PyCFunction)WriteYawAngle, METH_O, nullptr },

	// Terminate the array with an object containing nulls.
	{ nullptr, nullptr, 0, nullptr }
};

static PyModuleDef SubmarineMM_module = {
	PyModuleDef_HEAD_INIT,
	"SubmarineMM",                        // Module name to use with Python import statements
	"Memory management for the Submarine CSGO AI. It handle I/O with the game. DO NOT USE UNLESS RUNNING CSGO IN -insecure MODE!",  // Module description
	0,
	SubmarineMemoryManagement_methods                   // Structure that defines the methods of the module
};

PyMODINIT_FUNC PyInit_SubmarineMM() { // SubmarineMM is supposed to exactly match the project name, as found under target name in properties
	return PyModule_Create(&SubmarineMM_module);
}