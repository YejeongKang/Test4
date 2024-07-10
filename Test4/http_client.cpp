#include "http_client.h"
#include <windows.h>
#include <winhttp.h>
#include <iostream>
#include <string>
#include <json/json.h>

#pragma comment(lib, "winhttp.lib")

using namespace std;

wstring HttpClient::StringToWideString(const string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

void HttpClient::sendHttpRequest(const string& serverAddr, const string& endpoint, const string& json_data) {
    wstring wide_serverAddr = StringToWideString(serverAddr);
    wstring wide_endpoint = StringToWideString(endpoint);
    wstring wide_json_data = StringToWideString(json_data);

    HINTERNET hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession) {
        cerr << "Error in WinHttpOpen: " << GetLastError() << endl;
        return;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, wide_serverAddr.c_str(), INTERNET_DEFAULT_HTTP_PORT, 0);

    if (!hConnect) {
        cerr << "Error in WinHttpConnect: " << GetLastError() << endl;
        WinHttpCloseHandle(hSession);
        return;
    }

    wstring requestType = L"POST";
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, requestType.c_str(), wide_endpoint.c_str(), NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

    if (!hRequest) {
        cerr << "Error in WinHttpOpenRequest: " << GetLastError() << endl;
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return;
    }

    wstring headers = L"Content-Type: application/json";
    BOOL bResults = WinHttpSendRequest(hRequest, headers.c_str(), -1L, (LPVOID)wide_json_data.c_str(),
        wide_json_data.size() * sizeof(wchar_t), wide_json_data.size() * sizeof(wchar_t), 0);

    if (!bResults) {
        cerr << "Error in WinHttpSendRequest: " << GetLastError() << endl;
    }

    bResults = WinHttpReceiveResponse(hRequest, NULL);

    if (bResults) {
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        LPSTR pszOutBuffer;
        BOOL bKeepGoing = true;

        do {
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                cerr << "Error in WinHttpQueryDataAvailable: " << GetLastError() << endl;
                bKeepGoing = false;
            }

            pszOutBuffer = new char[dwSize + 1];

            if (!pszOutBuffer) {
                cerr << "Out of memory" << endl;
                dwSize = 0;
                bKeepGoing = false;
            }
            else {
                ZeroMemory(pszOutBuffer, dwSize + 1);

                if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
                    cerr << "Error in WinHttpReadData: " << GetLastError() << endl;
                }
                else {
                    cout << "Response: " << pszOutBuffer << endl;
                }

                delete[] pszOutBuffer;
            }
        } while (dwSize > 0 && bKeepGoing);
    }
    else {
        cerr << "Error in WinHttpReceiveResponse: " << GetLastError() << endl;
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
}
