#pragma once

//NtDriver.h
#include "stdafx.h"
#include "tchar.h"
#include "corecrt_malloc.h"

#include <Winsvc.h>
#include <winioctl.h>
#include<iostream> 
using namespace std;
#define NDER_GETLASTERROR -1
#define NDER_SUCCESS 0
#define NDER_FILE_NO_FOUND 1
#define NDER_SERVICE_NOT_OPEN 2
#define NDER_SERVICE_NOT_STARTED 3
#define NDER_SERVICE_ALREADY_OPENED 4
#define NDER_SERVICE_ALREADY_STARTED 5
#define NDER_SERVICE_IO_PENDING 6

#define DRIVER_CLOSED 0
#define DRIVER_OPENED 1
#define DRIVER_STARTED 2
#define DRIVER_PAUSED 3
#define DRIVER_BUSY 4

#define ExistFile(x) (GetFileAttributes(x)!=0xFFFFFFFF)

#define MAX_OPENED_DEVICES 0x100
#define DEVICE_LINK_NAME    L"\\\\.\\BufferedIODevcieLinkName"


#define CTL_SYS \
    CTL_CODE(FILE_DEVICE_UNKNOWN,0x830,METHOD_BUFFERED,FILE_ANY_ACCESS)
class CNtDriver {
public:
	//构造函数
	CNtDriver(LPTSTR lpszSysFilePath = NULL);
	//析构函数
	~CNtDriver();

	DWORD SetFilePath(LPTSTR lpszSysFilePath);
	DWORD Create(LPTSTR szDrvSvcName, LPTSTR szDrvDisplayName, LPTSTR filePath);
	DWORD Open(LPTSTR szDrvSvcName);
	DWORD Load();
	DWORD Unload(DWORD dwWaitMilliseconds);
	DWORD Delete();
	DWORD Close();
	DWORD GetStatus(LPTSTR driverServiceName);

	HANDLE OpenDevice(LPTSTR lpszSymLinkName);
	BOOL CloseDevice(HANDLE hDevice);
	void OperateDriver();
	string GetLastErrorToString(DWORD errorCode);
	LPCWSTR stringToLPCWSTR(string orig);
	void ShowError();
	HWND GetThisHwnd();
	inline BOOL IoControl(HANDLE hDevice, DWORD dwIoControlCode,
		LPVOID lpInBuffer, DWORD nInBufferSize,
		LPVOID lpOutBuffer, DWORD nOutBufferSize,
		LPDWORD lpBytesReturned) {
		return DeviceIoControl(
			hDevice,
			dwIoControlCode,
			lpInBuffer,
			nInBufferSize,
			lpOutBuffer,
			nOutBufferSize,
			lpBytesReturned,
			NULL
		);
	}
private:
	LPTSTR szSysFilePath;
	SC_HANDLE hService;

	SC_HANDLE hSCM;

	HANDLE hDevices[MAX_OPENED_DEVICES];
	int hDeviceN;
};

