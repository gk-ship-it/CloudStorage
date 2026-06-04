
#include <unordered_map>
#include <string>

#include <mysql/mysql.h>
#include <iostream>

#include "routes/download.h"
#include "routes/files.h"
#include "routes/file.h"
#include "routes/rename.h"
#include "routes/delete.h"
#include "routes/upload.h"
#include "routes/register.h"
#include "routes/login.h"
#include "routes/change_password.h"

#include "crow.h"
#include "crow/middlewares/cors.h"

const char *dbPassword =
    std::getenv(
        "DB_PASSWORD"
    );

std::unordered_map<std::string, int> sessions;

int main()
{
    MYSQL *conn;

    conn = mysql_init(NULL);

    if (conn == NULL)
    {
        std::cout
            << "mysql_init failed\n";

        return 1;
    }

    MYSQL *result;

    if(dbPassword == nullptr)
    {
        std::cout
            << "DB_PASSWORD not set"
            << std::endl;

        return 1;
    }

    result = mysql_real_connect(
        conn,
        "localhost",
        "root",
        dbPassword,//here put your password and dbPassword towork like previous
        "cloud_storage",
        0,
        NULL,
        0
    );

    if (result == NULL)
    {
        std::cout
            << "Connection failed\n";

        std::cout
            << mysql_error(conn)
            << std::endl;

        return 1;
    }

    std::cout
        << "MySQL Connected\n";

    // crow::SimpleApp app;
    crow::App<crow::CORSHandler> app;
    app.get_middleware<crow::CORSHandler>()
    .global()
    .origin("*")
    .headers(
        "Authorization",
        "Content-Type"
    )
    .methods(
        crow::HTTPMethod::GET,
        crow::HTTPMethod::POST,
        crow::HTTPMethod::OPTIONS
    );
    
    registerDownloadRoute(app, conn);
    registerFilesRoute(app, conn);
    registerFileRoute(app, conn);
    registerRenameRoute(app, conn);
    registerDeleteRoute(app, conn);
    registerUploadRoute(app, conn);
    registerRegisterRoute(app,conn);
    registerLoginRoute(app, conn);
    registerChangePasswordRoute(app, conn);

    CROW_ROUTE(app, "/")
    ([]()
     { return "Home"; });

    app.port(18080).run();
}
