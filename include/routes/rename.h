#pragma once

#include "crow.h"
#include "cloud_utils.h"

#include <mysql/mysql.h>
#include <cstdio>
#include <string>

void registerRenameRoute(
    crow::SimpleApp &app,
    MYSQL *conn
)
{
    CROW_ROUTE(
        app,
        "/rename/<string>/<string>"
    )
    .methods("POST"_method)

    ([conn](
        std::string oldName,
        std::string newName
    )
    {
        if(
            !safe(oldName)
            ||
            !safe(newName)
        )
        {
            return errorResponse(
    400,
    "Invalid filename"
);
        }

        std::string oldPath =
            "../uploads/" + oldName;

        std::string newPath =
            "../uploads/" + newName;

        if(
            std::rename(
                oldPath.c_str(),
                newPath.c_str()
            ) != 0
        )
        {
            return errorResponse(
    404,
    "Rename failed"
);
        }

        std::string safeOldName =
            escapeSQL(
                conn,
                oldName
            );

        std::string safeNewName =
            escapeSQL(
                conn,
                newName
            );

        std::string safeNewPath =
            escapeSQL(
                conn,
                newPath
            );

        std::string query =
            "UPDATE files "
            "SET filename='" +
            safeNewName +
            "', path='" +
            safeNewPath +
            "' "
            "WHERE filename='" +
            safeOldName +
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
    "Database update error"
);
        }

        crow::json::wvalue json;

        json["success"] = true;
        json["message"] = "File renamed";

        return crow::response(
            json
        );

    });
}