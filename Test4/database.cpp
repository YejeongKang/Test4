#include "database.h"
#include <string>

Database::Database(const string& server, const string& username, const string& password) {
    driver = get_driver_instance();
    con = driver->connect(server, username, password);
}

Database::~Database() {
    delete pstmt;
    delete con;
}

bool Database::connect(const string& schema) {
    try {
        con->setSchema(schema);
        return true;
    }
    catch (sql::SQLException& e) {
        cerr << "SQLException: " << e.what() << endl;
        return false;
    }
}

void Database::close() {
    delete pstmt;
    delete con;
}

vector<vector<double>> Database::fetchDataFromTable(const string& tableName, int limit, int offset) {
    string col, data, column_name;
    vector<string> col_list;
    vector<double> vec;
    vector<vector<double>> data_set;

    try {

        pstmt = con->prepareStatement("SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = '" + tableName + "' ORDER BY ORDINAL_POSITION LIMIT ? OFFSET ?");
        pstmt->setString(1, to_string(limit));
        pstmt->setString(2, to_string(offset));
        result = pstmt->executeQuery();


        while (result->next()) {
            col = result->getString(1).c_str();
            col_list.push_back(col);
        }

        for (string col : col_list) {
            // cout << col << endl;

            pstmt = con->prepareStatement("select " + col + " from pop.robot_current_anomaly order by Time");
            result = pstmt->executeQuery();

            while (result->next()) {

                data = result->getString(1).c_str();

                vec.push_back(stod(data));
            }
            //for (string v : vec) cout << v << " ";

            data_set.push_back(vec);
            vec.clear();
        }
        col_list.clear();

        // 데이터 확인
        for (int i = 0; i < data_set.size(); i++) {
            cout << "[ " << i + 1 << " ]" << endl;
            for (int j = 0; j < data_set[0].size(); j++) {
                cout << j + 1 << " : " << data_set[i][j] << endl;
            }
        }

    }
    catch (sql::SQLException& e) {
        cerr << "SQLException: " << e.what() << endl;
    }

    return data_set;

    /*
    try {
        pstmt = con->prepareStatement("SELECT * FROM " + tableName + " LIMIT ? OFFSET ?");
        pstmt->setInt(1, limit);
        pstmt->setInt(2, offset);
        result = pstmt->executeQuery();

        int columnCount = result->getMetaData()->getColumnCount();

        while (result->next()) {
            vec.clear();
            for (int i = 1; i <= columnCount; ++i) {
                vec.push_back(result->getString(i));
            }
            data_set.push_back(vec);
        }
    }
    catch (sql::SQLException& e) {
        cerr << "SQLException: " << e.what() << endl;
    }
    */
}
