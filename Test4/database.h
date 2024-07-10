#pragma once
#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <vector>
#include <string>

#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

using namespace std;

struct Database {
    sql::Driver* driver;
    sql::Connection* con;
    sql::PreparedStatement* pstmt;
    sql::ResultSet* result;

    Database(const string& server, const string& username, const string& password);
    ~Database();

    bool connect(const string& schema);
    void close();

    vector<vector<double>> fetchDataFromTable(const string& tableName, int limit, int offset);
};

#endif // DATABASE_H
