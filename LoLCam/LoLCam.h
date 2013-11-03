#ifndef LOLCAM_H
#define LOLCAM_H
	
	//VALUES
	float XAngRot = 0;
	float YAngRot = 0;
	float XAngVal;
	float YAngVal;
	float XSpeed = 0.35f;
	float YSpeed = 0.35f;
	float XAccel = 200;
	float YAccel = 200;

	//SIGSCAN & MEMORY & PID ETC
	HANDLE hProcess;
	DWORD ModuleBase;
	DWORD pID;
	DWORD pID2;
	DWORD pID3;

	ULONG GetModuleAddress(char *szModuleName, const ULONG dwProcessId);
	DWORD GetModuleBase(LPSTR lpModuleName, DWORD dwProcessId);
	DWORD GetProcId(const char* ProcName);

	DWORD FindPattern( DWORD dwAddress, DWORD dwSize, PBYTE pbSignature, char* pszSignature );
	DWORD dwFindPattern( BYTE* pData, DWORD dwSize, BYTE* bMask, char* szMask );
	bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask);
	void offset();

	DWORD ADDRESS;
	int YANG;
	int XANG;


	void WriteMem(int ADDR, int VALUE);
	void WriteMemFloat(int ADDR, float VALUE);
	int ReadMem(int Address);
	float ReadMemFloat(int Address);

	char const *LoL = "\x4c\x65\x61\x67\x75\x65\x20\x4f\x66\x20\x4c\x65\x67\x65\x6e\x64\x73\x2e\x65\x78\x65";

	//MENU
	bool WindowActive();
	void button(int KEY, void (*f)(), bool &First, bool SingleClick, int ClickSpeed);//A function that can make a button that will repeat itself if holding down the button or single clicks
		//BUTTONS
		bool FirstTime;
		bool FirstTime2;
		bool FirstTime3;
		bool FirstTime4;
		bool boolDummy;

	char const *activity;

	void menu();
	void selectplus();
	void selectminus();
	void next();
	void prev();
	void RESET();
	int heightmenu = 0; //where the menu starts when opening program
	int maxmenu = 4; //maxmenu items minus 1 because it starts at 0
	int minmenu = 0; //minmenu items should always be 0
	int i = 0; //for the forloop
	char const *menuheight[] =
	{	"0", 
		"1",
		"2",
		"3",
	};// add for more menu items remember to put 1 more in maxmenu

	//THREADS
	void XRotatorTH(void);
	void YRotatorTH(void);

#endif