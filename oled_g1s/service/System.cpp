
#include <sstream>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define _WIN32_DCOM
#include <comdef.h>
#include <wbemidl.h>

#include "System.h"

System::System()
{
  locator = NULL;
  services = NULL;

  memset(processorUsage, 0, 12);

  try
  {
    if(FAILED(CoInitializeEx(0, COINIT_MULTITHREADED))) throw L"CoInitializeEx";

    if(FAILED(CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL))) throw L"CoInitializeSecurity";

    if(FAILED(CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, &locator))) throw L"CoCreateInstance";

    if(FAILED(((IWbemLocator*)locator)->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, (IWbemServices**)(&services)))) throw L"ConnectServer";

    if(FAILED(CoSetProxyBlanket((IWbemServices*)services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE))) throw "CoSetProxyBlanket";

    thr.reset(new boost::thread(boost::bind(&System::run, this)));
  }
  catch(...)
  {
    if(services != 0)
    {
      ((IWbemServices*)services)->Release();
      services = 0;
    }
    if(locator != 0)
    {
      ((IWbemLocator*)locator)->Release();
      locator = 0;
    }

    CoUninitialize();

    throw;
  }
}

System::~System()
{
  {
    boost::mutex::scoped_lock sl(mt);
    if(services != 0)
    {
      ((IWbemServices*)services)->Release();
      services = 0;
    }
    if(locator != 0)
    {
      ((IWbemLocator*)locator)->Release();
      locator = 0;
    }
  }

  if(thr) thr->join();

  CoUninitialize();
}

std::wstring System::getTime() const
{
  SYSTEMTIME systemTime;
  GetLocalTime(&systemTime);

  wchar_t time[9];

  wsprintf(time, L"%02d:%02d:%02d", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

  return time;
}

unsigned __int32 System::getBatteryLife() const
{
  SYSTEM_POWER_STATUS systemPowerStatus;
  GetSystemPowerStatus(&systemPowerStatus);
  return systemPowerStatus.BatteryLifePercent;
}

unsigned __int32 System::getProcessorUsage(unsigned __int32 n) const
{
  return processorUsage[n];
}

void System::run()
{
  __int64 tp1[3];
  __int64 ts1[3];
  __int64 tp2[3];
  __int64 ts2[3];

  {
    boost::mutex::scoped_lock sl(mt);

    if(services == 0 || locator == 0) return;

    update(tp1, ts1);
  }

  while(true)
  {
    boost::mutex::scoped_lock sl(mt);

    for(unsigned __int32 i = 0; i < 5; i++)
    {
      sl.unlock();
      Sleep(100);
      sl.lock();
      if(services == 0 || locator == 0) return;
    }

    update(tp2, ts2);

    for(unsigned __int32 i = 0; i < 3; i++)
    {
      if(ts2[i] - ts1[i] != 0)
      {
        processorUsage[i] = (unsigned __int32)(100 - 100 * (tp2[i] - tp1[i]) / (ts2[i] - ts1[i]));
      }
      else processorUsage[i] = 0;
    }

    memcpy(tp1, tp2, 24);
    memcpy(ts1, ts2, 24);
  }
}

void System::update(__int64 tp[3], __int64 ts[3])
{
  IEnumWbemClassObject* objectEnum;
  IWbemClassObject *object;

  IWbemServices* services = (IWbemServices*)System::services;

  try
  {
    objectEnum = NULL;
    if(FAILED(services->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_PerfRawData_PerfOS_Processor"), WBEM_FLAG_FORWARD_ONLY, NULL, &objectEnum))) throw L"ExecQuery";

    for(unsigned int i = 0; i < 3; i++)
    {
      ULONG result = 0;
      object = NULL;
      if(FAILED(objectEnum->Next(WBEM_INFINITE, 1, &object, &result))) throw L"Next";
      if(object == 0 || result == 0) break;

      VARIANT variant;
      VariantInit(&variant);

      if(FAILED(object->Get(L"PercentProcessorTime", 0, &variant, 0, 0))) throw L"Get";
      tp[i] = _wtoi64(variant.bstrVal);
      VariantClear(&variant);

      if(FAILED(object->Get(L"TimeStamp_Sys100NS", 0, &variant, 0, 0))) throw L"Get";
      ts[i] = _wtoi64(variant.bstrVal);
      VariantClear(&variant);

      if(object != 0)
      {
        object->Release();
        object = 0;
      }
    }

    if(objectEnum != 0)
    {
      objectEnum->Release();
      objectEnum = 0;
    }
  }
  catch(...)
  {
    if(object != 0)
    {
      object->Release();
      object = 0;
    }

    if(objectEnum != 0)
    {
      objectEnum->Release();
      objectEnum = 0;
    }
  }
}
