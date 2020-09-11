
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void main(void)
{
  std::cout << "CreateEvent(...)" << std::endl;
  HANDLE hevent = CreateEvent(NULL, TRUE, FALSE, NULL);

  std::cout << "RegNotifyChangeKeyValue(...)" << std::endl;
  long status = RegNotifyChangeKeyValue(
    HKEY_CURRENT_USER, TRUE,
    REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_ATTRIBUTES | REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_SECURITY,
    hevent, TRUE);
  if(status != ERROR_SUCCESS)
  {
    std::cout << "RegNotifyChangeKeyValue(...) == WAIT_FAILED" << std::endl;
    return;
  }

  std::cout << "WaitForSingleObject(...)" << std::endl;
  if(WaitForSingleObject(hevent, INFINITE) == WAIT_FAILED)
  {
    std::cout << "WaitForSingleObject(...) == WAIT_FAILED" << std::endl;
    return;
  }

  std::cout << "CloseHandle(...)" << std::endl;
  CloseHandle(hevent);
}
