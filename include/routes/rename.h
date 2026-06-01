#pragma once

#include "crow.h"
#include "cloud_utils.h"

#include <mysql/mysql.h>
#include <cstdio>
#include <string>

void registerRenameRoute(
    crow::SimpleApp &app,
    MYSQL *conn)
{
    CROW_ROUTE(
        app,
        "/rename/<string>/<string>")
        .methods("POST"_method)

            ([conn](
                 std::string oldName,
                 std::string newName)
             {
                 if (
                     !safe(oldName) ||
                     !safe(newName))
                 {
                     return errorResponse(
                         400,
                         "Invalid filename");
                 }

                std::string safeOldName =
                    escapeSQL(
                        conn,
                        oldName
                    );
                
                std::string selectquery =
                    "SELECT path, category "
                    "FROM files "
                    "WHERE filename='" +
                    safeOldName +
                    "'";
                
                if(
                    mysql_query(
                        conn,
                        selectquery.c_str()
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

                std::string oldPath =
                     row[0];

                std::string category =
                     row[1];
                
                mysql_free_result(
                    result
                );
                
                std::string newPath =
                    "../uploads/" +
                    category +
                    "/" +
                    newName;

                 if (
                     std::rename(
                         oldPath.c_str(),
                         newPath.c_str()) != 0)
                 {
                     return errorResponse(
                         404,
                         "Rename failed");
                 }

                 std::string safeNewName =
                     escapeSQL(
                         conn,
                         newName);

                 std::string safeNewPath =
                     escapeSQL(
                         conn,
                         newPath);

                 std::string updatequery =
                     "UPDATE files "
                     "SET filename='" +
                     safeNewName +
                     "', path='" +
                     safeNewPath +
                     "' "
                     "WHERE filename='" +
                     safeOldName +
                     "'";

                 if (
                     mysql_query(
                         conn,
                         updatequery.c_str()))
                 {
                     return errorResponse(
                         500,
                         "Database update error");
                 }

                 crow::json::wvalue json;

                 json["success"] = true;
                 json["message"] = "File renamed";

                 return crow::response(
                     json); });
}