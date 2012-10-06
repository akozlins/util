
#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <dbghelp.h>

#include <stdio.h>

LONG WINAPI fltr(PEXCEPTION_POINTERS ex)
{
  printf("Exception!\n");

  HANDLE file = CreateFileA("dmp.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  if(file == NULL || file == INVALID_HANDLE_VALUE) return EXCEPTION_EXECUTE_HANDLER;

  MINIDUMP_EXCEPTION_INFORMATION info;
  info.ThreadId = GetCurrentThreadId();
  info.ExceptionPointers = ex;
  info.ClientPointers = FALSE;

  MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpNormal, &info, NULL, NULL);
  CloseHandle(file);

  return EXCEPTION_EXECUTE_HANDLER;
}

void main()
{
  LPTOP_LEVEL_EXCEPTION_FILTER fltrOld = SetUnhandledExceptionFilter(fltr);

  int* p = 0;
  p[0] = 0;

  SetUnhandledExceptionFilter(fltrOld);
}
