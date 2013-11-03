#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <Windows.h>
#include <direct.h>
#include <TlHelp32.h>
#include <ctime>
#include <string>
#include <cstdlib>
#include "LoLCam.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	system("MODE 50,16");
	printf("Awaiting League of Legends Processor...\n"); while(!pID) { pID = GetProcId(LoL);Sleep(100);pID2 = GetProcId("LoLCam.exe");Sleep(100); }
	printf("  League of Legends process found!\n");
	printf("\nProcess ID Found\n  Process ID: %d\n", pID);
	while(!hProcess) { hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pID);Sleep(100); }
	printf("\nSearching for modulebase address..."); while(!ModuleBase) { ModuleBase = GetModuleBase((LPSTR)LoL, pID);Sleep(100); }
	printf("\nModule base found!\n  Modulebase: 0x%X\n", ModuleBase);Sleep(100);
	
	offset(); //Finds the right addresses
	menu(); //Initializes menu

	YANG = ADDRESS;
	XANG = ADDRESS+0x4;

	CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)XRotatorTH, NULL, NULL, NULL);
	CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)YRotatorTH, NULL, NULL, NULL);
	
	while(1)
	{
		button(VK_UP, prev, FirstTime, true, NULL);
		button(VK_DOWN, next, FirstTime2, true, NULL);
		button(VK_RETURN, selectplus, FirstTime3, true, NULL);
		button(VK_LEFT, selectminus, boolDummy, false, 50);
		button(VK_RIGHT, selectplus, boolDummy, false, 50);
		button(VK_F8, RESET, FirstTime4, true, NULL);
	}
	return 0;
}

#pragma region MENU

bool WindowActive()
{
	HWND WindowHandel = GetForegroundWindow();
	GetWindowThreadProcessId(WindowHandel, &pID3);
	Sleep(1);
	if(pID2 == pID3)
		return true;
	else
		return false;
}

void button(int KEY, void (*f)(), bool &First, bool SingleClick, int ClickSpeed)
{
	Sleep(1);
	if(GetAsyncKeyState(KEY) && WindowActive())
	{
		if(SingleClick)
		{
			if(First)
			{
				First = false;
				f();
			}
		}
		else
		{
			f();
			for(int i=1;i<=ClickSpeed; i++)
			{
				Sleep(1);
				if(!GetAsyncKeyState(KEY))
					i = ClickSpeed;
			}
		}
	}
	else
		First = true;
}

void next()
{
	if(heightmenu == maxmenu)
		heightmenu = minmenu;
	else
		heightmenu = heightmenu+1;

	menu();
}

void prev()
{
	if(heightmenu == minmenu)
		heightmenu = maxmenu;
	else
		heightmenu = heightmenu-1;

	menu();
}

void selectplus()
{
	if(heightmenu == 0)
	{
		XSpeed+=0.01f;
		activity = "";
		menu();
	}
	else if(heightmenu == 1)
	{
		YSpeed+=0.01f;
		activity = "";
		menu();
	}
	else if(heightmenu == 2)
	{
		XAccel+=10;
		activity = "";
		menu();
	}
	else if(heightmenu == 3)
	{
		YAccel+=10;
		activity = "";
		menu();
	}
	else if(heightmenu == 4)
		RESET();
}


void selectminus()
{
	if(heightmenu == 0)
	{
		if(XSpeed <= 0.02)
			activity = "Reached minimum value";
		else
		{
			activity = "";
			XSpeed-=0.01f;
		}
		menu();
	}
	else if(heightmenu == 1)
	{
		if(YSpeed <= 0.02)
			activity = "Reached minimum value";
		else
		{
			activity = "";
			YSpeed-=0.01f;
		}
		menu();
	}
	else if(heightmenu == 2)
	{
		if(XAccel <= 10)
			activity = "Reached minimum value";
		else
		{
			activity = "";
			XAccel-=10;
		}
		menu();
	}
	else if(heightmenu == 3)
	{
		if(YAccel <= 10)
			activity = "Reached minimum value";
		else
		{
			activity = "";
			YAccel-=10;
		}
		menu();
	}
	else if(heightmenu == 4)
		RESET();
}

