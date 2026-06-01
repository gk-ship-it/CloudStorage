#include <mysql/mysql.h>
#include <iostream>

#include "routes/download.h"
#include "routes/files.h"
#include "routes/file.h"
#include "routes/rename.h"
#include "routes/delete.h"
#include "routes/upload.h"

#include "crow.h"

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

    result = mysql_real_connect(
        conn,
        "localhost",
        "root",
        "Gk13(18)",
        "cloud_storage",
        0,
        NULL,
        0);

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

    crow::SimpleApp app;
    registerDownloadRoute(app, conn);
    registerFilesRoute(app, conn);
    registerFileRoute(app, conn);
    registerRenameRoute(app, conn);
    registerDeleteRoute(app, conn);
    registerUploadRoute(app, conn);

    CROW_ROUTE(app, "/")
    ([]()
     { return "Home"; });

    app.port(18080).run();
}
