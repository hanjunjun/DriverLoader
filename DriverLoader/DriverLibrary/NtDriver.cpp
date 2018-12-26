
//NtDriver.cpp
#include "NtDriver.h"

CNtDriver::CNtDriver(LPTSTR lpszSysFilePath) {
	szSysFilePath = lpszSysFilePath;
	hSCM = NULL;
	hService = NULL;
	hDeviceN = 0;
}

CNtDriver::~CNtDriver() {
	//检查驱动是否已停止
	SERVICE_STATUS ss;

	if (hDeviceN) {
		__try {
			for (int i = 0; i<hDeviceN; i++)
				CloseHandle(hDevices[i]);
		}
		__except (EXCEPTION_CONTINUE_EXECUTION) {
		}
		hDeviceN = 0;
	}

	if (hService) {
		if (!QueryServiceStatus(hService, &ss))
			MessageBox(GetThisHwnd(), _T("无法获取驱动状态！"), _T("警告"), MB_OK | MB_ICONSTOP);
		if (ss.dwCurrentState != SERVICE_STOPPED) {
			if (!ControlService(hService, SERVICE_CONTROL_STOP, &ss)) {
				MessageBox(GetThisHwnd(), _T("无法停止驱动！"), _T("警告"), MB_OK | MB_ICONSTOP);
			}
		}
		if (!DeleteService(hService))
			MessageBox(GetThisHwnd(), _T("无法删除驱动！"), _T("警告"), MB_OK | MB_ICONSTOP);
		CloseServiceHandle(hService);
		hService = 0;
	}
	if (hSCM) {
		CloseServiceHandle(hSCM);
	}
}

//设置驱动文件路径
DWORD CNtDriver::SetFilePath(LPTSTR lpszSysFilePath) {
	//检查驱动状态（必须是关闭状态）
	if (hService)
		return NDER_SERVICE_ALREADY_OPENED;

	szSysFilePath = lpszSysFilePath;
	return ExistFile(szSysFilePath) ? NDER_SUCCESS : NDER_FILE_NO_FOUND;
}

//创建驱动设备
DWORD CNtDriver::Create(LPTSTR szDrvSvcName, LPTSTR szDrvDisplayName, LPTSTR filePath) {
	//(_T("Create service: %s\n"), szDrvSvcName);
	//检查驱动状态（必须是关闭状态）
	if (hService)
		return NDER_SERVICE_ALREADY_OPENED;

	//检查文件是否存在
	if (!ExistFile(filePath))
		return NDER_FILE_NO_FOUND;

	//打开驱动控制管理器
	if (!hSCM)
		hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCM)
		return NDER_GETLASTERROR;

	//创建驱动所对应的驱动
	hService = CreateService(
		hSCM,
		szDrvSvcName,//驱动名称
		szDrvDisplayName,//驱动显示名称
		SERVICE_ALL_ACCESS,//权限（所有权限）
		SERVICE_KERNEL_DRIVER,//驱动类型（内核驱动）
		SERVICE_DEMAND_START,//启动类型（手动）
		SERVICE_ERROR_IGNORE,//忽略错误
		filePath,//驱动文件路径
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	);
	if (!hService) {
		hService = OpenService(hSCM, szDrvSvcName, SERVICE_ALL_ACCESS);
		if (!hService)
			return NDER_GETLASTERROR;
	}

	//(_T("Create service successful.\n"));
	return NDER_SUCCESS;
}

//打开驱动设备
DWORD CNtDriver::Open(LPTSTR szDrvSvcName) {
	//(_T("Open service: %s\n"), szDrvSvcName);
	//检查驱动状态（必须是关闭状态）
	if (hService)
		return NDER_SERVICE_ALREADY_OPENED;

	//打开驱动控制管理器
	if (!hSCM)
		hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCM)
		return NDER_GETLASTERROR;

	//打开驱动
	hService = OpenService(hSCM, szDrvSvcName, SERVICE_ALL_ACCESS);
	if (!hService)
		return NDER_GETLASTERROR;

	//获取驱动类型
	LPQUERY_SERVICE_CONFIG pqsc;
	DWORD dwNeedSize = 0;
	DWORD dwServiceType=NULL;
	QueryServiceConfig(hService, NULL, 0, &dwNeedSize);
	if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
		pqsc = (LPQUERY_SERVICE_CONFIG)malloc(dwNeedSize);
		if (QueryServiceConfig(hService, pqsc, dwNeedSize, &dwNeedSize)) {
			dwServiceType = pqsc->dwServiceType;
			free(pqsc);
		}
		else
			return NDER_GETLASTERROR;
	}
	else
		return NDER_GETLASTERROR;

	//检查驱动类型
	if (dwServiceType != SERVICE_KERNEL_DRIVER) {
		CloseServiceHandle(hService);
		hService = 0;
		SetLastError(ERROR_BAD_DRIVER);
		return NDER_GETLASTERROR;
	}

	//(_T("Open service successful.\n"));
	return NDER_SUCCESS;
}