void menu()
{
	for(i=0;i<=maxmenu; i++)
	{
		if(i == heightmenu)
		{
			menuheight[i] = "      <--";
		}
		else
		{
			menuheight[i] = "          ";
		}
	}
	system("cls");
	printf("\n  [Speed Control]\n");
	printf("    X Rotation speed - %g %s\n", XSpeed*10, menuheight[0]);
	printf("    Y Rotation speed - %g %s\n", YSpeed*10, menuheight[1]);
	printf("    X Stop/Start acceleration - %g %s\n", XAccel/10, menuheight[2]);
	printf("    Y Stop/Start acceleration - %g %s\n", YAccel/10, menuheight[3]);
	printf("\n  [Controls]\n");
	printf("    F8 - Reset everything %s\n", menuheight[4]);
	printf("\n  [Values]\n");
	printf("    X Angle: %g\n", ReadMemFloat(XANG));
	printf("    Y Angle: %g\n", ReadMemFloat(YANG));
	printf("\n  Message: %s\n", activity);
}

void RESET()
{
	YSpeed = 0.35f;
	XSpeed = 0.35f;
	XAccel = 200;
	YAccel = 200;
	WriteMemFloat(XANG, 180);
	WriteMemFloat(YANG, 56);
	activity = "Everything has been reset.";
	menu();
}

#pragma endregion MENU

#pragma region THREADS

void XRotatorTH(void)
{
	bool FirstLeft = true;
	bool FirstRight = true;
	double tempi;
	while(1)
	{
		if(GetAsyncKeyState(VK_NUMPAD4))
		{
			if(FirstLeft == true)
			{
				for(float i=XSpeed*XAccel; i-->0;)
				{
					if(tempi != 0)
					{
						i -= (float)tempi;
						tempi = 0;
					}
					Sleep(10);
					XAngRot += XSpeed-(i/XAccel);
					if (XAngRot >= 360)
						XAngRot -= 360;
					WriteMemFloat(XANG, XAngRot);
					if(!GetAsyncKeyState(VK_NUMPAD4))
					{
						tempi = i;
						break;
					}
					else
						tempi = 0;
				}
				FirstLeft = false;
			}
			else
			{
				Sleep(10);
				XAngRot += XSpeed;
				if (XAngRot >= 360)
					XAngRot -= 360;
				WriteMemFloat(XANG, XAngRot);
			}
		}
		else
		{
			if(FirstLeft == false)
			{
				FirstLeft = true;
				for(float i=XSpeed*XAccel; i-->0;)
				{
					if(tempi != 0)
					{
						i -= (float)tempi;
						tempi = 0;
					}
					Sleep(10);
					XAngRot += i/XAccel;
					if (XAngRot >= 360)
						XAngRot -= 360;
					WriteMemFloat(XANG, XAngRot);
					if(GetAsyncKeyState(VK_NUMPAD6) || GetAsyncKeyState(VK_NUMPAD4))
					{
						tempi = i;
						break;
					}
					else
						tempi = 0;
				}
				menu();
			}
			XAngRot = ReadMemFloat(XANG);
		}

		if(GetAsyncKeyState(VK_NUMPAD6) & (1 << 15))
		{
			if(FirstRight == true)
			{
				for(float i=XSpeed*XAccel; i-->0;)
				{
					if(tempi != 0)
					{
						i -= (float)tempi;
						tempi = 0;
					}
					Sleep(10);
					XAngRot -= XSpeed-(i/XAccel);
					if (XAngRot <= 0)
						XAngRot += 360;
					WriteMemFloat(XANG, XAngRot);
					if(!GetAsyncKeyState(VK_NUMPAD6))
					{
						tempi = i;
						break;
					}
					else
						tempi = 0;
				}
				FirstRight = false;
			}
			else
			{
				Sleep(10);
				XAngRot -= XSpeed;
				if (XAngRot <= 0)
					XAngRot += 360;
				WriteMemFloat(XANG, XAngRot);
			}
		}
		else
		{
			if(FirstRight == false)
			{
				FirstRight = true;
				for(float i=XSpeed*XAccel; i-->0;)
				{
					if(tempi != 0)
					{
						i -= (float)tempi;
						tempi = 0;
					}
					Sleep(10);
					XAngRot -= i/XAccel;
					if (XAngRot <= 0)
						XAngRot += 360;
					WriteMemFloat(XANG, XAngRot);
					if(GetAsyncKeyState(VK_NUMPAD6) || GetAsyncKeyState(VK_NUMPAD4))
					{
						tempi = i;
						break;
					}
					else
						tempi = 0;
				}
				menu();
			}
			XAngRot = ReadMemFloat(XANG);
		}
		Sleep(1);
	}
}

