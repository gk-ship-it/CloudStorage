#pragma once

#include "crow.h"
#include <fstream>
#include <sstream>
#include <mysql/mysql.h>
#include "cloud_utils.h"
#include "sessions.h"

template<typename App>
void registerDownloadRoute(
    App &app,
    MYSQL *conn)
{
    CROW_ROUTE(app, "/download/<string>")
        .methods("GET"_method)

            ([conn](
                const crow::request &req,
                std::string filename)
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
                    "SELECT path "
                    "FROM files "
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
            
                std::ifstream file(
                    path
                );
            
                if(!file)
                {
                    return errorResponse(
                        404,
                        "File not found"
                    );
                }
            
                std::stringstream buffer;
            
                buffer << file.rdbuf();
            
                return crow::response(
                    200,
                    buffer.str()
        ); });
}