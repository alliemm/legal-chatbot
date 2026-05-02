#include "config.hpp"
#include <cstdlib>

const std::string CONNECTION_STRING = []()
{
    const char *env = std::getenv("LEGALCHATBOT_CONSTRING");
    return env ? env : "";
}();

const std::string GEMINI_API_KEY = []()
{
    const char *env = std::getenv("GEMINI_API_KEY");
    return env ? env : "";
}();

const std::string GEMINI_ENDPOINT =
    "https://generativelanguage.googleapis.com/v1beta/models/" + GEMINI_MODEL + ":generateContent";
