// DriverLibrary.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "NtDriver.h"
#include "Common.h"
 static CNtDriver cs;

extern "C" _declspec(dllexport) int __stdcall add(int a,int b) {
	return a+b;
}
extern "C"  _declspec(dllexport) DWORD __stdcall Create(LPTSTR szDrvSvcName, LPTSTR szDrvDisplayName, LPTSTR filePath) {
	//CNtDriver cs;
	return cs.Create(szDrvSvcName, szDrvDisplayName, filePath);
	/*DWORD test = 0x0001;
	return test;*/
}

extern "C"  _declspec(dllexport) DWORD __stdcall GetStatus(LPTSTR driverServiceName) {
	//CNtDriver cs;
	return cs.GetStatus(driverServiceName);
}

extern "C"  _declspec(dllexport) DWORD __stdcall Load() {
	//CNtDriver cs;
	return cs.Load();
}

extern "C"  _declspec(dllexport) DWORD __stdcall Unload(DWORD dwWaitMilliseconds) {
	//CNtDriver cs;
	return cs.Unload(dwWaitMilliseconds);
}

extern "C"  _declspec(dllexport) DWORD __stdcall Delete() {
	//CNtDriver cs;
	return cs.Delete();
}

extern "C"  _declspec(dllexport) DWORD __stdcall Open(LPTSTR szDrvSvcName) {
	//CNtDriver cs;
	return cs.Open(szDrvSvcName);
}

extern "C"  _declspec(dllexport) DWORD __stdcall Close() {
	//CNtDriver cs;
	return cs.Close();
}

extern "C"  _declspec(dllexport) void __stdcall OperateDriver() {
	//CNtDriver cs;
	return cs.OperateDriver();
}

extern "C"  _declspec(dllexport) BOOL __stdcall IsRunasAdmin() {
	return _IsRunasAdmin();
}