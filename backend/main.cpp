#include "config.hpp"
#include "routes.hpp"

#include <cstdlib>
#include <curl/curl.h>
#include <iostream>
#include <sodium.h>

int main()
{
    if (sodium_init() < 0)
    {
        return 1;
    }
    if (GEMINI_API_KEY.empty())
    {
        std::cerr << "CRITICAL ERROR: GEMINI_API_KEY is not set in the environment!" << std::endl;
        return 1;
    }
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK)
    {
        std::cerr << "Failed to initialize curl" << std::endl;
        return 1;
    }

    crow::FileStore sessionStore("./sessionData");
    crow::App<crow::CORSHandler, crow::CookieParser, Session> app{Session{sessionStore}};

    auto &cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .origin("http://localhost:5173")
        .methods("POST"_method, "GET"_method, "DELETE"_method, "OPTIONS"_method)
        .allow_credentials()
        .headers("Content-Type", "Authorization", "Accept", "ChatID");

    app.loglevel(crow::LogLevel::Debug);

    CROW_ROUTE(app, "/")([](){ return "This is just a test!"; });

    setupRoutes(app);

    char *envPort = std::getenv("PORT");
    int port = (envPort) ? std::stoi(envPort) : 18080;
    app.bindaddr("0.0.0.0").port(port).multithreaded().run();
    curl_global_cleanup();
}