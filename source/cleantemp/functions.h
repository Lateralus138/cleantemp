#pragma once
#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <Windows.h>
#include <string>
bool DirectoryExists (const char* dirName);
bool FileExists (LPCWSTR pszFilename);
void WaitForEnter (std::string message);
LPWSTR ConvertString (const std::string & instr);
bool IsOwnWindow ();
#endif // !FUNCTIONS_H