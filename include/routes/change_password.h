#pragma once

#include "crow.h"
#include "cloud_utils.h"
#include "sessions.h"

#include <mysql/mysql.h>
#include <string>

template<typename App>
void registerChangePasswordRoute(
    App &app,
    MYSQL *conn)
{
    CROW_ROUTE(
        app,
        "/change-password/<string>/<string>")
        .methods("POST"_method)

        ([conn](
            const crow::request &req,
            std::string oldPassword,
            std::string newPassword)
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

            
            std::string safeOldPassword =
                escapeSQL(
                    conn,
                    oldPassword
                );
            std::string query =
                "SELECT id "
                "FROM users "
                "WHERE id = " +
                std::to_string(userId) +
                " AND password = '" +
                safeOldPassword +
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
                    401,
                    "Old password incorrect"
                );
            }
            mysql_free_result(
                result
            );
            std::string safeNewPassword =
                escapeSQL(
                    conn,
                    newPassword
                );
            std::string updateQuery =
                "UPDATE users "
                "SET password='" +
                safeNewPassword +
                "' "
                "WHERE id = " +
                std::to_string(userId);
            if(
                mysql_query(
                    conn,
                    updateQuery.c_str()
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
            json["message"] =
                "Password changed";

            return crow::response(
                json
            );
        });
}