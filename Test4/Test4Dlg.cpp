// Test4Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Test4.h"
#include "Test4Dlg.h"
#include "afxdialogex.h"

#include <winhttp.h>
#include <afxwin.h> // For AfxMessageBox
#pragma comment(lib, "winhttp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTest4Dlg dialog



CTest4Dlg::CTest4Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TEST4_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTest4Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTest4Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTest4Dlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CTest4Dlg message handlers

BOOL CTest4Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTest4Dlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTest4Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTest4Dlg::OnBnClickedButton1()
{
    HINTERNET hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (hSession)
    {
        HINTERNET hConnect = WinHttpConnect(hSession, L"127.0.0.1",
            5000, 0);

        if (hConnect)
        {
            // Prepare JSON data (ensure it's properly formatted)
            CStringA jsonData = "{\"num1\": 10, \"num2\": 20}";

            HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/api/add",
                NULL, WINHTTP_NO_REFERER,
                WINHTTP_DEFAULT_ACCEPT_TYPES,
                0);

            if (hRequest)
            {
                // Set request headers
                const wchar_t* headers = L"Content-Type: application/json";
                if (!WinHttpSendRequest(hRequest, headers, -1L,
                    (LPVOID)(LPSTR)jsonData.GetBuffer(), jsonData.GetLength(), jsonData.GetLength(), 0))
                {
                    AfxMessageBox(_T("Error sending request."));
                }
                else
                {
                    if (!WinHttpReceiveResponse(hRequest, NULL))
                    {
                        AfxMessageBox(_T("Error receiving response."));
                    }
                    else
                    {
                        DWORD dwSize = 0;
                        DWORD dwDownloaded = 0;
                        LPSTR pszOutBuffer;
                        CStringA response;

                        do
                        {
                            dwSize = 0;
                            if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
                            {
                                AfxMessageBox(_T("Error in WinHttpQueryDataAvailable."));
                                break;
                            }

                            pszOutBuffer = new char[dwSize + 1];

                            if (!pszOutBuffer)
                            {
                                AfxMessageBox(_T("Out of memory."));
                                dwSize = 0;
                            }
                            else
                            {
                                ZeroMemory(pszOutBuffer, dwSize + 1);

                                if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
                                    dwSize, &dwDownloaded))
                                {
                                    AfxMessageBox(_T("Error in WinHttpReadData."));
                                }
                                else
                                {
                                    response += CStringA(pszOutBuffer);
                                }

                                delete[] pszOutBuffer;
                            }
                        } while (dwSize > 0);

                        AfxMessageBox(CString(response));
                    }
                }

                WinHttpCloseHandle(hRequest);
            }

            WinHttpCloseHandle(hConnect);
        }

        WinHttpCloseHandle(hSession);
    }
}