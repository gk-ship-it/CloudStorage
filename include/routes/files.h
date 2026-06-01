#pragma once

#include "crow.h"
#include "cloud_utils.h"

#include <mysql/mysql.h>
#include <string>

void registerFilesRoute(
    crow::SimpleApp &app,
    MYSQL *conn)
{
    CROW_ROUTE(app, "/files")
        .methods("GET"_method)

            ([conn]()
             {
        if(
            mysql_query(
                conn,
                "SELECT * FROM files"
            )
        )
        {
            return crow::response(
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
            return crow::response(
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
        );    
    });
}