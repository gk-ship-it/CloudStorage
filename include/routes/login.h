#pragma once

#include "crow.h"
#include "cloud_utils.h"
#include "sessions.h"

#include <mysql/mysql.h>
#include <string>

template <typename App>
void registerLoginRoute(
    App &app,
    MYSQL *conn)
{
    CROW_ROUTE(
        app,
        "/login/<string>/<string>")
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
                    "SELECT id "
                    "FROM users "
                    "WHERE username ='" + safeUsername + "' AND password = '" + safePassword + "'";
                    
                if (mysql_query(
                       conn,
                       query.c_str()))
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
                        "Invalid credentials"
                    );
                }

                int userId =
                    std::stoi(row[0]);

                std::string token = generateToken();

                sessions[token] = userId;

                std::string updatequery =
                    "UPDATE users SET token='" +
                    token +
                    "' WHERE id=" +
                    std::to_string(userId);

                mysql_query(conn, updatequery.c_str());


                crow::json::wvalue json;
                
                json["success"] = true;
                json["message"] = "Login successful";
                json["userId"] = userId;
                json["token"] = token;

                mysql_free_result(
                    result
                );
                 return crow::response(
                     json); }

            );
}