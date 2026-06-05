#pragma once

#include "crow.h"
#include "cloud_utils.h"
#include "sessions.h"

#include <mysql/mysql.h>
#include <fstream>
#include <string>

template<typename App>
void registerUploadRoute(
    App &app,
    MYSQL *conn)
{

    CROW_ROUTE(
        app,
        "/upload/<string>")
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

                 if (!safe(filename))
                 {
                    return errorResponse(
                        400,
                        "Invalid filename"
                    );
                 }

                std::string category = getCategory(filename);

                std::string path = "uploads/" + category + "/" + filename;
                     
                std::ofstream file(path);

                if (!file)
                {
                    return errorResponse(
                        500,
                        "Cannot create file");
                }

                file << req.body;

                size_t fileSize = req.body.size();
                file.close();

                 std::string safeFilename = escapeSQL(conn, filename);

                 std::string safePath = escapeSQL(conn, path);

                 std::string safeCategory = escapeSQL(conn, category);

                 std::string query =
                     "INSERT INTO files "
                     "(filename,path,size,category,user_id) "
                     "VALUES ('" + safeFilename + "','" + safePath + "'," + std::to_string(fileSize) + ",'" + safeCategory + "'," + std::to_string(userId) + ")";


                 if (
                     mysql_query(
                         conn,
                         query.c_str()))
                 {
                     return errorResponse(
                         500,
                         "Database error");
                 }

                 crow::json::wvalue json;

                 json["success"] = true;
                 json["message"] = "Saved";

                 return crow::response(
                     json); });
}
