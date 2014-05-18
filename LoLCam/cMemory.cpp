#include "hMemory.h"

extern HANDLE g_hProcess;
extern LPVOID YANG;
extern LPVOID XANG;

Memory::Memory(const char *lpModuleName, LPSTR ModuleName)
{
	DWORD ModuleSize;

	DWORD pID = GetProcId(lpModuleName);
	if(pID == 0) 
		MessageBox(NULL, "Error: 1", NULL, NULL);
	
	g_hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pID);

	DWORD ModuleBase = GetModuleBase(ModuleName, pID);
	if(ModuleBase == 0)
	{
		MessageBox(NULL, "Error: 2 - LOLReplay not found, looking for BaronReplays", NULL, NULL);
		pID = GetProcId("League of Legends.exe");
			if(pID == 0)
				MessageBox(NULL, "Error: 1 - Process ID", NULL, NULL);

		ModuleBase = GetModuleBase("League of Legends.exe", pID);
		if(ModuleBase == 0)
		{
			MessageBox(NULL, "Error: 3 - BaronReplays not found either, will exit now", NULL, NULL);
			exit(1);
		}
		ModuleSize = GetModuleSize("League of Legends.exe", pID);
	}
	else
		ModuleSize = GetModuleSize("League Of Legends.exe", pID);

	if(offset(ModuleBase, ModuleSize) == 0)
		MessageBox(NULL, "Error: 4 - Sig scanner", NULL, NULL);

	XANG = LPVOID(offset(ModuleBase, ModuleSize) + 0x4);
	YANG = LPVOID(offset(ModuleBase, ModuleSize));
}

bool Memory::bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for(;*szMask;++szMask,++pData,++bMask)
		if(*szMask=='x' && *pData!=*bMask )
			return false;
	return (*szMask) == NULL;
}

DWORD Memory::dwFindPattern( BYTE* pData, DWORD dwSize, BYTE* bMask, char* szMask )
{
	for (size_t  i = 0; i < dwSize; i++ )
		if ( bDataCompare( (BYTE*)( pData + i ), bMask, szMask ) )
			return i;
	
	return NULL;
}

DWORD Memory::FindPattern(HANDLE hProcess, DWORD dwAddress, DWORD dwSize, PBYTE pbSignature, char* pszSignature )
{
	DWORD dwTemp = dwAddress;
	BYTE pBuf[ 2048 ] = { 0 };

	do
	{
		if ( ReadProcessMemory( hProcess, ( PVOID )dwTemp, pBuf, 2048, NULL ) == FALSE )
		{
			MessageBox(NULL, "Error: 5 - hProcess/dwTemp/pBuf", NULL, NULL);
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

DWORD Memory::GetModuleBase(LPSTR lpModuleName, DWORD dwProcessId)
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

DWORD Memory::GetModuleSize(char* module, DWORD pID)
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

DWORD Memory::GetProcId(const char* ProcName)
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

DWORD Memory::offset(DWORD ModuleBase, DWORD ModuleSize)
{
	DWORD ADDRESS;
	PBYTE XAngle_sig = (PBYTE)"\xD9\x05\x00\x00\x00\x00\x8D\x45\x00\x83\xC4\x00";
	char *XAngle_masks = "xx????xx?xx?";
	DWORD XAngle = FindPattern(g_hProcess, ModuleBase, ModuleSize, XAngle_sig, XAngle_masks);
	ReadProcessMemory(g_hProcess, LPCVOID(XAngle+0x2), &ADDRESS, sizeof(DWORD), NULL);
	return ADDRESS;
}
