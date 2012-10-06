
#include "System.h"

#include "../graphics/Graphics.h"

#include <windows.h>

class Service
{
private:
  bool stopFlag;
  bool pauseFlag;

  boost::scoped_ptr<boost::thread> thr;
private:
  void run()
  {
    try
    {
      wchar_t processorUsage[6];
      wchar_t batteryLife[6];

      System system;
      GraphicsPtr graphics(new GraphicsG1S());

      while(true)
      {
        graphics->clear();

        int x, y;

        x = 35; y = 18;
        graphics->drawRect(x, y, 124, y + 6);
        if(system.getProcessorUsage(0) != 0) graphics->fillRect(x + 2, y + 2, x + 2 + (124 - x - 4) * system.getProcessorUsage(0) / 100, y + 2);
        if(system.getProcessorUsage(1) != 0) graphics->fillRect(x + 2, y + 4, x + 2 + (124 - x - 4) * system.getProcessorUsage(1) / 100, y + 4);

        x = 50, y = 3;
        graphics->drawRect(x, y, 85, y + 12); graphics->fillRect(86, y + 3, 89, y + 9);
        if(system.getBatteryLife() != 0) graphics->fillRect(x + 2, y + 2, x + 2 + (85 - x - 4) * system.getBatteryLife() / 100, y + 10);

        graphics->drawString(system.getTime().c_str(), 3, 3);
        wsprintf(processorUsage, L"%d %%", system.getProcessorUsage(2));
        graphics->drawString(processorUsage, 3, 15);
        wsprintf(batteryLife, L"%d %%", system.getBatteryLife());
        graphics->drawString(batteryLife, 95, 3);

        graphics->flush();

        for(int i = 0; i < 1; i++)
        {
          while(pauseFlag)
          {
            Sleep(100);
            if(i != -1)
            {
              graphics->clear();
              graphics->flush();
              i = -1;
            }
          }
          if(i == -1) break;

          if(stopFlag) return;

          Sleep(100);
        }
      }
    }
    catch(...)
    {
    }
  }
public:
  Service(): stopFlag(false), pauseFlag(false)
  {
  }

  void start()
  {
    stopFlag = false;
    pauseFlag = false;

    thr.reset(new boost::thread(boost::bind(&Service::run, this)));
  }

  void stop()
  {
    stopFlag = true;
    pauseFlag = false;
  }

  void pause()
  {
    if(!stopFlag) pauseFlag = true;
  }

  void resume()
  {
    pauseFlag = false;
  }

  void join()
  {
    if(thr)
    {
      thr->join();
      thr.reset();
    }
  }

  boolean isStopped()
  {
    return stopFlag;
  }
};

Service service;

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle;

void install()
{
  wchar_t path[256];
  DWORD size = GetModuleFileName(NULL, path, 256);

  wchar_t* name = L"G1S OLED service";

  SC_HANDLE manager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
  if(manager == 0)
  {
    MessageBox(0, L"OpenSCManager failed.", L"Error", MB_OK);
    return;
  }

  SC_HANDLE service = CreateService(manager, name, name, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, path, NULL, NULL, NULL, NULL, NULL);

  if(service == 0)
  {
    MessageBox(0, L"CreateService failed.", L"Error", MB_OK);
  }
  else
  {
    CloseServiceHandle(service);
  }

  CloseServiceHandle(manager);
}

void uninstall()
{
  wchar_t* name = L"G1S OLED service";

  SC_HANDLE manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

  if(manager ==0)
  {
    MessageBox(0, L"OpenSCManager failed.", L"Error", MB_OK);
    return;
  }

  SC_HANDLE service = OpenService(manager, name, SERVICE_ALL_ACCESS);
  if(service == 0)
  {
    MessageBox(0, L"OpenService failed.", L"Error", MB_OK);
  }
  else
  {
    if(!DeleteService(service))
    {
      MessageBox(0, L"DeleteService failed.", L"Error", MB_OK);
    }
    CloseServiceHandle(service);
  }

  CloseServiceHandle(manager);
}

BOOL reportStatus(DWORD currentState)
{
  if(currentState == SERVICE_START_PENDING) serviceStatus.dwControlsAccepted = 0;
  else serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;

  serviceStatus.dwCurrentState = currentState;
  serviceStatus.dwWin32ExitCode = NO_ERROR;
  serviceStatus.dwWaitHint = 100;

  if(currentState == SERVICE_RUNNING || currentState == SERVICE_STOPPED) serviceStatus.dwCheckPoint = 0;
  else serviceStatus.dwCheckPoint++;

  return SetServiceStatus(serviceStatusHandle, &serviceStatus);
}

void WINAPI serviceControl(DWORD controlCode)
{
  if(controlCode == SERVICE_CONTROL_STOP || controlCode == SERVICE_CONTROL_SHUTDOWN)
  {
    reportStatus(SERVICE_STOP_PENDING);
    service.stop();
  }
  if(controlCode == SERVICE_CONTROL_PAUSE)
  {
    service.pause();
    reportStatus(SERVICE_PAUSED);
  }
  if(controlCode == SERVICE_CONTROL_CONTINUE)
  {
    service.resume();
    reportStatus(SERVICE_RUNNING);
  }
}

void WINAPI serviceMain(DWORD argc, LPTSTR *argv)
{
  serviceStatusHandle = RegisterServiceCtrlHandler(L"", serviceControl);

  try
  {
    if(!serviceStatusHandle) throw "";

    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwServiceSpecificExitCode = 0;

    if(!reportStatus(SERVICE_START_PENDING)) throw "";

    if(!reportStatus(SERVICE_RUNNING)) throw "";

    while(true)
    {
      service.start();
      service.join();
      if(service.isStopped()) break;
      Sleep(1000);
    }
  }
  catch(...)
  {
  }

  if(serviceStatusHandle) reportStatus(SERVICE_STOPPED);
  return;
}

void _cdecl wmain(int argc, wchar_t **argv)
{
  if(argc == 2 && _wcsicmp(L"-t", argv[1]) == 0)
  {
    service.start();

    Sleep(60000);

    service.stop();
    service.join();

    return;
  }

  if(argc == 2 && _wcsicmp(L"-i", argv[1]) == 0)
  {
    install();

    return;
  }

  if(argc == 2 && _wcsicmp(L"-u", argv[1]) == 0)
  {
    uninstall();

    return;
  }

  SERVICE_TABLE_ENTRY dispatchTable[] = { { L"", (LPSERVICE_MAIN_FUNCTION)serviceMain }, { NULL, NULL } };
  StartServiceCtrlDispatcher(dispatchTable);
}
