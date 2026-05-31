#pragma once

#include <mysql/mysql.h>
#include <string>

bool safe(std::string name)
{
    return name.find("..") == std::string::npos &&
           name.find("/") == std::string::npos;
}

std::string escapeSQL(
    MYSQL *conn,
    std::string text
)
{
    char escaped[512];

    mysql_real_escape_string(
        conn,
        escaped,
        text.c_str(),
        text.length()
    );

    return escaped;
}