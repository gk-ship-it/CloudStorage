#pragma once

#include "crow.h"
#include "cloud_utils.h"

#include <mysql/mysql.h>
#include <fstream>
#include <string>

void registerUploadRoute(
    crow::SimpleApp &app,
    MYSQL *conn
)
{
    CROW_ROUTE(
        app,
        "/upload/<string>"
    )
    .methods("POST"_method)

    ([conn](
        const crow::request &req,
        std::string filename
    )
    {
        if(!safe(filename))
        {
            return errorResponse(
    400,
    "Invalid filename"
);
        }

        std::ofstream file(
            "../uploads/" +
            filename
        );

        if(!file)
        {
            return errorResponse(
    500,
    "Cannot create file"
);
        }

        file << req.body;

        size_t fileSize =
            req.body.size();

        file.close();

        std::string path =
            "../uploads/" +
            filename;

        std::string safeFilename =
            escapeSQL(
                conn,
                filename
            );

        std::string safePath =
            escapeSQL(
                conn,
                path
            );

        std::string query =
            "INSERT INTO files "
            "(filename,path,size) "
            "VALUES ('" +
            safeFilename +
            "','" +
            safePath +
            "'," +
            std::to_string(
                fileSize
            ) +
            ")";

        if(
            mysql_query(
                conn,
                query.c_str()
            )
        )
        {
            return errorResponse(
    500,
    "Database error"
);
        }

        crow::json::wvalue json;

        json["success"] = true;
        json["message"] = "Saved";

        return crow::response(
            json
        );


    });
}