//加载驱动
DWORD CNtDriver::Load() {
	//(_T("Start service..\n"));
	//检查驱动是否已打开
	if (!hService)
		return NDER_SERVICE_NOT_OPEN;

	SERVICE_STATUS ss;
	if (!QueryServiceStatus(hService, &ss))
		return NDER_GETLASTERROR;
	if (ss.dwCurrentState != SERVICE_STOPPED)
		return NDER_SERVICE_ALREADY_STARTED;

	//启动驱动
	if (!StartService(hService, NULL, NULL)) {
		DWORD dwErr = GetLastError();
		if (dwErr == ERROR_IO_PENDING)
			return NDER_SERVICE_IO_PENDING;
		else if (dwErr == ERROR_SERVICE_ALREADY_RUNNING)
			return NDER_SERVICE_ALREADY_STARTED;
		else
			return NDER_GETLASTERROR;
	}

	//(_T("Start service successful.\n"));
	return NDER_SUCCESS;
}

//获取驱动状态
DWORD CNtDriver::GetStatus(LPTSTR driverServiceName) {
	//(_T("GetStatus service..\n"));
	//检查驱动是否已打开
	//打开驱动
	/*if (!hSCM)
		hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCM)
		return NDER_GETLASTERROR;

	hService = OpenService(hSCM, driverServiceName, SERVICE_ALL_ACCESS);
	if (!hService)
		return DRIVER_CLOSED;*/

	if (!hService)
		return DRIVER_CLOSED;

	SERVICE_STATUS ss;
	if (!QueryServiceStatus(hService, &ss))
		return NDER_GETLASTERROR;

	switch (ss.dwCurrentState) {
	case SERVICE_RUNNING:
		return DRIVER_STARTED;
	case SERVICE_STOPPED:
		return DRIVER_OPENED;
	case SERVICE_PAUSED:
		return DRIVER_PAUSED;
	default:
		return DRIVER_BUSY;
	}
}

//卸载驱动
DWORD CNtDriver::Unload(DWORD dwWaitMilliseconds = 1) {
	//(_T("Unload service..\n"));
	//检查驱动是否已打开
	if (!hService)
		return NDER_SERVICE_NOT_OPEN;

	//检查驱动是否已停止
	SERVICE_STATUS ss;
	if (!QueryServiceStatus(hService, &ss))
		return NDER_GETLASTERROR;
	if (ss.dwCurrentState == SERVICE_STOPPED)
		return NDER_SERVICE_NOT_STARTED;

	if (!ControlService(hService, SERVICE_CONTROL_STOP, &ss))
		return NDER_GETLASTERROR;

	DWORD dwStopTick = GetTickCount();
	while (GetTickCount()<dwStopTick + dwWaitMilliseconds) {
		Sleep(1);
		if (!QueryServiceStatus(hService, &ss))
			return NDER_GETLASTERROR;
		switch (ss.dwCurrentState) {
		case SERVICE_STOPPED:
			return NDER_SUCCESS;
		case SERVICE_STOP_PENDING:
			break;
		default:
			return NDER_SERVICE_IO_PENDING;
		}
	}

	//(_T("Unload service successful.\n"));
	return NDER_SUCCESS;
}

DWORD CNtDriver::Close() {
	//(_T("Close service..\n"));
	//检查驱动是否已打开
	if (!hService)
		return NDER_SERVICE_NOT_OPEN;

	//关闭驱动句柄
	if (!CloseServiceHandle(hService))
		return NDER_GETLASTERROR;
	hService = 0;

	//(_T("Close service successful.\n"));
	return NDER_SUCCESS;
}

DWORD CNtDriver::Delete() {
	//(_T("Delete service..\n"));
	//检查驱动是否已打开
	if (!hService)
		return NDER_SERVICE_NOT_OPEN;

	//检查驱动是否已停止
	SERVICE_STATUS ss;
	if (!QueryServiceStatus(hService, &ss))
		return NDER_GETLASTERROR;
	if (ss.dwCurrentState != SERVICE_STOPPED)
		return NDER_SERVICE_ALREADY_STARTED;

	//删除驱动
	if (!DeleteService(hService))
		return NDER_GETLASTERROR;

	//关闭驱动句柄
	if (!CloseServiceHandle(hService))
		return NDER_GETLASTERROR;
	hService = 0;

	//关闭SCM句柄
	if (hSCM) {
		if (!CloseServiceHandle(hSCM))
			return NDER_GETLASTERROR;
		hSCM = 0;
	}

	//(_T("Delete service successful.\n"));
	return NDER_SUCCESS;
}

