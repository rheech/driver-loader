//************************************
// File:     DynamicDriverLoader.h
// Version:  1.1
// Date:     2012-12-15
// Author:   Cheong H Lee
// Contains: Driver Loader
//************************************

//************************************
// Revision History
// 2012-12-18   Convert CString to TCHAR String.
//              document not modified yet. (1.1)
//************************************

#pragma once

#include <Winsvc.h>
#pragma comment(lib, "Advapi32.lib")

#include "DriverLoadException.h"

typedef void (*CB_DBGPRINT)(LPTSTR);

class DynamicDriverLoader
//************************************
// Class:     DynamicDriverLoader
// Purpose:   To implement load/unload driver
//            on Driver Development
// Members:
//************************************
{
	enum DRIVER_LOAD_STATUS
	{
		DRIVER_UNINSTALLED = 0x0,
		DRIVER_INSTALLED = 0x1,
		DRIVER_STOPPED = 0x1,
		DRIVER_STARTED = 0x3
	};

protected:
	TCHAR _filePath[MAX_PATH];
	TCHAR _serviceName[MAX_PATH];

	SC_HANDLE _hSCManager;
	SC_HANDLE _hService;

	DRIVER_LOAD_STATUS drvStatus;

	BOOL isRandomName;

public:
	DynamicDriverLoader();
	DynamicDriverLoader(LPCTSTR FilePath);

	virtual ~DynamicDriverLoader();

	void SetRandomName(BOOL isRandom);

	DWORD Install();
	void Start();
	void Stop();
	void Uninstall();

protected:
	void _init();
	CB_DBGPRINT cbDbgPrint;
	void DebugPrint(LPTSTR message);

	SC_HANDLE OpenSCM();
	SC_HANDLE OpenSvc(SC_HANDLE hSCManager);
	void CloseSCM();
	void CloseSvc();

	static LPTSTR GetServiceName(LPCTSTR FilePath, LPTSTR OutString);
	static LPTSTR GetServiceName(LPCTSTR FilePath, BOOL isRandomName, LPTSTR OutString);
};

