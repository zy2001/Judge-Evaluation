//
// Created by zy on 2020/2/1.
//
#include <windows.h>
#include <iostream>
#include "../header/connect.h"
#include "../../config.h"


MYSQL Connect::conn;

bool Connect::init() {
//    int start = GetCurrentTime();

    mysql_init(&Connect::conn);
    if (mysql_real_connect(&Connect::conn, "localhost", MYSQL_USERNAME, MYSQL_PASSWORD, "openjudge", 3306, NULL, 0)) {
        std::cout << "MYSQL has Connected!" << std::endl;
//        int end = GetCurrentTime();
//        std::cout<<end-start<<std::endl;
        return true;
    } else {
        return false;
    };
}


void Connect::close() {
    mysql_close(&Connect::conn);
}

MYSQL_RES *Connect::mysql_select(std::string sql) {
    MYSQL_RES *result = NULL;
    if (0 == mysql_query(&Connect::conn, sql.c_str())) {
        std::cout << "mysql_query() select data succeed" << std::endl;
        //一次性取得数据集
        result = mysql_store_result(&Connect::conn);
    } else {
        std::cout << "mysql_query() select data failed" << std::endl;
    }
    return result;
}

bool Connect::mysql_insert(std::string sql) {
    if (0 == mysql_query(&Connect::conn, sql.c_str())) {
        std::cout << "mysql_query() insert data succeed" << std::endl;
        return true;
    } else {
        return false;
    }
}

bool Connect::mysql_update(std::string sql) {
    if (0 == mysql_query(&Connect::conn, sql.c_str())) {
        std::cout << "mysql_query() update data succeed" << std::endl;
        return true;
    } else {
        return false;
    }
}


bool Connect::mysql_delete(std::string sql) {
    if (0 == mysql_query(&Connect::conn, sql.c_str())) {
        std::cout << "mysql_query() update data succeed" << std::endl;
        return true;
    } else {
        return false;
    }
}

void Connect::showResult(MYSQL_RES *result) {
    //取得并打印行数
    int rowcount = mysql_num_rows(result);
    std::cout << "row count: " << rowcount << std::endl;
//取得并打印各字段的名称
    unsigned int fieldCount = mysql_num_fields(result);
    MYSQL_FIELD *field = NULL;
    for (unsigned int i = 0; i < fieldCount; i++) {
        field = mysql_fetch_field_direct(result, i);
        std::cout << field->name << "\t\t";
    }
    std::cout << std::endl;
    MYSQL_ROW row = NULL;
    row = mysql_fetch_row(result);
    while (NULL != row) {
        for (int i = 0; i < fieldCount; i++) {
            if (NULL != row[i])
                std::cout << row[i] << "\t\t";
        }
        std::cout << std::endl;
        row = mysql_fetch_row(result);
    }
}
