#pragma once

#include "crow.h"
#include "cloud_utils.h"

#include <mysql/mysql.h>
#include <cstdio>
#include <string>

void registerDeleteRoute(
    crow::SimpleApp &app,
    MYSQL *conn)
{
    CROW_ROUTE(
        app,
        "/delete/<string>")
        .methods("POST"_method)

            ([conn](std::string filename)
             {
        if(!safe(filename))
        {
            return errorResponse(
                400,
                "Invalid filename"
            );
                    }
                
                    std::string safeFilename =
                escapeSQL(
                    conn,
                    filename
                );
            
            std::string selectQuery =
                "SELECT path "
                "FROM files "
                "WHERE filename='" +
                safeFilename +
                "'";
            
            if(
                mysql_query(
                    conn,
                    selectQuery.c_str()
                )
            )
            {
                return errorResponse(
                    500,
                    "Database error"
                );
            }

            MYSQL_RES *result =
                mysql_store_result(
                    conn
                );
            
            if(result == NULL)
            {
                return errorResponse(
                    500,
                    "Result error"
                );
            }

            MYSQL_ROW row =
                mysql_fetch_row(
                    result
                );
            
            if(row == NULL)
            {
                mysql_free_result(
                    result
                );
            
                return errorResponse(
                    404,
                    "File not found"
                );
            }

            std::string path =
                row[0];

            mysql_free_result(
                result
            );

        if(
            std::remove(
                path.c_str()
            ) != 0
        )
        {
            return errorResponse(
            404,
            "File Not Found"
        );
        }

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
            return errorResponse(
    500,
    "Database error"
);
        }

        crow::json::wvalue json;

        json["success"] = true;
        json["message"] = "File deleted";

        return crow::response(
            json
        ); });
}