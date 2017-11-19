#include "stdafx.h"
#pragma warning(disable: 4996)
#pragma comment(lib, "user32.lib")
#include <wchar.h>
#include <atlstr.h>
#include <iostream>
#include <Windows.h>
#include <sstream>
#include <TlHelp32.h>
#include <vector>
#include <stack> 
#include<list>
#define BUF_SIZE 100000


TCHAR szName[] = TEXT("mem");


using namespace std;

struct ProcessData {
	DWORD pid;
	DWORD ppid;
	WCHAR exeName[100];
};


void kill_by_pid(int pid)
{
	HANDLE handy;
	handy = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, TRUE, pid);
	TerminateProcess(handy, 0);
}

vector<ProcessData> getAllProcessesAsAVector() {
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	vector<ProcessData> processesVector;
	//cer un snapshot la procese
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		cout<<"CreateToolhelp32Snapshot failed.err ";

	}
	//initializez dwSize cu dimensiunea structurii.
	pe32.dwSize = sizeof(PROCESSENTRY32);
	//obtin informatii despre primul proces
	if (!Process32First(hProcessSnap, &pe32))
	{

		CloseHandle(hProcessSnap);
		//	inchidem snapshot

		cout<< "Process32First failed. ";

	}
	do
	{
		//	afisez pid-ul si executabilul
		ProcessData p;
		p.pid = pe32.th32ProcessID;
		p.ppid = pe32.th32ParentProcessID;
		wcscpy_s(p.exeName, pe32.szExeFile);
		processesVector.push_back(p);
	} while (Process32Next(hProcessSnap, &pe32));
	//trec la urmatorul proces
	//inched handle-ul catre snapshot
	CloseHandle(hProcessSnap);
	system("pause");
	return processesVector;
}



int createMappedFile(vector<ProcessData> processesVector) {
	HANDLE hMapFile;
	char* pBuf;

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (hMapFile == NULL)
	{
		cout << "Could not create file mapping object (%d).\n" <<
			GetLastError();
		return 1;
	}
	pBuf = (char*)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		BUF_SIZE);

	if (pBuf == NULL)
	{
		cout << "Could not create file mapping object (%d).\n" <<
			GetLastError();
		CloseHandle(hMapFile);
		return 1;
	}

	//cout << _tcslen(processString) * sizeof(TCHAR);
	//pBuf = (char*)malloc(10000);
	DWORD size = processesVector.size();
	CopyMemory((LPVOID)pBuf, &size, sizeof(ProcessData));
	pBuf += sizeof(DWORD);
	for (int i = 0; i < processesVector.size(); i++) {
		CopyMemory((LPVOID)pBuf, &processesVector[i], sizeof(ProcessData));
		pBuf += sizeof(ProcessData);
	}
	cin.get(); 
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	return 0;

}

void printTree(DWORD pid, vector<ProcessData>processes,int count) {
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
				wcout << processes[i].exeName << " " << processes[i].pid << " " << processes[i].ppid;
				wcout << endl;
				for (int i = 0; i < count; i++) {
					cout << " ";
				}
				printTree(processes[i].pid, processes, count);
			}
		}
	
	}
}


void transformToTreeProcesses(vector<ProcessData> processes) {

	DWORD ppidCur;
	stack<DWORD> ppidQueue;
	list<DWORD>visitedPids;
	int count = 0;
	list<DWORD>allPpids;
	vector<DWORD>rootTreesPids;
	int treesCount = 1;
	
	kill_by_pid(284);

	for (int i = 0; i < processes.size(); i++) {
		allPpids.push_back(processes[i].pid);
	}
	for (int i = 0; i < processes.size(); i++) {
		if (!(find(allPpids.begin(), allPpids.end(), processes[i].ppid) != allPpids.end())) {
			wcout << processes[i].exeName << " " << processes[i].pid << " " << processes[i].ppid;
			//cout << endl;
			
			ppidQueue.push(processes[i].pid);
			rootTreesPids.push_back(processes[i].pid);
			
		}
	}

	for (int i = 0; i < rootTreesPids.size(); i++) {
		wcout << "Arborele : " << rootTreesPids[i];
		printTree(rootTreesPids[i], processes,0);
		wcout << endl << endl;
	}
	wcout << endl << endl;
	wcout << endl << endl;
	wcout << endl << endl;
	while (!ppidQueue.empty())
	{
		ppidCur = ppidQueue.top();
		ppidQueue.pop();
		visitedPids.push_back(ppidCur);
		if ((find(rootTreesPids.begin(), rootTreesPids.end(), ppidCur) != rootTreesPids.end())) {
			count = 0;
		}

			for (int i = 0; i < processes.size(); i++) {
				if (processes[i].ppid == ppidCur) {
					
					//visitedPids.push_back(processes[i].pid);
					if (count == 0) {
						cout << endl << "Arborele nr. :" << treesCount << endl;
						treesCount++;
					}
					for (int i = 0; i < count; i++) {
						cout << " ";
					}
					wcout << processes[i].exeName << " " << processes[i].pid << " " << processes[i].ppid;
					cout << endl;
					ppidQueue.push(processes[i].pid);
					count++;
					ppidQueue.pop();
					ppidCur = ppidQueue.top();
				}

			}
		
	}
}


int main()
{

	vector<ProcessData> processes = getAllProcessesAsAVector();

	//TCHAR *param = new TCHAR[string.size() + 1];
	//param[string.size()] = 0;
	//std::copy(string.begin(), string.end(), param);


	 transformToTreeProcesses(processes);
	//cout << "aici" << createMappedFile(processes);

	getchar();
	return 0;

}