void YRotatorTH(void)
{
	bool FirstDown = true;
	bool FirstUp = true;
	double tempi;
	while(1)
	{
		if(GetAsyncKeyState(VK_NUMPAD8))
		{
			if(FirstUp == true)
			{
				for(float i=YSpeed*YAccel; i-->0;)
				{
					if(tempi != 0)
					{
						i -= (float)tempi;
						tempi = 0;
					}
					Sleep(10);
					YAngRot += YSpeed-(i/YAccel);
					if (YAngRot >= 360)
						YAngRot -= 360;
					WriteMemFloat(YANG, (float)YAngRot);
					if(!GetAsyncKeyState(VK_NUMPAD8))
					{
						tempi = i;
						break;
					}
					else
						tempi = 0;
				}
				FirstUp = false;
			}
			else
			{
				Sleep(10);
				YAngRot += YSpeed;
				if (YAngRot >= 360)
					YAngRot -= 360;
				WriteMemFloat(YANG, YAngRot);
			}
		}
		else
		{
			if(FirstUp == false)
			{
				FirstUp = true;
				for(float i=YSpeed*YAccel; i-->0;)
				{
					if(tempi != 0)
					{
						i -= (float)tempi;
						tempi = 0;
					}
					Sleep(10);
					YAngRot += i/YAccel;
					if (YAngRot >= 360)
						YAngRot -= 360;
					WriteMemFloat(YANG, YAngRot);
					if(GetAsyncKeyState(VK_NUMPAD2) || GetAsyncKeyState(VK_NUMPAD8))
					{
						tempi = i;
						break;
					}
					else
						tempi = 0;
				}
				menu();
			}
			YAngRot = ReadMemFloat(YANG);
		}

		if(GetAsyncKeyState(VK_NUMPAD2))
		{
			if(FirstDown)
			{
				FirstDown = false;
				for(float i=YSpeed*YAccel; i-->0;)
				{
					if(tempi != 0)
					{
						i -= (float)tempi;
						tempi = 0;
					}
					Sleep(10);
					YAngRot -= YSpeed-(i/YAccel);
					if (YAngRot <= 0)
						YAngRot += 360;
					WriteMemFloat(YANG, YAngRot);
					if(!GetAsyncKeyState(VK_NUMPAD2))
					{
						tempi = i;
						break;
					}
					else
						tempi = 0;
				}
			}
			else
			{
				Sleep(10);
				YAngRot -= YSpeed;
				if (YAngRot <= 0)
					YAngRot += 360;
			}
			WriteMemFloat(YANG, YAngRot);
		}
		else
		{
			if(FirstDown == false)
			{
				FirstDown = true;
				for(float i=YSpeed*YAccel; i-->0;)
				{
					if(tempi != 0)
					{
						i -= (float)tempi;
						tempi = 0;
					}
					Sleep(10);
					YAngRot -= i/YAccel;
					if (YAngRot <= 0)
						YAngRot += 360;
					WriteMemFloat(YANG, YAngRot);
					if(GetAsyncKeyState(VK_NUMPAD2) || GetAsyncKeyState(VK_NUMPAD8))
					{
						tempi = i;
						break;
					}
					else
						tempi = 0;
				}
				menu();
			}
			YAngRot = ReadMemFloat(YANG);
		}
		Sleep(1);
	}
}

#pragma endregion THREADS

#pragma region MEMORY

void WriteMem(int ADDR, int VALUE)
{
	WriteProcessMemory(hProcess, LPVOID(ADDR), &VALUE, sizeof(VALUE), NULL);
}

void WriteMemFloat(int ADDR, float VALUE)
{
	WriteProcessMemory(hProcess, LPVOID(ADDR), &VALUE, sizeof(VALUE), NULL);
}

int ReadMem(int Address)
{
	int Output = 0;
	ReadProcessMemory(hProcess, LPCVOID(Address), &Output, sizeof(Output), NULL);
	return Output;
}

