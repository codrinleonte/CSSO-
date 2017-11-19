// tema1csso.cpp : Defines the entry point for the console application.
//
#include <tchar.h>
#include "stdafx.h"
#include "windows.h"
#include "iostream"
#pragma warning(disable: 4996)
using namespace std;

	

	int SetKeyData(HKEY hRootKey, wstring subKey, DWORD dwType, wchar_t * value, LPBYTE data, DWORD cbData)
	{
		HKEY hKey;
		if (RegOpenKeyEx(hRootKey, subKey.c_str(), NULL, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
			return -10;

		if (RegSetValueEx(hKey, value, NULL, dwType, data, cbData) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return -20;
		}

		RegCloseKey(hKey);
		return 1;
	}


	LONG CreateKey(wstring directory) {
		HKEY hkey;
		return RegCreateKeyEx(HKEY_CURRENT_USER, directory.c_str(), NULL, NULL, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL);
	}

	
	void MakeKeysAndValues(wstring root, wstring path,wstring pathDir, wstring rootPathDir) {

		WIN32_FIND_DATA lpFindFileData;
		HANDLE hFind;
		DWORD size;
		char*charPath = NULL;

		if ((hFind = FindFirstFile(path.c_str(), &lpFindFileData)) == INVALID_HANDLE_VALUE) {
			cout << "Nu gasesc fisiere";
		}
		else
		{
			FindNextFile(hFind, &lpFindFileData);

			while (FindNextFile(hFind, &lpFindFileData)) {

				if (lpFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					//cout << endl<<"Director: ";
		
					if (CreateKey(pathDir+lpFindFileData.cFileName) != ERROR_SUCCESS) {
						cout << " Something went wrong, key was not created!";
					}

				
					path.pop_back();
					path += (wstring)(lpFindFileData.cFileName) + L"\\*";
					pathDir += (wstring)(lpFindFileData.cFileName) + L"\\";
					
					MakeKeysAndValues(root,path,pathDir,rootPathDir);
					
				}
				else {
					
				    size = lpFindFileData.nFileSizeLow;
					cout<<SetKeyData(HKEY_CURRENT_USER, pathDir, REG_DWORD, lpFindFileData.cFileName, (BYTE*)(&size), sizeof(DWORD));
					//wprintf(lpFindFileData.cFileName);
					//cout << endl;
				}
				path = root;
				pathDir = rootPathDir;
			}
		}
	}

		
	int main(int argc, char *argv[])
	{

		/*
		HKEY hkey;
		LONG lError = RegCreateKeyEx(HKEY_CURRENT_USER,L"Software\\CSSO", NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey,NULL);
		if (lError != ERROR_SUCCESS) {
			cout << " Something went wrong, key was not created!";
		}
		BYTE data = 100;
		char* copiedDirectoryPath = "D:\\ProiecteAndroid";

		if (verifyIfIsADirectory(copiedDirectoryPath) == 1) {
			cout << SetKeyData(HKEY_CURRENT_USER, L"Software\\CSSO", REG_DWORD, L"fisier", &data, sizeof(DWORD));
		}

		else
			cout << "Couldn't find a Directory at the path that you provided";
	
		*/
		HKEY hkey;
		LONG lError = RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\CSSO", NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL);


		//RegDeleteTree(HKEY_CURRENT_USER, L"Software\\CSSO");

		try {
			MakeKeysAndValues(L"D:\\test\\*",L"D:\\test\\*",L"Software\\CSSO\\test\\", L"Software\\CSSO\\test\\");
		}
		catch (...) {
			cout << "Exceptie";
		}
		
		getchar();
		return 0;
	}
	
