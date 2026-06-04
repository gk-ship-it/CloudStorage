#pragma once

#include "crow.h"
#include "cloud_utils.h"
#include "sessions.h"

#include <mysql/mysql.h>
#include <string>
template<typename App>
void registerFilesRoute(
    App &app,
    MYSQL *conn)
{
    CROW_ROUTE(app, "/files")
        .methods("GET"_method)

            ([conn](
                 const crow::request &req)
            {
                std::string token =
                req.get_header_value(
                    "Authorization"
                );
            
                auto it =
                sessions.find(
                    token
                );
                if (it == sessions.end())
                {
                    return errorResponse(
                         401,
                         "Unauthorized access");
                }
                int userId =it->second;

                std::string query = "SELECT * FROM files WHERE user_id = " + std::to_string(userId) + ";";
            
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

        MYSQL_ROW row;

        std::string output;
        crow::json::wvalue json;

        int index = 0;

        while(
            (row =
                mysql_fetch_row(
                    result
                )
            )
        )
        {
            json[index]["id"] =
                row[0];
        
            json[index]["filename"] =
                row[1];
                
            json[index]["category"] =
            (
                row[5]
                ? row[5]
                : "others"
            );
        
            json[index]["size"] =
            (
                row[3]
                ? row[3]
                : "NULL"
            );
        
            json[index]["uploaded"] =
                row[4];
        
            index++;
        }

        mysql_free_result(
            result
        );

        return crow::response(
            json
        ); });
}