//************************************
// File:     DriverLoaderDlg.h
// Date:     2012-12-15
// Author:   Cheong H Lee
// Contains: Header of dialog class
//************************************

#pragma once

#include "DynamicDriverLoader.h"
#include "afxwin.h"

// CDriverLoaderDlg dialog
class CDriverLoaderDlg : public CDialog
{
// Construction
public:
	CDriverLoaderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DRIVERLOADER_DIALOG };
	enum CONTROL_STATE
	{
		CTRL_DEFAULT,
		CTRL_INSTALLED,
		CTRL_STOPPED,
		CTRL_STARTED
	};

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	DynamicDriverLoader drvLoader;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnbtnInstallClicked();
	afx_msg void OnbtnStartClicked();
	afx_msg void OnbtnStopClicked();
	afx_msg void OnbtnUninstallClicked();
	afx_msg void OnbtnBrowseClicked();
	afx_msg void OnbtnAboutClicked();
	afx_msg void OnbtnDisplayRndTip();
	BOOL checkValidInput();
	void SetControlState(CONTROL_STATE state);

	CEdit txtFilePath;
	CButton btnInstall;
	CButton btnStart;
	CButton btnStop;
	CButton btnUninstall;
	CButton btnBrowse;
	CStatic txtState;
	
	CButton chkRandomize;
	CButton btnRndNameTip;
};
