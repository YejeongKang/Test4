
// Test4Dlg.h : header file
//

#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream> 
#include <string>

using namespace std;

// CTest4Dlg dialog
class CTest4Dlg : public CDialogEx
{
// Construction
public:
	CTest4Dlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEST4_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation

protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButton1();
	
	string setArrayForCurrent();
	string setArrayForVibration();
	vector<double> current, vibration;

	afx_msg void OnBnClickedButton2();
};
