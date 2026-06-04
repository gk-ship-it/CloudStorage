#pragma once

#include "crow.h"
#include "cloud_utils.h"
#include "sessions.h"

#include <mysql/mysql.h>
#include <string>

template<typename App>
void registerFileRoute(
    App &app,
    MYSQL *conn)
{
    CROW_ROUTE(app, "/file/<string>")
        .methods("GET"_method)

            ([conn](
                 const crow::request &req,
                 std::string filename)
             {
                std::string token =
                req.get_header_value(
                    "Authorization"
                );
            
                int userId =
    getUserIdFromToken(
        conn,
        token
    );

if(userId == -1)
{
    crow::json::wvalue json;

    json["success"] = false;
    json["message"] =
        "Unauthorized access";

    return crow::response(
        401,
        json
    );
}

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
                
                std::string query =
                    "SELECT * FROM files WHERE filename = '" + safeFilename + "' AND user_id = " + std::to_string(userId) + ";";
                
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

        crow::json::wvalue json;

        json["id"] = row[0];
        json["filename"] = row[1];
        json["path"] = row[2];
        json["size"] =
        (
            row[3]
            ? row[3]
            : "NULL"
        );
        json["uploaded"] = row[4];

        mysql_free_result(
            result
        );

        return crow::response(
            json
        ); });
}