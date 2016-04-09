//************************************
// File:     DriverLoadException.h
// Version:  1.1
// Date:     2012-12-15
// Author:   Cheong H Lee
// Contains: Exception handler for DriverLoader
//************************************

//************************************
// Revision History
// 2012-12-18   Convert CString to TCHAR String.
//              document not modified yet. (1.1)
//************************************

#pragma once

class DriverLoadException
//************************************
// Class:     DriverLoadException
// Purpose:   To provide error handler for
//            DynamicDriverLoader class
// Members:
//************************************
{
protected:
	TCHAR _strMessage[MAX_PATH];
	DWORD _dwError;
	LPTSTR _lastErrorMessage;

public:
	DriverLoadException();
	DriverLoadException(DriverLoadException& org);
	DriverLoadException(LPCTSTR msg);
	DriverLoadException(DWORD ErrorNumber);
	DriverLoadException(DWORD ErrorNumber, LPCTSTR msg);
	
	DWORD GetErrorNumber();
	LPTSTR GetErrorMessage();

	virtual ~DriverLoadException();

protected:
	void _init(DWORD ErrorNumber, LPCTSTR msg);
	LPTSTR CreateErrorMessage();
};


