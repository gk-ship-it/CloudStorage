#pragma once

#include "crow.h"
#include <fstream>
#include <sstream>

#include "cloud_utils.h"

void registerDownloadRoute(
    crow::SimpleApp &app
)
{
    CROW_ROUTE(app, "/download/<string>")
    .methods("GET"_method)

    ([](std::string filename)
    {
        if(!safe(filename))
        {
            return crow::response(
                400,
                "Invalid filename"
            );
        }

        std::ifstream file(
            "../uploads/" + filename
        );

        if(!file)
        {
            return crow::response(
                404,
                "File not found"
            );
        }

        std::stringstream buffer;

        buffer << file.rdbuf();

        return crow::response(
            200,
            buffer.str()
        );
    });
}