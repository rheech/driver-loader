//************************************
// File:     DynamicDriverLoader.cpp
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

#include "stdafx.h"
#include "DynamicDriverLoader.h"
#include <ctime>

//************************************
// Method:    _init
// Purpose:   To initialize class members on construction
//************************************
void DynamicDriverLoader::_init()
{
	//************************************
	// Procedure
	//    1. Initialize all pointers and variables.
	//    2. OpenSCManager for accessing service
	//************************************
	cbDbgPrint = NULL;
	_hSCManager = NULL;
	_hService = NULL;
	drvStatus = DRIVER_UNINSTALLED;
	isRandomName = FALSE;
}

//************************************
// Type:      Default constructor
// Purpose:   To support default constructor.
//            must reconstruct with other constructors to use.
//************************************
DynamicDriverLoader::DynamicDriverLoader()
{
	_init();
}

//************************************
// Type:      Constructor
// Purpose:   Constructor gets file path
// Parameter: CString FilePath - driver file path
//************************************
DynamicDriverLoader::DynamicDriverLoader(LPCTSTR FilePath)
{
	_init();
	_tcscpy_s(_filePath, MAX_PATH, FilePath);

	TCHAR strServiceName[MAX_PATH];
	_tcscpy_s(_serviceName, MAX_PATH, GetServiceName(FilePath, strServiceName));
}

//************************************
// Type:      Destructor
// Purpose:   To return all resources to operating system
//************************************
DynamicDriverLoader::~DynamicDriverLoader()
{
	try
	{
		Uninstall();
	}
	catch (DriverLoadException)
	{
	}

	CloseSvc();
	CloseSCM();
}


//************************************
// Method:    SetRandomName
// Purpose:   To determine random service name requested
// Parameter: BOOL isRandom - TRUE if random
//************************************
void DynamicDriverLoader::SetRandomName(BOOL isRandom)
{
	isRandomName = isRandom;

	TCHAR strServiceName[MAX_PATH];
	_tcscpy_s(_serviceName, MAX_PATH, GetServiceName(_filePath, isRandom, strServiceName));
}

//************************************
// Method:    Install
// Purpose:   Install driver to system
// Returns:   DWORD - service installed (ERROR_SUCCESS) / already installed
// Parameter: CString FilePath - sys file path
//************************************
DWORD DynamicDriverLoader::Install()
{
	//************************************
	// Procedure
	//    1. Create service. Service name is the name of file.
	//    2.1 If service already exists or creation succeed, return ERROR_SUCCESS
	//    2.2 else, return error detail.
	//************************************

	SC_HANDLE hSCManager;
	SC_HANDLE hService;

	hSCManager = OpenSCM();

	DebugPrint(_T("Create service"));

	hService = CreateService(hSCManager, _serviceName,
				_serviceName,
				SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,
				SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE,
				_filePath,
				NULL, NULL, NULL, NULL, NULL);

	DWORD lastError = GetLastError();

	if (!hService)
	{
		if (lastError == ERROR_SERVICE_EXISTS)
		{
			drvStatus = DRIVER_INSTALLED;
			return ERROR_SUCCESS;
		}
		
		throw DriverLoadException(lastError, _T("Error on installing driver."));

		return lastError;
	}

	CloseServiceHandle(hService);

	drvStatus = DRIVER_INSTALLED;
	return ERROR_SUCCESS;
}

//************************************
// Method:    Start
// Purpose:   To start service driver
//************************************
void DynamicDriverLoader::Start()
{
	//************************************
	// Procedure
	//    1. Open service with service name (which is the name of the file)
	//    2. if started, change status to started.
	//************************************

	if (drvStatus != DRIVER_INSTALLED &&
			Install() != ERROR_SUCCESS)
	{
		throw DriverLoadException(GetLastError(), _T("Installing driver failed."));
	}

	SC_HANDLE hService = OpenSvc(OpenSCM());

	DebugPrint(_T("Start service."));

	if (!StartService(hService, 0, NULL))
	{
		DWORD dwError = GetLastError();

		if (dwError == ERROR_SERVICE_ALREADY_RUNNING)
		{
			drvStatus = DRIVER_STARTED;
			return;
		}
		
		throw DriverLoadException(dwError, _T("Start service failed."));
	}

	drvStatus = DRIVER_STARTED;
}

//************************************
// Method:    Stop
// Purpose:   To stop service driver
//************************************
void DynamicDriverLoader::Stop()
{
	SC_HANDLE hService;

	if (drvStatus == DRIVER_STARTED)
	{
		DebugPrint(_T("Stopping service."));
		SERVICE_STATUS ss;

		hService = OpenSvc(OpenSCM());

		if (!ControlService(hService, SERVICE_CONTROL_STOP, &ss))
			throw DriverLoadException(GetLastError(), _T("Stop service failed."));
	}
	else
	{
		throw DriverLoadException(_T("Unable to stop service."));
	}

	drvStatus = DRIVER_STOPPED;
}