float ReadMemFloat(int Address)
{
	float Output = 0;
	ReadProcessMemory(hProcess, LPCVOID(Address), &Output, sizeof(Output), NULL);
	return Output;
}

DWORD GetModuleBase(LPSTR lpModuleName, DWORD dwProcessId)
{ 
	MODULEENTRY32 lpModuleEntry = {0};
	HANDLE hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwProcessId);
	if(!hSnapShot)	return NULL;
 
	lpModuleEntry.dwSize = sizeof(lpModuleEntry);
	BOOL bModule = Module32First( hSnapShot, &lpModuleEntry );
 
	while(bModule)
	{
		if(!strcmp(lpModuleEntry.szModule, lpModuleName ) )
		{
			CloseHandle( hSnapShot );
			return (DWORD)lpModuleEntry.modBaseAddr;
		}
 
		bModule = Module32Next( hSnapShot, &lpModuleEntry );
	}
 
	CloseHandle( hSnapShot );
	return NULL;
}

DWORD GetModuleSize(char* module)
{
	HANDLE hSnap;
	MODULEENTRY32 xModule;
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
	xModule.dwSize = sizeof(MODULEENTRY32);
	if (Module32First(hSnap, &xModule))
	{
		while (Module32Next(hSnap, &xModule))
		{
			if (!strncmp((char*)xModule.szModule, module, 8))
			{
				CloseHandle(hSnap);
				return (DWORD)xModule.modBaseSize;
			}
		}
	}
	CloseHandle(hSnap);
	return 0;
}

DWORD GetProcId(const char* ProcName)
{
	PROCESSENTRY32 pe32;
	HANDLE hSnapshot = NULL;
 
	pe32.dwSize = sizeof( PROCESSENTRY32 );
	hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
 
	if( Process32First( hSnapshot, &pe32 ) )
	{
		do{
			if( strcmp(pe32.szExeFile, ProcName) == 0 )
				break;
		}while( Process32Next( hSnapshot, &pe32 ) );
	}
 
	if( hSnapshot != INVALID_HANDLE_VALUE )
		CloseHandle( hSnapshot );
 
	return pe32.th32ProcessID;
}

#pragma endregion MEMORY

#pragma region SIG_SCANNER

bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for(;*szMask;++szMask,++pData,++bMask)
		if(*szMask=='x' && *pData!=*bMask )
			return false;
	return (*szMask) == NULL;
}

DWORD dwFindPattern( BYTE* pData, DWORD dwSize, BYTE* bMask, char* szMask )
{
	for (size_t  i = 0; i < dwSize; i++ )
		if ( bDataCompare( (BYTE*)( pData + i ), bMask, szMask ) )
			return i;
	
	return NULL;
}

DWORD FindPattern( DWORD dwAddress, DWORD dwSize, PBYTE pbSignature, char* pszSignature )
{
	DWORD dwTemp = dwAddress;
	BYTE pBuf[ 2048 ] = { 0 };

	do
	{
		if ( ReadProcessMemory( hProcess, ( PVOID )dwTemp, pBuf, 2048, NULL ) == FALSE )
		{
			printf("External FindPattern RPM : Error!\n");
			return NULL;
		}
		else
		{
			DWORD dwDelta = dwFindPattern( pBuf, 2048, pbSignature, pszSignature );

			if ( dwDelta )
				return dwTemp + dwDelta;

			dwTemp += 2048 - strlen(pszSignature);
		}
	}
	while ( true );

	return NULL;
}

#pragma endregion SIG_SCANNER

void offset()
{
	DWORD replay_size = GetModuleSize((LPSTR)LoL);
	PBYTE YAngle_sig = (PBYTE)"\x10\x9C\x00\x03\xE8\x9B\xA8\xB5\xFF";
	char *YAngle_masks = "xx?xxxxxx";
	DWORD YAngle = FindPattern(ModuleBase, replay_size, YAngle_sig, YAngle_masks);
	ReadProcessMemory(hProcess, LPCVOID(YAngle), &ADDRESS, sizeof(DWORD), NULL);
	printf("\nPattern found\n  Memory address: 0x%X\n", ADDRESS);
	ADDRESS += 0x0;
	printf("\nAdjusting memory address\n  New address: 0x%X", ADDRESS);
	Sleep(1000);
}
