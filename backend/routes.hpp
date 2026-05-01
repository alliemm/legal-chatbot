#pragma once

#include "crow.h"
#include "crow/middlewares/session.h"
#include "crow/middlewares/cors.h"

using Session = crow::SessionMiddleware<crow::FileStore>;

void setupRoutes(crow::App<crow::CORSHandler, crow::CookieParser, Session> &app);
