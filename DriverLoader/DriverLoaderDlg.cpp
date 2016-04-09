
// DriverLoaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DriverLoader.h"
#include "DriverLoaderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDriverLoaderDlg dialog




CDriverLoaderDlg::CDriverLoaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDriverLoaderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDriverLoaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, txtFilePath);
	DDX_Control(pDX, IDC_BUTTON1, btnInstall);
	DDX_Control(pDX, IDC_BUTTON2, btnStart);
	DDX_Control(pDX, IDC_BUTTON3, btnStop);
	DDX_Control(pDX, IDC_BUTTON4, btnUninstall);
	DDX_Control(pDX, IDC_BUTTON5, btnBrowse);
	DDX_Control(pDX, IDC_TEXT, txtState);
	DDX_Control(pDX, IDC_CHECK1, chkRandomize);
	DDX_Control(pDX, IDC_BUTTON7, btnRndNameTip);
}

BEGIN_MESSAGE_MAP(CDriverLoaderDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CDriverLoaderDlg::OnbtnInstallClicked)
	ON_BN_CLICKED(IDC_BUTTON2, &CDriverLoaderDlg::OnbtnStartClicked)
ON_BN_CLICKED(IDC_BUTTON3, &CDriverLoaderDlg::OnbtnStopClicked)
ON_BN_CLICKED(IDC_BUTTON4, &CDriverLoaderDlg::OnbtnUninstallClicked)
ON_BN_CLICKED(IDC_BUTTON5, &CDriverLoaderDlg::OnbtnBrowseClicked)
ON_BN_CLICKED(IDC_BUTTON6, &CDriverLoaderDlg::OnbtnAboutClicked)
ON_BN_CLICKED(IDC_BUTTON7, &CDriverLoaderDlg::OnbtnDisplayRndTip)
END_MESSAGE_MAP()


// CDriverLoaderDlg message handlers

BOOL CDriverLoaderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	btnStart.EnableWindow(FALSE);
	btnStop.EnableWindow(FALSE);
	btnUninstall.EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDriverLoaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDriverLoaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDriverLoaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//************************************
// Method:    PreTranslateMessage
// Purpose:   To modify Enter/Esc key method
// Parameter: MSG * pMsg
//************************************
BOOL CDriverLoaderDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

//************************************
// Method:    OnbtnInstallClicked
// Purpose:   To install service
//************************************
void CDriverLoaderDlg::OnbtnInstallClicked()
{
	if (!checkValidInput())
		return;

	CString filePath;
	txtFilePath.GetWindowText(filePath);

	drvLoader = DynamicDriverLoader(filePath.GetBuffer());
	
	if (chkRandomize.GetCheck() == BST_CHECKED)
		drvLoader.SetRandomName(TRUE);

	try
	{
		drvLoader.Install();

		SetControlState(CTRL_INSTALLED);

		txtState.SetWindowText(_T("Service installed."));
	}
	catch (DriverLoadException ex)
	{
		txtState.SetWindowText(ex.GetErrorMessage());
	}
}

//************************************
// Method:    OnbtnStartClicked
// Purpose:   To start service
//************************************
void CDriverLoaderDlg::OnbtnStartClicked()
{
	if (!checkValidInput())
		return;

	try
	{
		drvLoader.Start();
		SetControlState(CTRL_STARTED);
		txtState.SetWindowText(_T("Service started."));
	}
	catch (DriverLoadException ex)
	{
		txtState.SetWindowText(ex.GetErrorMessage());
	}
}

//************************************
// Method:    OnbtnStopClicked
// Purpose:   To stop service
//************************************
void CDriverLoaderDlg::OnbtnStopClicked()
{
	if (!checkValidInput())
		return;

	try
	{
		drvLoader.Stop();
		SetControlState(CTRL_STOPPED);
		txtState.SetWindowText(_T("Service stopped."));
	}
	catch (DriverLoadException ex)
	{
		txtState.SetWindowText(ex.GetErrorMessage());
	}
}

