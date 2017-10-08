﻿#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <windows.h>
#include <fstream>
#include <io.h>
#include <TCHAR.H>

using namespace std;
int num = 0;
int ibs = 512;
int obs = 1024;
const int N = 256;
char *FName = "help"; 
DWORD dwCreation = CREATE_NEW;
DWORD dwCreationAccess = GENERIC_WRITE;


LPWSTR CharToLPWSTR(LPCSTR char_string)
{
	LPWSTR res;
	DWORD res_len = MultiByteToWideChar(1251, 0, char_string, -1, NULL, 0);
	res = (LPWSTR)GlobalAlloc(GPTR, (res_len + 1) * sizeof(WCHAR));
	MultiByteToWideChar(1251, 0, char_string, -1, res, res_len);
	return res;
}
void ReadHelp()
{
	char S[N] = { "" }; //В S будут считываться строки
	ifstream in1(FName); //Открыли файл для чтения
	if (!in1.is_open()) {// если файл не открыт
		cout << "File will not open!\n"; // сообщить об этом
		return;
	}
	while (!in1.eof()) //Будем читать информацию пока не дойдем до конца файла
	{
		in1.getline(S, N); //Построчное считывание информации в S

		cout << S << endl; //Вывод очередной строки на экран
	}
	in1.close();  //Закрыли открытый файл
	system("pause");
}
void Helper()
{
	cout << "Example: mv [options...] source_file file_destanation\n" <<
		"mv[options...] source_file... catalog" << endl;
	system("Pause");
}
void Help()
{
	ReadHelp();
}
bool FileExists(LPCTSTR fname)
{
	WIN32_FIND_DATA wfd;
	HANDLE hFind = ::FindFirstFile(fname, &wfd);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		// Если этого не сделать то произойдет утечка ресурсов
		::FindClose(hFind);
		return true;
	}
	return false;
}
void Writer(char **mass, LPCTSTR fileDest, DWORD FileSize, DWORD FileIter, int tempIBS) {
	DWORD  dwTemp, FileIterWr,nOBS = obs; int temp;
	if (FileExists(fileDest))
		DeleteFile(fileDest);
	HANDLE hFile = CreateFile(fileDest, dwCreationAccess, 0, NULL,
		dwCreation, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile) {
		return;
	}
	if ((FileSize / obs) > 0) {
		temp = FileSize%obs;
		FileIterWr = (FileSize / obs) + 1;
	}
	else FileIterWr = 1;
	char** masWr = new char*[FileIterWr];
	for (int i = 0; i < FileIterWr; i++) {
		if (i == FileIterWr - 1)
			masWr[i] = new char[temp];
		else masWr[i] = new char[obs];
	}
	int tm = 0,z =0;
	for (int i = 0; i < FileIter-1; i++) {
		for (int j = 0; j < ibs; j++) {
				if (tm < obs) {
					masWr[z][tm] = mass[i][j];
					tm++;
				}
				else {
					z++;
					masWr[z][tm] = mass[i][j];
					tm = 0;
				}
		}
	}
	for (int j = 0; j <= tempIBS; j++) {
		if (tm < obs) {
			masWr[z][tm] = mass[FileIter - 1][j];
			tm++;
		}
		else {
			z++;
			tm = 0;
			masWr[z][tm] = mass[FileIter - 1][j];

		}
	}
	for (int i = 0; i < FileIterWr; i++) {
		if (i == FileIterWr - 1)
			nOBS = temp-3;
		WriteFile(hFile, masWr[i], nOBS, &dwTemp, NULL);
	}
	CloseHandle(hFile);
}

void ReadFile(LPCTSTR file, LPCTSTR fileDest) {
	int i, str; int temp;
	DWORD  dwTemp, FileSize, FileIter, nIBS=ibs;
	HANDLE fin = CreateFile(file, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	FileSize = GetFileSize(fin, NULL);
	if ((FileSize / ibs) > 0) {
		temp = (FileSize%ibs);
		FileIter = (FileSize / ibs)+1;
	}
	else FileIter = 1;
	char** mass = new char*[FileIter];
	for (int i = 0; i < FileIter; i++) {
		if(i== FileIter-1)
			mass[i] = new char[temp];
		else mass[i] = new char[ibs];
	}
	for (int i = 0; i < FileIter; i++) {
		if (i == FileIter - 1) {
			nIBS = temp;
			ReadFile(fin, mass[i], nIBS, &dwTemp, NULL);
			mass[i][temp] = '\0';
		}
		else ReadFile(fin, mass[i], nIBS, &dwTemp, NULL);
	}
	Writer(mass, fileDest, FileSize, FileIter, temp);
	CloseHandle(fin);
}

void StartValue(char *value, int nextValue) {
	if(strcmp(value,"-ibs")==0)
		ibs = nextValue;
	if (strcmp(value, "-obs") == 0)
		obs = nextValue;
	if (strcmp(value, "-bs") == 0){ ibs = nextValue; obs = nextValue; }
}

int main(int argc, char *argv[]) {
	setlocale(LC_ALL, "Russian");
	
	if (argc > 1) {
		
		if (strcmp(argv[1], "--help") == 0) {
			Help();
		}
		for (int i = 1; i < argc - 2; i++) {
			int valueI = 0, valueZ = 0, nextValueI = 0, nextValueZ = 0;
			char *value, *nextValue; bool go = false; bool goNext = false;
			const char *oldname = argv[i];
			for (int j = 0; j < strlen(oldname); j++) {
				if (argv[i][j] == '=') break;
				else valueI++;
			}
			for (int j = 0; j < strlen(oldname); j++) {
				if (argv[i][j] == '=') goNext = true;
				if (goNext) nextValueI++;
			}
			value = new char[valueI];
			nextValue = new char[nextValueI];
			for (int j = 0; j < strlen(oldname); j++) {
				if (argv[i][j] == '=') break;
				else { value[valueZ] = argv[i][j]; valueZ++; }
			}
			for (int j = valueZ; j < strlen(oldname); j++) {
				if (argv[i][j] == '=')
					go = true;
				else if (go) {nextValue[nextValueZ] = argv[i][j]; nextValueZ++; }
			}
			value[valueZ] = '\0'; nextValue[nextValueZ + 1] = '\0';
			StartValue(value, atoi(nextValue));
			value = NULL; nextValue = NULL;
		}
	}
	ReadFile(CharToLPWSTR(argv[argc - 2]), CharToLPWSTR(argv[argc - 1]));
	/*
	if (argc >= 4) {
		//cout << argv[argc - 3][1];
		if (argv[argc - 3][1] == 'T') {
			DestType = 0;
			if (check(CharToLPWSTR(argv[argc - 2]))) {
				cout << "file not exist\n or file not a dir";
				return 0;
			}
			GoNext(argv[argc - 4], CharToLPWSTR(argv[argc - 2]), CharToLPWSTR(argv[argc - 1]));
		}
		else if (argv[argc - 3][1] == 't') {
			DestType = 1;
			if (check(CharToLPWSTR(argv[argc - 2])) == true) {
				cout << "file not exist\n or file not a dir";
				return 0;
			}
			GoNext(argv[argc - 4], CharToLPWSTR(argv[argc - 2]), CharToLPWSTR(argv[argc - 1]));
		}
		else GoNext(argv[argc - 3], CharToLPWSTR(argv[argc - 2]), CharToLPWSTR(argv[argc - 1]));
	}
	*/
	if (argc == 1) {
		Helper();
		//choose('-v', "E:\\help.txt", "E:\\help1.txt"); //debug
	}
	
}