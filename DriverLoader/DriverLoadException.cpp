//************************************
// File:     DriverLoadException.cpp
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

#include "stdafx.h"
#include "DriverLoadException.h"


//************************************
// Method:    _init
// Purpose:   Internal initializer for construction
// Parameters:
//   IN       DWORD ErrorNumber - Error number on GetLastError()
//   IN       CString& msg - Custom message (additional details on error)
//************************************
void DriverLoadException::_init(DWORD ErrorNumber, LPCTSTR msg)
{
	_strMessage[0] = _T('\0');
	_lastErrorMessage = NULL;
	_dwError = ErrorNumber;

	if (_tcslen(msg) != 0)
		_tcscpy_s(_strMessage, MAX_PATH, msg);
}

//************************************
// Type:      Default Constructor
// Purpose:   To provide default construction
//************************************
DriverLoadException::DriverLoadException()
{
	_init(NULL, _T(""));
}


//************************************
// Type:      Copy Constructor
// Purpose:   To provide construction copy on throw
// Parameter: DriverLoadException& org - original object
//************************************
DriverLoadException::DriverLoadException(DriverLoadException& org)
{
 	_lastErrorMessage = NULL;
	_tcscpy_s(_strMessage, MAX_PATH, org._strMessage);
	_dwError = org._dwError;

	if (org._lastErrorMessage != NULL)
	{
		int length = _tcslen(org._lastErrorMessage);
		_lastErrorMessage = new TCHAR[length];
		_tcscpy_s(_lastErrorMessage, length, org._lastErrorMessage);
	}
}


//************************************
// Type:      Constructor
// Purpose:   Constructor with custom error message
// Parameter: CString& msg - detail message of error
//************************************
DriverLoadException::DriverLoadException(LPCTSTR msg)
{
	_init(NULL, msg);
}

//************************************
// Type:      Constructor
// Purpose:   Constructor with windows error number
// Parameter: DWORD ErrorNumber - error number for windows
//************************************
DriverLoadException::DriverLoadException(DWORD ErrorNumber)
{
	_init(ErrorNumber, _T(""));
}

//************************************
// Type:      Constructor
// Purpose:   Constructor with windows error number and message
// Parameters:
//   IN       DWORD ErrorNumber - Error number for windows
//   IN       CString & msg - additional details of error
//************************************
DriverLoadException::DriverLoadException(DWORD ErrorNumber, LPCTSTR msg)
{
	_init(ErrorNumber, msg);
}

DriverLoadException::~DriverLoadException()
{
	if (_lastErrorMessage != NULL)
		delete [] _lastErrorMessage;

	_lastErrorMessage = NULL;
}

//************************************
// Method:    GetErrorNumber
// Purpose:   To provide accessor for error number.
// Returns:   DWORD - Error number
//************************************
DWORD DriverLoadException::GetErrorNumber()
{
	return _dwError;
}

//************************************
// Method:    GetErrorMessage
// Purpose:   To provide accessor for error details
// Returns:   CString - error details on string
//************************************
LPTSTR DriverLoadException::GetErrorMessage()
{
	return CreateErrorMessage();
}

//************************************
// Method:    CreateErrorMessage
// Purpose:   Create detailed and formatted
//            error message.
// Returns:   CString - formatted error message in string
//************************************
LPTSTR DriverLoadException::CreateErrorMessage()
{
	int intTotalLen = 0;
	TCHAR* rtnMsg = NULL;

	if (_dwError != NULL)
	{
		LPTSTR lpErrMsg;

		if(::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
					| FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, _dwError,	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpErrMsg,
					0, NULL))
		{
			int winMsgLen = _tcslen(lpErrMsg) + 1;
			TCHAR* winMsg = new TCHAR[winMsgLen];
			_tcscpy_s(winMsg, winMsgLen, lpErrMsg);
			::LocalFree(lpErrMsg);

			if (winMsg[_tcslen(winMsg) - 1] == _T('\n'))
				winMsg[_tcslen(winMsg) - 1] = _T('\0');

			if (winMsg[_tcslen(winMsg) - 1] == _T('\r'))
				winMsg[_tcslen(winMsg) - 1] = _T('\0');

			intTotalLen += _tcslen(winMsg);
			intTotalLen += sizeof(TCHAR);
			intTotalLen += 512;

			rtnMsg = new TCHAR[intTotalLen];
			wsprintf(rtnMsg, _T("Error Code: %d\r\nMessage: %s\r\nDetails: %s"), 
							_dwError, winMsg, _strMessage);

			_lastErrorMessage = rtnMsg;
			delete [] winMsg;
		}
	}
	else
	{
		intTotalLen += sizeof(TCHAR);
		intTotalLen += 512;
		rtnMsg = new TCHAR[intTotalLen];

		wsprintf(rtnMsg, _T("Error: %s\r\n"), _strMessage);

		_lastErrorMessage = rtnMsg;
	}

	return rtnMsg;
}