//************************************
// Method:    OnbtnUninstallClicked
// Purpose:   To uninstall service
//************************************
void CDriverLoaderDlg::OnbtnUninstallClicked()
{
	if (!checkValidInput())
		return;

	try
	{
		drvLoader.Uninstall();

		SetControlState(CTRL_DEFAULT);

		txtState.SetWindowText(_T("Service uninstalled."));
	}
	catch (DriverLoadException ex)
	{
		txtState.SetWindowText(ex.GetErrorMessage());
	}
}

//************************************
// Method:    OnbtnBrowseClicked
// Purpose:   To get sys file path from user
//************************************
void CDriverLoaderDlg::OnbtnBrowseClicked()
{
	TCHAR BASED_CODE szFilter[] = _T("System Configuration File (*.sys)|*.sys|")
			_T("All Files (*.*)|*.*||");

	CFileDialog fileDlg = CFileDialog(TRUE, NULL, NULL,
				OFN_FILEMUSTEXIST, szFilter);

	if (fileDlg.DoModal() == IDOK)
		txtFilePath.SetWindowText(fileDlg.GetPathName());
}

//************************************
// Method:    OnbtnAboutClicked
// Purpose:   To open about dialog
//************************************
void CDriverLoaderDlg::OnbtnAboutClicked()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


//************************************
// Method:    checkValidInput
// Purpose:   To check file path in textbox is valid
// Returns:   BOOL - TRUE if valid
//************************************
BOOL CDriverLoaderDlg::checkValidInput()
{
	CString strPath;
	txtFilePath.GetWindowText(strPath);

	CFileFind pFind;

	if (!pFind.FindFile(strPath))
	{
		txtState.SetWindowText(_T("Please choose a valid file."));
		return FALSE;
	}
	
	return TRUE;
}

//************************************
// Method:    SetControlState
// Purpose:   To change control state dynamically
// Parameter: CONTROL_STATE state - Application state
//************************************
void CDriverLoaderDlg::SetControlState(CONTROL_STATE state)
{
	switch (state)
	{
	case CTRL_INSTALLED:
	case CTRL_STOPPED:
		btnInstall.EnableWindow(FALSE);
		btnStart.EnableWindow(TRUE);
		btnStop.EnableWindow(FALSE);
		btnUninstall.EnableWindow(TRUE);
		txtFilePath.EnableWindow(FALSE);
		btnBrowse.EnableWindow(FALSE);
		chkRandomize.EnableWindow(FALSE);
		btnRndNameTip.EnableWindow(FALSE);
		break;
	case CTRL_STARTED:
		btnInstall.EnableWindow(FALSE);
		btnStart.EnableWindow(FALSE);
		btnStop.EnableWindow(TRUE);
		btnUninstall.EnableWindow(TRUE);
		txtFilePath.EnableWindow(FALSE);
		btnBrowse.EnableWindow(FALSE);
		chkRandomize.EnableWindow(FALSE);
		btnRndNameTip.EnableWindow(FALSE);
		break;
	default:
		btnInstall.EnableWindow(TRUE);
		btnStart.EnableWindow(FALSE);
		btnStop.EnableWindow(FALSE);
		btnUninstall.EnableWindow(FALSE);
		txtFilePath.EnableWindow(TRUE);
		btnBrowse.EnableWindow(TRUE);
		chkRandomize.EnableWindow(TRUE);
		btnRndNameTip.EnableWindow(TRUE);
		break;
	}
}


//************************************
// Method:    OnbtnDisplayRndTip
// Purpose:   To explain user what random name does
//************************************
void CDriverLoaderDlg::OnbtnDisplayRndTip()
{
	CString msg = CString("By checking the checkbox, application randomizes the name of service and display.\r\n");
	msg += CString("Not recommended unless there is a problem with installing driver.");

	MessageBox(msg.GetBuffer(), _T("Driver Loader"), MB_ICONINFORMATION);
}
