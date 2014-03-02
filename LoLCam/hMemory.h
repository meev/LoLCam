#ifndef HMEMORY_H
#define HMEMORY_H

#include <Windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <tchar.h>

class Memory
{
public:
	//Constructor
	Memory(const char *lpModuleName, LPSTR ModuleName);
	
	//Destructor
	~Memory();
private:

	//pID & Module info
	DWORD GetModuleBase(LPSTR lpModuleName, DWORD dwProcessId);
	DWORD GetModuleSize(char* module, DWORD pID);
	DWORD GetProcId(const char* ProcName);

	//Signature scanner
	DWORD FindPattern(HANDLE hProcess, DWORD dwAddress, DWORD dwSize, PBYTE pbSignature, char* pszSignature );
	DWORD dwFindPattern( BYTE* pData, DWORD dwSize, BYTE* bMask, char* szMask );
	bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask);

	//Returns X angle
	DWORD offset(DWORD ModuleBase, DWORD ModuleSize);
};

#endif