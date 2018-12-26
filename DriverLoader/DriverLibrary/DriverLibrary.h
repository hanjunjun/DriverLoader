#pragma once
#include "stdafx.h"
#include "NtDriver.h"

extern "C"  _declspec(dllexport) int __stdcall add(int a, int b);

extern "C"  _declspec(dllexport) DWORD __stdcall Create(LPTSTR szDrvSvcName, LPTSTR szDrvDisplayName);

extern "C"  _declspec(dllexport) DWORD __stdcall GetStatus();

extern "C"  _declspec(dllexport) DWORD __stdcall Load();

extern "C"  _declspec(dllexport) DWORD __stdcall Unload(DWORD dwWaitMilliseconds);

extern "C"  _declspec(dllexport) DWORD __stdcall Delete();

extern "C"  _declspec(dllexport) DWORD __stdcall Open(LPTSTR szDrvSvcName);

extern "C"  _declspec(dllexport) DWORD __stdcall Close();

extern "C"  _declspec(dllexport) void __stdcall OperateDriver();

extern "C"  _declspec(dllexport) BOOL __stdcall IsRunasAdmin();