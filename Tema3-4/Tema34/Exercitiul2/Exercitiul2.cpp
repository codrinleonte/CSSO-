#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <stack> 
#include<list>
#define BUF_SIZE 100000

using namespace std;


TCHAR szName[] = TEXT("fisier");

struct ProcessData {
	DWORD pid;
	DWORD ppid;
	WCHAR exeName[500];
};


BOOL SetPrivilege(
	HANDLE hToken,          // access token handle
	LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege   // to enable or disable privilege
)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(
		NULL,            // lookup privilege on local system
		lpszPrivilege,   // privilege to lookup 
		&luid))        // receives LUID of privilege
	{
		printf("LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		printf("AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

	{
		printf("The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}


vector<ProcessData> readProcesses()
{

	HANDLE hMapFile;
	char* pBuf;
	DWORD size;
	vector<ProcessData>processesVector;

	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		szName);               // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
		
		
	}

	pBuf = (char*)MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0, 0, 0);

	CopyMemory((LPVOID)&size, pBuf, sizeof(DWORD));
	pBuf += sizeof(DWORD);
	
	//wcout << "size " << size;
	for (int i = 0; i < size; ++i)
	{
		ProcessData process;

		CopyMemory((LPVOID)&process.pid, pBuf, sizeof(DWORD));
		pBuf += sizeof(DWORD);

		CopyMemory((LPVOID)&process.ppid, pBuf, sizeof(DWORD));
		pBuf += sizeof(DWORD);

		CopyMemory((LPVOID)process.exeName, pBuf, sizeof(WCHAR) * 500);
		pBuf += (sizeof(WCHAR) * 500);

		if (process.pid == 0)
		{
			continue;
		}
		//wcout << process.exeName << " " << process.pid << endl;
		processesVector.push_back(process);
	}
//	cout << size;
	getchar();
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);

	return processesVector;
}



void printTree(DWORD pid, vector<ProcessData>processes, int count) {
	stack<DWORD> stack;
	stack.push(pid);
	DWORD curPid;

	while (!stack.empty()) {
		curPid = stack.top();
		stack.pop();
		count++;
		for (int i = 0; i < processes.size(); i++) {
			if (processes[i].ppid == curPid) {
				stack.push(processes[i].pid);
				wcout <<" "<< processes[i].exeName << " " << processes[i].pid << " " << processes[i].ppid;
				wcout << endl;
				for (int i = 0; i < count; i++) {
					cout << " ";
				}
				printTree(processes[i].pid, processes, count);
			}
		}

	}
}



vector<DWORD> transformToTreeProcesses(vector<ProcessData> processes) {

	DWORD ppidCur;
	stack<DWORD> ppidQueue;
	list<DWORD>visitedPids;
	int count = 0;
	list<DWORD>allPpids;
	vector<ProcessData>rootsOfTree;
	vector<DWORD>rootsOfTreePids;
	int treesCount = 1;

	//kill_by_pid(284);

	for (int i = 0; i < processes.size(); i++) {
		allPpids.push_back(processes[i].pid);
	}
	for (int i = 0; i < processes.size(); i++) {
		if (!(find(allPpids.begin(), allPpids.end(), processes[i].ppid) != allPpids.end())) {
			//wcout << processes[i].exeName << " " << processes[i].pid << " " << processes[i].ppid;
			//cout << endl;

			ppidQueue.push(processes[i].pid);
			ProcessData p;
			p.pid = processes[i].pid;
			wcscpy_s(p.exeName, processes[i].exeName);
			p.ppid = processes[i].ppid;
			rootsOfTree.push_back(p);
			rootsOfTreePids.push_back(processes[i].pid);
		}
	}

	for (int i = 0; i <rootsOfTree.size(); i++) {
		wcout << "Arborele : " << treesCount++;
		wcout << endl;
		wcout << rootsOfTree[i].exeName<< rootsOfTree[i].pid;
		wcout << endl;
		printTree(rootsOfTree[i].pid, processes, 0);
		wcout << endl << endl;
	}
	
	while (!ppidQueue.empty())
	{
		ppidCur = ppidQueue.top();
		ppidQueue.pop();
		visitedPids.push_back(ppidCur);
		if ((find(rootsOfTreePids.begin(), rootsOfTreePids.end(), ppidCur) != rootsOfTreePids.end())) {
			count = 0;
		}
	}
	return rootsOfTreePids;
}



void kill_by_pid(int pid)
{
	HANDLE handy;
	handy = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, TRUE, pid);
	TerminateProcess(handy, 0);
}

int main()
{
	int nr;
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return FALSE;
	SetPrivilege(hToken, SE_DEBUG_NAME, true);

	vector<ProcessData> processes = readProcesses();
	vector<DWORD>roots;
	
	roots = transformToTreeProcesses(processes);


	cout << "Stergeti arborele cu numarul :";
	cin >> nr;
	
	if (nr > roots.size() || nr <= 0) {
		cout << "nu exista arborele cu numarul "<<nr;
		return 0;
	}
	kill_by_pid(roots[nr - 1]);

	CloseHandle(hToken);
	cin.get();
	return 0;

}