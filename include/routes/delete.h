#pragma once

#include "crow.h"
#include "cloud_utils.h"

#include <mysql/mysql.h>
#include <cstdio>
#include <string>

void registerDeleteRoute(
    crow::SimpleApp &app,
    MYSQL *conn
)
{
    CROW_ROUTE(
        app,
        "/delete/<string>"
    )
    .methods("POST"_method)

    ([conn](std::string filename)
    {
        if(!safe(filename))
        {
            return crow::response(
                400,
                "Invalid filename"
            );
        }

        std::string path =
            "../uploads/" +
            filename;

        if(
            std::remove(
                path.c_str()
            ) != 0
        )
        {
            return crow::response(
                404,
                "File not found"
            );
        }

        std::string safeFilename =
            escapeSQL(
                conn,
                filename
            );

        std::string query =
            "DELETE FROM files "
            "WHERE filename='" +
            safeFilename +
            "'";

        if(
            mysql_query(
                conn,
                query.c_str()
            )
        )
        {
            return crow::response(
                500,
                "Database error"
            );
        }

        return crow::response(
            200,
            "File deleted"
        );
    });
}