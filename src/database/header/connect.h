//
// Created by zy on 2020/2/1.
//

#ifndef JUDGE_CONNECT_H
#define JUDGE_CONNECT_H

#include <mysql.h>

class Connect
{
private:
    static MYSQL conn;
public:
    //MYSQL Connection Init
    static bool init();

    //MYSQL Connection Close
    static void close();

    //Select Data By sql
    static MYSQL_RES* mysql_select(std::string sql);

    //Insert Data By sql
    static bool mysql_insert(std::string sql);

    //Update Data By sql
    static bool mysql_update(std::string sql);

    //Delete Data By sql
    static bool mysql_delete(std::string sql);


    static void showResult(MYSQL_RES *result);
};

#endif //JUDGE_CONNECT_H
