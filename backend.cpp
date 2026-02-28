#define CROW_MAIN
#include "crow_all.h"

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return "Hello from Cross-Platform Crow!";
    });

    app.port(18080).multithreaded().run();
}