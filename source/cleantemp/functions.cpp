#include "functions.h"
#include <iostream>

bool DirectoryExists (const char* dirName)
{
  DWORD attributes = ::GetFileAttributesA(dirName);
  return
    (
      (attributes == INVALID_FILE_ATTRIBUTES) ?
      false :
      (attributes & FILE_ATTRIBUTE_DIRECTORY)
      );
}

bool FileExists (LPCWSTR pszFilename)
{
  DWORD dwAttrib = GetFileAttributes  (pszFilename);
  return
    (
      !(dwAttrib & FILE_ATTRIBUTE_DEVICE) &&
      !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
      );
}

void WaitForEnter(std::string message)
{
  std::cout << message;
  std::cin.get ();
  std::cout << '\n';
}

LPWSTR ConvertString (const std::string & instr)
{
  int bufferlen = ::MultiByteToWideChar (CP_ACP, 0, instr.c_str (), (int)instr.size (), NULL, 0);
  if (bufferlen == 0)
  {
    return 0;
  }
  LPWSTR widestr = new WCHAR [static_cast <unsigned __int64> (bufferlen) + 1];
  ::MultiByteToWideChar (CP_ACP, 0, instr.c_str (), (int) instr.size (), widestr, bufferlen);
  widestr [bufferlen] = 0;
  return widestr;
}

bool IsOwnWindow ()
{
  HWND hwndConsole = GetConsoleWindow ();
  DWORD dwProcessId;
  GetWindowThreadProcessId (hwndConsole, & dwProcessId);
  return (GetCurrentProcessId () == dwProcessId);
}