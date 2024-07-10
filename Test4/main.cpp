#include <iostream>
#include <vector>
#include <string>
#include "database.h"
#include "http_client.h"
#include "Test4Dlg.h"
#include <json/json.h>

#include "afxdialogex.h"

#include <winhttp.h>
#include <afxwin.h> // For AfxMessageBox
#pragma comment(lib, "winhttp.lib")

int main() {

    HttpClient client;

    // 데이터베이스 초기화 및 연결
    Database db("tcp://127.0.0.1:3306", "user", "1234");
    if (!db.connect("pop")) { // 수정
        cerr << "Failed to connect to MySQL database." << endl;
        return 1;
    }

    // 테이블에서 데이터 가져오기
    vector<vector<double>> data_set = db.fetchDataFromTable("robot_current_anomaly", 5, 1); // 수정

    // JSON 데이터 생성
    Json::Value root;
    Json::StreamWriterBuilder writer;

    // data_set 벡터를 JSON으로 변환
    for (int i = 0; i < data_set.size(); ++i) {
        Json::Value innerArray(Json::arrayValue);
        for (int j = 0; j < data_set[i].size(); ++j) {
            innerArray.append(data_set[i][j]);
        }
        root["data"].append(innerArray);
    }

    // JSON을 문자열로 변환
    string json_data = Json::writeString(writer, root);

    // JSON 문자열을 wide string으로 변환
    wstring wide_json_data = client.StringToWideString(json_data);

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
            string vibString = setArrayForVibration();
            string curString = setArrayForCurrent();
            CStringA jsonData;
            jsonData.Format("{\"vibration\": [%s], \"current\": [%s]}", vibString.c_str(), curString.c_str());
            HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/api/robot_welding_predicitive_maintenance",
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

    /*
        // 진동과 전류 데이터
        Json::Value vib_data(Json::arrayValue);
        vib_data.append(0.02);
        vib_data.append(0.03);
        vib_data.append(0.04);

        Json::Value cur_data(Json::arrayValue);
        cur_data.append(0.05);
        cur_data.append(0.06);
        cur_data.append(0.07);

        root["vibration"] = vib_data;
        root["current"] = cur_data;

        string json_data = Json::writeString(writer, root);

        // HTTP 요청 보내기

        client.sendHttpRequest("127.0.0.1", "/api/robot_welding_predicitive_maintenance", json_data);
        */

        // 데이터베이스 연결 해제
    db.close();

    return 0;
}
