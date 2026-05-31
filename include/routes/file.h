#pragma once

#include "crow.h"
#include "cloud_utils.h"

#include <mysql/mysql.h>
#include <string>

void registerFileRoute(
    crow::SimpleApp &app,
    MYSQL *conn
)
{
    CROW_ROUTE(app, "/file/<string>")
    .methods("GET"_method)

    ([conn](std::string filename)
    {
        if(!safe(filename))
        {
            return crow::response(
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
            "SELECT * FROM files "
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

        MYSQL_ROW row =
            mysql_fetch_row(
                result
            );

        if(row == NULL)
        {
            mysql_free_result(
                result
            );

            return crow::response(
                404,
                "File not found"
            );
        }

        std::string output;

        output += "ID: ";
        output += row[0];
        output += "\n";

        output += "Filename: ";
        output += row[1];
        output += "\n";

        output += "Path: ";
        output += row[2];
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

        mysql_free_result(
            result
        );

        return crow::response(
            200,
            output
        );
    });
}