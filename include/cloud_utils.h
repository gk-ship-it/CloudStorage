#pragma once

#include "crow.h"
#include <mysql/mysql.h>
#include <string>
#include <random>

bool safe(std::string name)
{
    return name.find("..") == std::string::npos &&
       name.find("/")  == std::string::npos &&
       name.find("\\") == std::string::npos;
}

std::string getCategory(
    std::string filename
)
{
    size_t dot =
        filename.find_last_of('.');

    if(dot == std::string::npos)
    {
        return "others";
    }

    std::string ext =
        filename.substr(dot + 1);

    if(
        ext == "txt"  ||
        ext == "pdf"  ||
        ext == "doc"  ||
        ext == "docx" ||
        ext == "xls"  ||
        ext == "xlsx" ||
        ext == "ppt"  ||
        ext == "pptx"
    )
    {
        return "documents";
    }

    if(
        ext == "jpg"  ||
        ext == "jpeg" ||
        ext == "png"  ||
        ext == "gif"  ||
        ext == "webp" ||
        ext == "bmp"
    )
    {
        return "photos";
    }

    if(
        ext == "mp4" ||
        ext == "avi" ||
        ext == "mkv" ||
        ext == "mov" ||
        ext == "webm"
    )
    {
        return "videos";
    }

    if(
        ext == "zip" ||
        ext == "rar" ||
        ext == "7z"  ||
        ext == "tar" ||
        ext == "gz"
    )
    {
        return "archives";
    }

    return "others";
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

std::string generateToken()
{
    std::string characters =
        "0123456789abcdef";

    std::string token;
    std::random_device rd;
        std::mt19937 generator(
            rd()
        );
    std::uniform_int_distribution<> dist(
            0,
            characters.size()-1
        );
    for (int i = 0; i < 16; i++)
    {
        int index =dist(generator);
        token += characters[index];
    }
    
    return token;
}