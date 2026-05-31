#pragma once

#include "crow.h"
#include "cloud_utils.h"

#include <mysql/mysql.h>
#include <string>

void registerFilesRoute(
    crow::SimpleApp &app,
    MYSQL *conn
)
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

        while(
            (row =
                mysql_fetch_row(
                    result
                )
            )
        )
        {
            output += "ID: ";
            output += row[0];
            output += "\n";

            output += "Filename: ";
            output += row[1];
            output += "\n";

            output += "Size: ";
            output +=
            (
                row[3]
                ? row[3]
                : "NULL"
            );
            output += "\n";

            output += "Uploaded: ";
            output += row[4];
            output += "\n";

            output +=
                "-------------------\n";
        }

        mysql_free_result(
            result
        );

        return crow::response(
            200,
            output
        );
    });
}