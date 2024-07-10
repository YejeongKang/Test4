#pragma once
#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>

using namespace std;

struct HttpClient {
    wstring StringToWideString(const string& str);
    void sendHttpRequest(const string& serverAddr, const string& endpoint, const string& json_data);
};

#endif // HTTP_CLIENT_H
