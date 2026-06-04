#pragma once

#include "crow.h"
#include "cloud_utils.h"

#include <mysql/mysql.h>
#include <string>

template<typename App>
void registerRegisterRoute(
    App &app,
    MYSQL *conn)
{
    CROW_ROUTE(
        app,
        "/register/<string>/<string>")
        .methods("POST"_method)

            ([conn](
                 std::string username,
                 std::string password)
             {
        std::string safeUsername =
            escapeSQL(
                conn,
                username
            );

        std::string safePassword =
            escapeSQL(
                conn,
                password
            );

        std::string query =
            "INSERT INTO users "
            "(username,password) "
            "VALUES ('" +
            safeUsername +
            "','" +
            safePassword +
            "')";

        if(
            mysql_query(
                conn,
                query.c_str()
            )
        )
        {
            return errorResponse(
                500,
                "Registration failed"
            );
        }

        crow::json::wvalue json;

        json["success"] = true;
        json["message"] = "User created";

        return crow::response(
            json
        ); });
}