//************************************
// Method:    Uninstall
// Purpose:   To uninstall driver
//************************************
void DynamicDriverLoader::Uninstall()
{
	//************************************
	// Procedure
	//    1. If service is running, stop service
	//    2. If service is installed, uninstall service
	//************************************

	if (drvStatus == DRIVER_STARTED)
		Stop();

	if (drvStatus == DRIVER_INSTALLED)
	{
		SC_HANDLE hService = OpenSvc(OpenSCM());

		DebugPrint(_T("Uninstalling service."));
		
		if (!DeleteService(hService))
			throw DriverLoadException(GetLastError(), _T("Uninstall service failed."));

		CloseSvc();
		CloseSCM();
	}

	drvStatus = DRIVER_UNINSTALLED;
}


//************************************
// Method:    DebugPrint
// Purpose:   To provide detailed message on
//            every process.
// Parameter: LPCTSTR message - message to out
//************************************
void DynamicDriverLoader::DebugPrint(LPTSTR message)
{
	if (cbDbgPrint != NULL)
	{
		cbDbgPrint(message);
	}
}

//************************************
// Method:    OpenSCM
// Purpose:   To open SCManager safely
//************************************
SC_HANDLE DynamicDriverLoader::OpenSCM()
{
	CloseSCM();

	_hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (!_hSCManager)
		throw DriverLoadException(GetLastError(), _T("OpenSCManager failed."));

	return _hSCManager;
}

//************************************
// Method:    OpenSvc
// Purpose:   To open service safely
// Parameter: SC_HANDLE hSCManager - handle to service manager
//************************************
SC_HANDLE DynamicDriverLoader::OpenSvc(SC_HANDLE hSCManager)
{
	CloseSvc();

	_hService = OpenService(hSCManager, _serviceName, 
					SERVICE_ALL_ACCESS);

	if (!_hService)
		throw DriverLoadException(GetLastError(), _T("OpenService failed."));

	return _hService;
}

//************************************
// Method:    CloseSCM
// Purpose:   To close SCM safely
//************************************
void DynamicDriverLoader::CloseSCM()
{
	if (_hSCManager != NULL)
		CloseServiceHandle(_hSCManager);
	
	_hSCManager = NULL;
}

//************************************
// Method:    CloseSvc
// Purpose:   To close service safely
//************************************
void DynamicDriverLoader::CloseSvc()
{
	if (_hService != NULL)
		CloseServiceHandle(_hService);

	_hService = NULL;
}


//************************************
// Method:    GetServiceName
// Purpose:   To extract service name from file path
// Parameter: LPTSTR FilePath - file path in string
//************************************
LPTSTR DynamicDriverLoader::GetServiceName(LPCTSTR FilePath, LPTSTR OutString)
{
	return GetServiceName(FilePath, FALSE, OutString);
}

//************************************
// Method:    GetServiceName
// Purpose:   To extract service name from file path
// Parameters:
//            LPTSTR FilePath - file path in string
//            BOOL isRandomName - randomize service name
//************************************
LPTSTR DynamicDriverLoader::GetServiceName(LPCTSTR FilePath, BOOL isRandomName, LPTSTR OutString)
{
	//************************************
	// Procedure
	//    1. Get file name with extension from full path
	//    2. Remove extension
	//    3. Randomize name if random requested
	//    4. Return extracted service name (file name)
	//************************************
	int intTokenPos = 0;
	TCHAR cpyOfFilePath[MAX_PATH];
	TCHAR strTemp[MAX_PATH + 4];

	TCHAR* tok = NULL;
	TCHAR* lastTok = NULL;
	TCHAR* pos = NULL;

	_tcscpy_s(cpyOfFilePath, MAX_PATH, FilePath);
	tok = _tcstok_s(cpyOfFilePath, _T("\\"), &pos);

	while (tok != NULL)
	{
		tok = _tcstok_s(NULL, _T("\\"), &pos);

		if (tok != NULL)
			lastTok = tok;
	}

	tok = _tcsstr(lastTok, _T("."));

	int index = &(tok[0]) - &(lastTok[0]);
	lastTok[index] = _T('\0');

	_tcscpy_s(strTemp, MAX_PATH + 4, lastTok);

	if (isRandomName)
	{
		int intRnd;
		TCHAR tempCpy[MAX_PATH + 4];

		srand((unsigned int)time(NULL));

		intRnd = rand() % 0xFF;
		wsprintf(tempCpy, _T("%s%02X"), strTemp, intRnd);

		_tcscpy_s(strTemp, MAX_PATH + 4, tempCpy);
	}

	_tcscpy_s(OutString, MAX_PATH, strTemp);

	return OutString;
}

