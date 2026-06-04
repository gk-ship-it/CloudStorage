#pragma once

#include "crow.h"
#include "cloud_utils.h"
#include "sessions.h"

#include <mysql/mysql.h>
#include <cstdio>
#include <string>

template<typename App>
void registerDeleteRoute(
    App &app,
    MYSQL *conn)
{
    CROW_ROUTE(
        app,
        "/delete/<string>")
        .methods("POST"_method)

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
            
            std::string selectQuery =
                "SELECT path "
                "FROM files "
                "WHERE filename='" + safeFilename + "' AND user_id = " + std::to_string(userId) + ";";
            
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
            "WHERE filename='" + safeFilename + "' AND user_id = " + std::to_string(userId) + ";";

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