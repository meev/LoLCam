#include "hMain.h"
#include "hMemory.h"

Memory* g_pMemory;

float XAngRot;
float YAngRot;
float XAngVal;
float YAngVal;
float XSpeed;
float YSpeed;
float XAccel;
float YAccel;

void menu();
int heightmenu = 0;						//where the menu starts when opening program
int maxmenu = 4;						//maxmenu items minus 1 because it starts at 0
int minmenu = 0;						//minmenu items should always be 0
int i = 0;								//for the forloop
char const *activity;
char const *menuheight[] =
{	"0", 
	"1",
	"2",
	"3",
};										//Remember to add 1 for each menu item!

HANDLE g_hProcess;
LPVOID YANG;
LPVOID XANG;

void MouseControlTH(void)
{
	POINT p;
	POINT p2;
	while(1)
	{
		Sleep(1);
		if(GetAsyncKeyState(VK_NUMPAD7))
		{
			Sleep(10);
			GetCursorPos(&p2);
			float NewXAng = (p.x-p2.x)/XAccel;
			float NewYAng = (p.y-p2.y)/YAccel;

			ReadProcessMemory(g_hProcess, YANG, &YAngRot, sizeof(YAngRot), NULL);
			ReadProcessMemory(g_hProcess, XANG, &XAngRot, sizeof(XAngRot), NULL);

			NewYAng += YAngRot;
			NewXAng += XAngRot;

			WriteProcessMemory(g_hProcess, YANG, &NewYAng, sizeof(NewYAng), NULL);
			WriteProcessMemory(g_hProcess, XANG, &NewXAng, sizeof(NewXAng), NULL);

		}
		else
			GetCursorPos(&p);
	}
}

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
					WriteProcessMemory(g_hProcess, XANG, &XAngRot, sizeof(XAngRot), NULL);
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
				WriteProcessMemory(g_hProcess, XANG, &XAngRot, sizeof(XAngRot), NULL);
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
					WriteProcessMemory(g_hProcess, XANG, &XAngRot, sizeof(XAngRot), NULL);
					if(GetAsyncKeyState(VK_NUMPAD6) || GetAsyncKeyState(VK_NUMPAD4))
					{
						tempi = i;
						break;
					}
					else
						tempi = 0;
				}
			}
			ReadProcessMemory(g_hProcess, XANG, &XAngRot, sizeof(XAngRot), NULL);
		}

		if(GetAsyncKeyState(VK_NUMPAD6))
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
					WriteProcessMemory(g_hProcess, XANG, &XAngRot, sizeof(XAngRot), NULL);
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
				WriteProcessMemory(g_hProcess, XANG, &XAngRot, sizeof(XAngRot), NULL);
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
					WriteProcessMemory(g_hProcess, XANG, &XAngRot, sizeof(XAngRot), NULL);
					if(GetAsyncKeyState(VK_NUMPAD6) || GetAsyncKeyState(VK_NUMPAD4))
					{
						tempi = i;
						break;
					}
					else
						tempi = 0;
				}
			}
			ReadProcessMemory(g_hProcess, XANG, &XAngRot, sizeof(XAngRot), NULL);
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
					WriteProcessMemory(g_hProcess, YANG, &YAngRot, sizeof(YAngRot), NULL);
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
				WriteProcessMemory(g_hProcess, YANG, &YAngRot, sizeof(YAngRot), NULL);
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
					WriteProcessMemory(g_hProcess, YANG, &YAngRot, sizeof(YAngRot), NULL);
					if(GetAsyncKeyState(VK_NUMPAD2) || GetAsyncKeyState(VK_NUMPAD8))
					{
						tempi = i;
						break;
					}
					else
						tempi = 0;
				}
			}
			ReadProcessMemory(g_hProcess, YANG, &YAngRot, sizeof(YAngRot), NULL);
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
					WriteProcessMemory(g_hProcess, YANG, &YAngRot, sizeof(YAngRot), NULL);
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
			WriteProcessMemory(g_hProcess, YANG, &YAngRot, sizeof(YAngRot), NULL);
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
					WriteProcessMemory(g_hProcess, YANG, &YAngRot, sizeof(YAngRot), NULL);
					if(GetAsyncKeyState(VK_NUMPAD2) || GetAsyncKeyState(VK_NUMPAD8))
					{
						tempi = i;
						break;
					}
					else
						tempi = 0;
				}
			}
			ReadProcessMemory(g_hProcess, YANG, &YAngRot, sizeof(YAngRot), NULL);
		}
		Sleep(1);
	}
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

void RESET()
{
	float XAngReset = 180.f;
	float YAngReset = 56.f;
	YSpeed = 0.35f;
	XSpeed = 0.35f;
	XAccel = 200;
	YAccel = 200;
	WriteProcessMemory(g_hProcess, XANG, &XAngReset, sizeof(XAngReset), NULL);
	WriteProcessMemory(g_hProcess, YANG, &YAngReset, sizeof(YAngReset), NULL);
	activity = "Everything has been reset.";
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
			menuheight[i] = "<--";
		}
		else
		{
			menuheight[i] = "       ";
		}
	}
	system("cls");
	printf("\n  [Speed Control]\n");
	printf("    X Rotation speed - %g \t\t%s\n", XSpeed*10, menuheight[0]);
	printf("    Y Rotation speed - %g \t\t%s\n", YSpeed*10, menuheight[1]);
	printf("    X Stop/Start acceleration - %g \t%s\n", XAccel/10, menuheight[2]);
	printf("    Y Stop/Start acceleration - %g \t%s\n", YAccel/10, menuheight[3]);
	printf("\n  [Controls]\n");
	printf("    F8 - Reset everything \t\t%s\n", menuheight[4]);
	printf("\n  [Values]\n");
	printf("    X Angle: %g\n", XAngRot);
	printf("    Y Angle: %g\n", YAngRot);
	printf("\n  Message: %s\n", activity);
}

int _tmain(int argc, _TCHAR* argv[])
{
	system("MODE 50,16");
	g_pMemory = new Memory("League Of Legends.exe", "League Of Legends.exe");

	RESET();
	menu();
	CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)XRotatorTH, NULL, NULL, NULL);
	CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)YRotatorTH, NULL, NULL, NULL);
	CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)MouseControlTH, NULL, NULL, NULL);

	while(true)//Controls for the menu
	{
		Sleep(1);
		if(GetAsyncKeyState(VK_UP)&1 && GetConsoleWindow() == GetForegroundWindow())
			prev();
		if(GetAsyncKeyState(VK_DOWN)&1 && GetConsoleWindow() == GetForegroundWindow())
			next();
		if(GetAsyncKeyState(VK_RETURN)&1 && GetConsoleWindow() == GetForegroundWindow())
			selectplus();
		if(GetAsyncKeyState(VK_LEFT)&1 && GetConsoleWindow() == GetForegroundWindow())
			selectminus();
		if(GetAsyncKeyState(VK_RIGHT)&1 && GetConsoleWindow() == GetForegroundWindow())
			selectplus();
		if(GetAsyncKeyState(VK_F8)&1)
			RESET();
	}
	CloseHandle(g_hProcess);
	return 0;
}