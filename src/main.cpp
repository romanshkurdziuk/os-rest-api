#include "crow.h"

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]{
        return "Hello, World! This is C++ REST API.";
    });
    app.port(18080).multithreaded().run();
}