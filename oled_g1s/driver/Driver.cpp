
#include "Driver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <setupapi.h>

#include <assert.h>

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID)
{
  return TRUE;
}

DriverG1S::DriverG1S(): handle(INVALID_HANDLE_VALUE), frame1(0), frame2(0)
{
  frame1 = new unsigned __int8[17 + 256];
  frame2 = new unsigned __int8[17 + 256];

  static const unsigned __int8 header[] =
  {
    0x00, 0x55, 0xAA,
    0x10 /*0x11*/,
    0x80, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  memcpy(frame1, header, 17);
  frame1[3] = 0x10;
  memset(frame1 + 17, 0xFF, 256);

  memcpy(frame2, header, 17);
  frame2[3] = 0x11;
  memset(frame2 + 17, 0xFF, 256);
}

DriverG1S::~DriverG1S()
{
  if(handle != INVALID_HANDLE_VALUE) CloseHandle(handle);

  if(frame1 != 0) delete[] frame1;
  if(frame2 != 0) delete[] frame2;
}

unsigned __int32 DriverG1S::getWidth() const
{
  return 128;
}

unsigned __int32 DriverG1S::getHeight() const
{
  return 32;
}

void DriverG1S::init()
{
  static const unsigned __int8 guid[] =
  {
    0xB2, 0x55, 0x1E, 0x4D,
    0x6F, 0xF1,
    0xCF, 0x11,
    0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30
  };

  HDEVINFO deviceInfoSet = SetupDiGetClassDevs((GUID*)&guid, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
  if(deviceInfoSet == INVALID_HANDLE_VALUE) return;

  SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
  deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

  for(DWORD memberIndex = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, 0, (GUID*)&guid, memberIndex, &deviceInterfaceData); memberIndex++)
  {
    DWORD requiredSize = 0;
    SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, 0, 0, &requiredSize, 0);

    PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)(operator new (requiredSize));
    deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    if(SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, deviceInterfaceDetailData, requiredSize, &requiredSize, 0))
    {
      if(_wcsnicmp(deviceInterfaceDetailData->DevicePath, L"\\\\?\\hid#vid_0b05&pid_1726", 25) == 0)
      {
        handle = CreateFile(deviceInterfaceDetailData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
      }
    }

    delete deviceInterfaceDetailData;

    if(handle != INVALID_HANDLE_VALUE) break;
  }

  SetupDiDestroyDeviceInfoList(deviceInfoSet);
}

void DriverG1S::flush()
{
  if(handle == INVALID_HANDLE_VALUE) throw L"handle == INVALID_HANDLE_VALUE";

  DWORD bytesWriten = 0;

  if(!WriteFile(handle, frame1, 17 + 256, &bytesWriten, 0) || bytesWriten != 17 + 256) throw L"bytesWriten != 17(header) + 256(frame)";
  if(!WriteFile(handle, frame2, 17 + 256, &bytesWriten, 0) || bytesWriten != 17 + 256) throw L"bytesWriten != 17(header) + 256(frame)";
}

void DriverG1S::write(const unsigned __int8* buffer, unsigned __int32 length)
{
  if(handle == INVALID_HANDLE_VALUE) throw L"handle == INVALID_HANDLE_VALUE";
  if(length != 4096) throw L"length != 4096";

  const unsigned __int8* buffer1_ = buffer + 2048;
  const unsigned __int8* buffer2_ = buffer + 4096;

  for(unsigned __int32 y = 16; y > 0; --y)
  {
    unsigned __int8* frame1_ = frame1 + 17 + 128 * (y > 8);
    unsigned __int8* frame2_ = frame2 + 17 + 128 * (y > 8);
    for(unsigned __int32 x = 0; x < 128; ++x)
    {
      assert(buffer1_ > buffer);
      assert(buffer2_ > buffer + 2048);
      assert(frame1_ < frame1 + 17 + 256);
      assert(frame2_ < frame2 + 17 + 256);
      (*(frame1_++) <<= 1) |= *(--buffer1_) == 0;
      (*(frame2_++) <<= 1) |= *(--buffer2_) == 0;
    }
  }
}
