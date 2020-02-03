//
// Created by zy on 2020/2/1.
//
#include <iostream>
#include "../../config.h"
#include "../header/connect.h"

MYSQL Connect::conn;

bool Connect::init() {
    mysql_init(&Connect::conn);
    if (mysql_real_connect(&Connect::conn, "localhost", MYSQL_USERNAME, MYSQL_PASSWORD, "openjudge", 3306, nullptr, 0)) {
        std::cout << "MYSQL has Connected!" << std::endl;
        return true;
    } else {
        return false;
    };
}


void Connect::close() {
    mysql_close(&Connect::conn);
}

MYSQL_RES *Connect::mysql_select(const std::string& sql) {
    MYSQL_RES *result = nullptr;
    if (0 == mysql_query(&Connect::conn, sql.c_str())) {
//        std::cout << "mysql_query() select data succeed" << std::endl;
        //一次性取得数据集
        result = mysql_store_result(&Connect::conn);
    }
    return result;
}

bool Connect::mysql_insert(const std::string& sql) {
    return 0 == mysql_query(&Connect::conn, sql.c_str());
}

bool Connect::mysql_update(const std::string& sql) {
    return 0 == mysql_query(&Connect::conn, sql.c_str());
}


bool Connect::mysql_delete(const std::string& sql) {
    return 0 == mysql_query(&Connect::conn, sql.c_str());
}

void Connect::showResult(MYSQL_RES *result) {
    //取得并打印行数
    int rowcount = mysql_num_rows(result);
    std::cout << "row count: " << rowcount << std::endl;
//取得并打印各字段的名称
    unsigned int fieldCount = mysql_num_fields(result);
    MYSQL_FIELD *field = nullptr;
    for (unsigned int i = 0; i < fieldCount; i++) {
        field = mysql_fetch_field_direct(result, i);
        std::cout << field->name << "\t\t";
    }
    std::cout << std::endl;
    MYSQL_ROW row = nullptr;
    row = mysql_fetch_row(result);
    while (nullptr != row) {
        for (unsigned int i = 0; i < fieldCount; i++) {
            if (nullptr != row[i])
                std::cout << row[i] << "\t\t";
        }
        std::cout << std::endl;
        row = mysql_fetch_row(result);
    }
}
