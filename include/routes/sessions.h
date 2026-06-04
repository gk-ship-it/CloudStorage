#pragma once

#include <unordered_map>
#include <string>
#include <mysql/mysql.h>

extern std::unordered_map<
    std::string,
    int
> sessions;

inline int getUserIdFromToken(
    MYSQL* conn,
    const std::string& token
)
{
    auto it =
        sessions.find(token);

    if(it != sessions.end())
    {
        return it->second;
    }

    std::string query =
        "SELECT id FROM users WHERE token='" +
        token +
        "'";

    if(mysql_query(conn, query.c_str()) != 0)
    {
        return -1;
    }

    MYSQL_RES* result =
        mysql_store_result(conn);

    if(!result)
    {
        return -1;
    }

    MYSQL_ROW row =
        mysql_fetch_row(result);

    if(!row)
    {
        mysql_free_result(result);
        return -1;
    }

    int userId =
        std::stoi(row[0]);

    mysql_free_result(result);

    sessions[token] =
        userId;

    return userId;
}