HANDLE CNtDriver::OpenDevice(LPTSTR lpszSymLinkName) {
	//检查驱动是否已打开
	if (!hService)
		return 0;

	//检查驱动是否正常运行
	SERVICE_STATUS ss;
	if (!QueryServiceStatus(hService, &ss))
		return 0;
	if (ss.dwCurrentState != SERVICE_RUNNING)
		return 0;

	HANDLE hDevice;
	hDevice = CreateFile(
		lpszSymLinkName,
		GENERIC_READ | GENERIC_WRITE,
		0,	//非共享
		NULL,	//安全描述符
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL	//没有模板
	);
	if (hDevice >= 0)
		hDevices[hDeviceN++] = hDevice;
	return hDevice;
}

BOOL CNtDriver::CloseDevice(HANDLE hDevice) {
	int i;
	if (CloseHandle(hDevice)) {
		for (i = 0; i<hDeviceN; i++) {
			if (hDevice == hDevices[i]) {
				hDevices[i] = hDevices[--hDeviceN];
			}
		}
		return TRUE;
	}
	else
		return FALSE;
}

void CNtDriver::OperateDriver() {
	//检查驱动是否已打开
	if (!hService) {
		MessageBox(GetActiveWindow(), _T("驱动没有安装！"), _T("警告"), MB_OK | MB_ICONSTOP);
		return;
	}
	//检查驱动是否正常运行
	SERVICE_STATUS ss;
	if (!QueryServiceStatus(hService, &ss))
	{
		MessageBox(GetActiveWindow(), _T("获取驱动状态失败！"), _T("警告"), MB_OK | MB_ICONSTOP);
		return;
	}
	if (ss.dwCurrentState != SERVICE_RUNNING)
	{
		MessageBox(GetActiveWindow(), _T("驱动没有运行！"), _T("警告"), MB_OK | MB_ICONSTOP);
		return;
	}

	HANDLE DeviceHandle;
	DeviceHandle = CreateFile(
		DEVICE_LINK_NAME,
		GENERIC_READ | GENERIC_WRITE,
		0,	//非共享
		NULL,	//安全描述符
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL	//没有模板
	);
	if (DeviceHandle == INVALID_HANDLE_VALUE)
	{
		MessageBox(GetActiveWindow(), _T("创建驱动句柄失败！"), _T("警告"), MB_OK | MB_ICONSTOP);
		return;
	}
	char BufferData = NULL;
	DWORD ReturnLength = 0;
	BOOL IsOk = DeviceIoControl(DeviceHandle, CTL_SYS,
		"Ring3->Ring0",//输入数据
		strlen("Ring3->Ring0") + 1,//输入数据长度
		(LPVOID)BufferData,//输出数据
		0,//输出数据长度
		&ReturnLength,
		NULL);
	if (IsOk == FALSE)
	{
		int LastError = GetLastError();

		if (LastError == ERROR_NO_SYSTEM_RESOURCES)
		{
			char BufferData[MAX_PATH] = { 0 };
			IsOk = DeviceIoControl(DeviceHandle, CTL_SYS,
				"Ring3->Ring0",//输入数据
				strlen("Ring3->Ring0") + 1,//输入数据长度
				(LPVOID)BufferData,//输出数据
				MAX_PATH,//输出数据长度
				&ReturnLength,
				NULL);

			if (IsOk == TRUE)
			{
				Sleep(2000);
				OutputDebugStringA((LPCSTR)BufferData);
				//MessageBox(GetActiveWindow(), (LPTSTR)BufferData, _T("警告"), MB_OK | MB_ICONSTOP);
			}
		}
	}
	if (DeviceHandle != NULL)
	{
		CloseHandle(DeviceHandle);
		DeviceHandle = NULL;
	}
}

string CNtDriver::GetLastErrorToString(DWORD errorCode)
{
	//因为FORMAT_MESSAGE_ALLOCATE_BUFFER标志，这个函数帮你分配内存，所以需要LocalFree来释放
	char *text;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&text, 0, NULL);
	string result(text);    //结果
	LocalFree(text);
	//return stringToLPCWSTR(result);
	return result;
}

LPCWSTR CNtDriver::stringToLPCWSTR(string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}

void CNtDriver::ShowError() {
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);
	MessageBox(GetActiveWindow(), (LPTSTR)lpMsgBuf, _T("警告"), MB_OK | MB_ICONSTOP);
	LocalFree(lpMsgBuf);
}

HWND CNtDriver:: GetThisHwnd() {
	return GetActiveWindow();
}

