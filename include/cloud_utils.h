#pragma once

#include "crow.h"
#include <mysql/mysql.h>
#include <string>

bool safe(std::string name)
{
    return name.find("..") == std::string::npos &&
       name.find("/")  == std::string::npos &&
       name.find("\\") == std::string::npos;
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
crow::response successResponse(
    std::string message
)
{
    crow::json::wvalue json;

    json["success"] = true;
    json["message"] = message;

    return crow::response(
        json
    );
}

crow::response errorResponse(
    int code,
    std::string message
)
{
    crow::json::wvalue json;

    json["success"] = false;
    json["message"] = message;

    return crow::response(
        code,
        json
    );
}