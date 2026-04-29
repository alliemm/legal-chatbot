#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "crow.h"
#include "crow/middlewares/session.h"
#include "crow/middlewares/cors.h"
#include "libenvpp/env.hpp"
#include <curl/curl.h>
#include <pqxx/pqxx>
#include <sodium.h>

const std::string CONNECTION_STRING = std::getenv("LEGALCHATBOT_CONSTRING");
const char* env_key = std::getenv("GEMINI_API_KEY");
const std::string GEMINI_API_KEY = (env_key) ? env_key : "";
const std::string GEMINI_MODEL = "gemini-3-flash-preview";
const std::string GEMINI_ENDPOINT =
    "https://generativelanguage.googleapis.com/v1beta/models/" + GEMINI_MODEL + ":generateContent";
const std::string GEMINI_SYSTEM_INSTRUCTION = R"(
## ROLE
You are LexAssist, a specialized AI legal document assistant.

##CONTEXT
The user will provide legal documents (PDFs, TXT) and ask questions. You have access to these files via 'inlineData'.

##OBJECTIVES
1. Summarize complex legal jargon into succinct and plain English.
2. Identify key dates, parties, and financial obligations.
3. Cite the document name and section number for every claim you make.

##CONSTRAINTS
- NEVER provide actual legal advice
- ALWAYS include a disclaimer at the beginning of conversations: "I am an AI, not a lawyer. Consult a legal professional for binding advice."
- If the answer isn't in the provided document, say, "I cannot find this information in the current files."

##DEMEANOR
Please retain a professional demeanor, but not overly technical. )";

struct ChatMessage
{
    std::string role;
    std::string text;
};

struct StoredDocument
{
    std::string name;
    std::string mimeType;
    std::string base64Data;
};

struct GeminiHttpResponse
{
    long statusCode = 0;
    std::string body;
    std::string error;
};

std::unique_ptr<pqxx::connection> connectToDatabase();
bool checkPassword(const std::string &email, const std::string &password);
pqxx::result getUserPreference(const std::string &email);
std::vector<ChatMessage> getChatHistory(const std::string &email, const std::string& chatid);
bool storeChatMessage(const std::string &email, const std::string& chatid, const std::string& role, const std::string& message);
std::string normalizeChatRole(const std::string &role);
std::string guessMimeType(const std::string &fileName);
std::string base64Encode(const unsigned char *data, size_t size);
std::vector<StoredDocument> loadDocuments(
    const std::string &email,
    const std::vector<std::string> &requestedNames,
    std::vector<std::string> &missingDocuments);
std::string buildGeminiRequestBody(
    const std::string &message,
    const std::vector<ChatMessage> &history,
    const std::vector<StoredDocument> &documents);
GeminiHttpResponse sendGeminiRequest(const std::string &requestBody);
std::string extractGeminiReply(const std::string &responseBody);
std::string extractGeminiError(const std::string &responseBody);
size_t writeCurlResponse(void *contents, size_t size, size_t nmemb, void *userp);

int main()
{
    // initialize sodium library
    if (sodium_init() < 0)
    {
        return 1;
    }
    if (GEMINI_API_KEY.empty()) {
        std::cerr << "CRITICAL ERROR: GEMINI_API_KEY is not set in the environment!" << std::endl;
        return 1; // stop the program if the key is missing
    }
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK)
    {
        std::cerr << "Failed to initialize curl" << std::endl;
        return 1;
    }
    // setup sessions
    // using file store so that session data is stored in json files
    using Session = crow::SessionMiddleware<crow::FileStore>;
    crow::FileStore sessionStore("./sessionData");
    // create app
    crow::App<crow::CookieParser, Session, crow::CORSHandler> app{Session{sessionStore}};

    //setup cors
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .origin("*") //For now this accepts everything since we don't have the frontend url yet
        .methods("POST"_method, "GET"_method, "DELETE"_method, "OPTIONS"_method)
        .headers("Content-Type", "Authorization", "Accept");

    app.loglevel(crow::LogLevel::Debug);

    CROW_ROUTE(app, "/")([](){ return "This is just a test!"; });

    // signup
    CROW_ROUTE(app, "/signup").methods("POST"_method, "OPTIONS"_method)([&app](const crow::request &req)
                                                      {
        if (req.method == "OPTIONS"_method)
        {
            return crow::response(204);
        }
        auto data = crow::json::load(req.body);
        //if there is no data or if the data is missing the email or password field return error
        if (!data || !data.has("password") || !data.has("email") || !data.has("name"))
        {
            return crow::response(400, "Missing signup information");
        }
        //get the email, password, and name values
        std::string password = data["password"].s();
        std::string email = data["email"].s();
        std::string name = data["name"].s();
        //check if the email is already taken and return error/message saying that email is taken
        auto conn = connectToDatabase();
        if (!conn)
        {
            return crow::response(500, "Failed to connect to database");
        }
        //start a transaction
        pqxx::work transaction(*conn);
        //run query to check if email exists in database
        pqxx::result res = transaction.exec("SELECT * FROM users WHERE email = $1", pqxx::params{email});
        //if the email isn't taken then insert the user into the database
        if (res.empty())
        {
            //hash password
            char hashed[crypto_pwhash_STRBYTES];
            int hashRes = crypto_pwhash_str(hashed, password.c_str(), password.length(),
                                            crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE);
            if (hashRes != 0)
            {
                std::cerr << "Failed to hash password" << std::endl;
                return crow::response(500, "Failed to hash password");
            }
            //convert hashed password to a string
            std::string finalHash = std::string(hashed);
            //add new user information into database
            transaction.exec("INSERT INTO users VALUES ($1, $2, $3)", pqxx::params{email, finalHash, name});
            //end transaction
            transaction.commit();
            //store the current user as authenticated
            auto& session = app.get_context<Session>(req);
            session.set("user", std::string(email));
            return crow::response(201, "Created new account");
        }
        else
        {
            //since it failed abort the transaction
            transaction.abort();
            return crow::response(400, "This email address is already taken");
        } });

    // login
    CROW_ROUTE(app, "/login").methods("POST"_method, "OPTIONS"_method)([&app](const crow::request &req)
                                                     {
        if (req.method == "OPTIONS"_method)
        {
            return crow::response(204);
        }
        auto data = crow::json::load(req.body);
        //if there is no data or if the data is missing the username or password or email field return error
        if (!data || !data.has("email") || !data.has("password"))
        {
            return crow::response(400, "Missing login information");
        }
        std::string email = data["email"].s();
        std::string password = data["password"].s();
        //verify that the user exists and used the correct password
        if (checkPassword(email, password))
        {
            auto& session = app.get_context<Session>(req);
            session.set("user", std::string(email));

            return crow::response(202, "User logged in");
        }
        else
        {
            return crow::response(400, "Incorrect login information");
        } });
    // logout
    CROW_ROUTE(app, "/logout")([&app](const crow::request &req)
                               {
        auto& session = app.get_context<Session>(req);
        //return error since user isn't logged
        if (session.get("user", "").empty())
        {
            return crow::response(400, "Already logged out");
        }
        std::cout << "Logging out" << std::endl;
        //remove user from session
        session.remove("user");
        //return success
        return crow::response(200, "User logged out"); });
    // delete account
    CROW_ROUTE(app, "/deactivate").methods("DELETE"_method, "OPTIONS"_method)([&app](const crow::request &req)
                                                            {
        auto& session = app.get_context<Session>(req);
        if (req.method == "OPTIONS"_method)
        {
            return crow::response(204);
        }
        std::string email = session.get("user", "");
        //if the user isn't logged in then respond with error
        if (email.empty())
        {
            return crow::response(401, "Unauthorized");
        }
        //else delete the user from all tables in the database
        try
        {
            //connect to database
            auto conn = connectToDatabase();
            if (!conn)
            {
                return crow::response(500, "Failed to connect to database");
            }
            //start a transaction
            pqxx::work transaction(*conn);
            //run query to delete from all databases
            transaction.exec("DELETE FROM users WHERE email = $1", pqxx::params{email});
            transaction.exec("DELETE FROM surveys WHERE email = $1", pqxx::params{email});
            transaction.exec("DELETE FROM documents WHERE email = $1", pqxx::params{email});
            //commit the transaction
            transaction.commit();
            return crow::response(200, "Account Deleted");
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return crow::response(500, "Failed to delete account");
        } });

    // survey
    CROW_ROUTE(app, "/survey").methods("POST"_method, "OPTIONS"_method)([&app](const crow::request &req)
    {
        if (req.method == "OPTIONS"_method)
        {
            return crow::response(204);
        }
        // ensure that user is logged in
        auto& session = app.get_context<Session>(req);
        std::string email = session.get("user", "");
        // if the user isn't logged in then respond with error
        if (email.empty())
        {
            return crow::response(401, "Unauthorized");
        }
        // get the body of the request
        auto data = crow::json::load(req.body);
        // if there is no data or if the data is missing any of the necessary fields return an error
        if (!data || !data.has("userType") || !data.has("workedWithLawyer") || !data.has("howOften") || !data.
            has("typesOfDocuments") || !data.has("biggestConcerns") || !data.has("jargonUnderstanding") || !data.
            has("outcome"))
        {
            return crow::response(400, "Missing survey information");
        }
        // get survey answers from request
        std::string userType = data["userType"].s();
        std::string workedWithLawyer = data["workedWithLawyer"].s();
        std::string howOften = data["howOften"].s();
        std::string typesOfDocuments = data["typesOfDocuments"].s();
        std::string biggestConcerns = data["biggestConcerns"].s();
        std::string jargonUnderstanding = data["jargonUnderstanding"].s();
        std::string outcome = data["outcome"].s();
        // connect to database
        try
        {
            auto conn = connectToDatabase();
            if (!conn)
            {
                return crow::response(500, "Failed to connect to database");
            }
            // start a transaction
            pqxx::work transaction(*conn);
            // run query to add user survey answers to database
            pqxx::result res = transaction.exec("INSERT INTO surveys VALUES ($1, $2, $3, $4, $5, $6, $7, $8)",
                                                pqxx::params{
                                                    email, userType, workedWithLawyer, howOften, typesOfDocuments,
                                                    biggestConcerns, jargonUnderstanding, outcome
                                                });
            // commit the transaction
            transaction.commit();
            return crow::response(200, "Saved survey results");
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return crow::response(500, "Failed to save survey results");
        }
    });

    // upload
    CROW_ROUTE(app, "/upload").methods("POST"_method, "OPTIONS"_method)([&app](const crow::request &req)
                                                      {
        if (req.method == "OPTIONS"_method)
        {
            return crow::response(204);
        }
        //ensure that user is logged in
        auto& session = app.get_context<Session>(req);
        std::string email = session.get("user", "");
        //if the user isn't logged in then respond with error
        if (email.empty())
        {
            return crow::response(401, "Unauthorized");
        }
        //get the file
        crow::multipart::message_view fileMessage(req);
        for (const auto& part : fileMessage.part_map)
        {
            const auto& partName = part.first;
            const auto& partValue = part.second;
            if ("InputFile" == partName)
            {
                auto fileHeaders = partValue.headers.find("Content-Disposition");
                if (fileHeaders == partValue.headers.end())
                {
                    return crow::response(400, "No content-disposition found");
                }
                auto params = fileHeaders->second.params.find("filename");
                if (params == fileHeaders->second.params.end())
                {
                    return crow::response(400, "No filename found");
                }
                //get the file name
                const std::string fileName{params->second};
                try
                {
                    //connect to database
                    auto conn = connectToDatabase();
                    if (!conn)
                    {
                        return crow::response(500, "Failed to connect to database");
                    }
                    //start a transaction
                    pqxx::work transaction(*conn);
                    auto fileData = pqxx::bytes_view{reinterpret_cast<const std::byte*>(part.second.body.data()), part.second.body.size()};
                    //run query to add user survey answers to database
                    pqxx::result res = transaction.exec("INSERT INTO documents VALUES ($1, $2, $3)",
                        pqxx::params{email, fileName, fileData});
                    //commit the transaction
                    transaction.commit();
                    break;
                }
                catch (std::exception& e)
                {
                    std::cerr << e.what() << std::endl;
                    return crow::response(500, "Internal error");
                }

            }
        }
        return crow::response(202, "Successfully uploaded file"); });

    // profile
    CROW_ROUTE(app, "/profile").methods("GET"_method)([&app](const crow::request &req)
    {
        // ensure that user is logged in
        auto& session = app.get_context<Session>(req);
        std::string email = session.get("user", "");
        // if the user isn't logged in then respond with error
        if (email.empty())
        {
            return crow::response(401, "Unauthorized");
        }
        try
        {
            auto conn = connectToDatabase();
            pqxx::nontransaction nonTransaction(*conn);
            // get the names of the user's documents
            pqxx::result docs = nonTransaction.exec("SELECT name FROM documents WHERE email = $1", pqxx::params{email});
            // get the user's survey preference answers
            pqxx::result survey = nonTransaction.exec("SELECT * FROM surveys WHERE email = $1", pqxx::params{email});
            crow::json::wvalue response;
            response["email"] = email;

            // store the documents
            std::vector<std::string> documents;
            for (auto const& row : docs)
            {
                documents.push_back(row["name"].c_str());
            }
            response["documents"] = documents;
            // store the survey data
            if (!survey.empty())
            {
                crow::json::wvalue surveyData;
                for (auto const& column : survey[0])
                {
                    surveyData[column.name()] = column.c_str();
                }
                response["survey"] = std::move(surveyData);
            }
            else
            {
                response["survey"] = nullptr;
            }
            // return the name of the user's saved documents and the user's email
            return crow::response(response);
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return crow::response(500, "Failed to access profile information");
        }
    });

    // chat
    CROW_ROUTE(app, "/chat").methods("POST"_method, "OPTIONS"_method)([&app](const crow::request &req)
                                                    {
        if (req.method == "OPTIONS"_method)
        {
            return crow::response(204);
        }
        auto& session = app.get_context<Session>(req);
        std::string email = session.get("user", "");
        if (email.empty())
        {
            return crow::response(401, "Unauthorized");
        }

        auto data = crow::json::load(req.body);
        if (!data || !data.has("message"))
        {
            return crow::response(400, "Missing chat message");
        }
        if (!data.has("chatid"))
        {
            return crow::response(400, "Missing chat id");
        }
        const std::string chatid = data["chatid"].s();
        const std::string message = data["message"].s();
        if (message.empty())
        {
            return crow::response(400, "Chat message cannot be empty");
        }
        if (GEMINI_API_KEY == "replace-with-gemini-api-key")
        {
            return crow::response(500, "Gemini API key placeholder has not been replaced");
        }
        //get chat history from database
        std::vector<ChatMessage> history = getChatHistory(email, chatid);
        //save user's new message to database
        storeChatMessage(email, chatid, "user", message);

        std::vector<std::string> requestedDocuments;
        if (data.has("documentNames"))
        {
            for (size_t i = 0; i < data["documentNames"].size(); ++i)
            {
                requestedDocuments.push_back(data["documentNames"][i].s());
            }
        }

        try
        {
            std::vector<std::string> missingDocuments;
            const std::vector<StoredDocument> documents =
                loadDocuments(email, requestedDocuments, missingDocuments);

            if (!missingDocuments.empty())
            {
                return crow::response(404, "One or more requested documents were not found");
            }

            const std::string geminiRequestBody = buildGeminiRequestBody(message, history, documents);
            const GeminiHttpResponse geminiResponse = sendGeminiRequest(geminiRequestBody);

            if (!geminiResponse.error.empty())
            {
                return crow::response(502, geminiResponse.error);
            }

            if (geminiResponse.statusCode < 200 || geminiResponse.statusCode >= 300)
            {
                const std::string geminiError = extractGeminiError(geminiResponse.body);
                if (!geminiError.empty())
                {
                    return crow::response(502, geminiError);
                }
                return crow::response(502, "Gemini request failed");
            }

            const std::string reply = extractGeminiReply(geminiResponse.body);
            if (reply.empty())
            {
                return crow::response(502, "Gemini returned an empty response");
            }

            //store gemini's reply to the user in the database
            storeChatMessage(email, chatid, "model", reply);

            std::vector<std::string> usedDocuments;
            usedDocuments.reserve(documents.size());
            for (const auto& document : documents)
            {
                usedDocuments.push_back(document.name);
            }

            crow::json::wvalue response;
            response["reply"] = reply;
            response["usedDocuments"] = usedDocuments;
            return crow::response(response);
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return crow::response(500, "Failed to process chat request");
        } });

    char* envPort = std::getenv("PORT");
    int port = (envPort) ? std::stoi(envPort) : 18080;
    app.bindaddr("0.0.0.0").port(port).multithreaded().run();
    curl_global_cleanup();
}

std::unique_ptr<pqxx::connection> connectToDatabase()
{
    // try to connect to database
    try
    {
        auto conn = std::make_unique<pqxx::connection>(CONNECTION_STRING);
        if (conn->is_open())
        {
            std::cout << "Connection to database established!" << std::endl;
            return conn;
        }
        return nullptr;
    }
    catch (std::exception &e)
    {
        // if it failed to connect print out the error message and return a nullptr
        std::cerr << e.what() << std::endl;
        return nullptr;
    }
}

bool checkPassword(const std::string &email, const std::string &password)
{
    try
    {
        // attempt to connect to the database
        auto conn = connectToDatabase();
        if (!conn)
        {
            return false;
        }
        // start a transaction
        pqxx::nontransaction nonTransaction(*conn);
        // compare password in the database and the password the user entered
        std::cout << "Checking password" << std::endl;
        pqxx::result res = nonTransaction.exec("SELECT password FROM users WHERE email = $1", pqxx::params{email});
        if (res.empty())
        {
            return false;
        }
        else
        {
            std::string hashedPassword = res[0]["password"].as<std::string>();
            if (crypto_pwhash_str_verify(hashedPassword.c_str(), password.c_str(), password.length()) != 0)
            {
                // the password is incorrect so return false
                return false;
            }
            else
                return true;
        }
    }
    catch (std::exception &e)
    {
        // if we have an error printout a message and return false
        std::cerr << e.what() << std::endl;
        return false;
    }
}
pqxx::result getUserPreference(const std::string &email)
{
    try
    {
        auto conn = connectToDatabase();
        pqxx::nontransaction nonTransaction(*conn);
        // get the user's survey preference answers
        pqxx::result survey = nonTransaction.exec("SELECT * FROM surveys WHERE email = $1", pqxx::params{email});
        return survey;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        //if we run into an error return an empty result object
        return pqxx::result();
    }
}
std::vector<ChatMessage> getChatHistory(const std::string &email, const std::string& chatid)
{
    try
    {
        auto conn = connectToDatabase();
        pqxx::nontransaction nonTransaction(*conn);
        //retrieve all the messages in this chat in asc order so that oldest are first
        pqxx::result messages = nonTransaction.exec("SELECT * FROM chats WHERE email = $1 AND chatid = $2 ORDER BY time ASC", pqxx::params{email, chatid});
        std::vector<ChatMessage> chatHistory;
        //loop through the results of the query and add the messages to the chatHistory vector
        for (const auto& message : messages)
        {
            ChatMessage newMessage;
            newMessage.role = message["role"].as<std::string>();
            newMessage.text = message["content"].as<std::string>();
            chatHistory.push_back(newMessage);
        }
        //return the chatHistory vector
        return chatHistory;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        //if we fail for some reason then just return an empty vector
        return std::vector<ChatMessage>();
    }
}
bool storeChatMessage(const std::string &email, const std::string& chatid, const std::string& role, const std::string& message)
{
    try
    {
        auto conn = connectToDatabase();
        pqxx::work transaction(*conn);
        //add the new message to the database
        pqxx::result messages = transaction.exec("INSERT INTO chats (chatid, email, role, content) VALUES ($1, $2, $3, $4)", pqxx::params{chatid, email, role, message});
        //commit the transaction
        transaction.commit();
        return true;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
}
std::string normalizeChatRole(const std::string &role)
{
    std::string normalized = role;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c)
                   { return static_cast<char>(std::tolower(c)); });

    if (normalized == "user")
    {
        return "user";
    }
    if (normalized == "assistant" || normalized == "model")
    {
        return "model";
    }
    return "";
}

std::string guessMimeType(const std::string &fileName)
{
    const auto extensionPosition = fileName.find_last_of('.');
    if (extensionPosition == std::string::npos)
    {
        return "application/octet-stream";
    }

    std::string extension = fileName.substr(extensionPosition);
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char c)
                   { return static_cast<char>(std::tolower(c)); });

    if (extension == ".pdf")
    {
        return "application/pdf";
    }
    if (extension == ".txt")
    {
        return "text/plain";
    }
    if (extension == ".md")
    {
        return "text/markdown";
    }
    if (extension == ".doc")
    {
        return "application/msword";
    }
    if (extension == ".docx")
    {
        return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    }
    if (extension == ".rtf")
    {
        return "application/rtf";
    }

    return "application/octet-stream";
}

std::string base64Encode(const unsigned char *data, size_t size)
{
    if (size == 0)
    {
        return "";
    }

    std::string encoded(sodium_base64_ENCODED_LEN(size, sodium_base64_VARIANT_ORIGINAL), '\0');
    sodium_bin2base64(encoded.data(), encoded.size(), data, size, sodium_base64_VARIANT_ORIGINAL);
    encoded.resize(std::strlen(encoded.c_str()));
    return encoded;
}

std::vector<StoredDocument> loadDocuments(
    const std::string &email,
    const std::vector<std::string> &requestedNames,
    std::vector<std::string> &missingDocuments)
{
    auto conn = connectToDatabase();
    if (!conn)
    {
        throw std::runtime_error("Failed to connect to database");
    }

    pqxx::nontransaction nonTransaction(*conn);
    std::vector<StoredDocument> documents;

    if (requestedNames.empty())
    {
        pqxx::result rows = nonTransaction.exec("SELECT * FROM documents WHERE email = $1 ORDER BY name",
                                                pqxx::params{email});
        for (const auto &row : rows)
        {
            auto const binaryData = row[2].as<pqxx::bytes>();
            documents.push_back({row[1].c_str(),
                                 guessMimeType(row[1].c_str()),
                                 base64Encode(reinterpret_cast<const unsigned char*>(binaryData.data()), binaryData.size())});
        }
        return documents;
    }

    for (const auto &requestedName : requestedNames)
    {
        pqxx::result rows = nonTransaction.exec("SELECT * FROM documents WHERE email = $1 AND name = $2",
                                                pqxx::params{email, requestedName});
        if (rows.empty())
        {
            missingDocuments.push_back(requestedName);
            continue;
        }

        auto const binaryData = rows[0][2].as<pqxx::bytes>();
        documents.push_back({rows[0][1].c_str(),
                             guessMimeType(rows[0][1].c_str()),
                             base64Encode(reinterpret_cast<const unsigned char*>(binaryData.data()), binaryData.size())});
    }

    return documents;
}

std::string buildGeminiRequestBody(
    const std::string &message,
    const std::vector<ChatMessage> &history,
    const std::vector<StoredDocument> &documents)
{
    crow::json::wvalue payload;
    payload["systemInstruction"]["parts"][0]["text"] = GEMINI_SYSTEM_INSTRUCTION;

    for (size_t i = 0; i < history.size(); ++i)
    {
        payload["contents"][i]["role"] = history[i].role;
        payload["contents"][i]["parts"][0]["text"] = history[i].text;
    }

    const size_t currentMessageIndex = history.size();
    payload["contents"][currentMessageIndex]["role"] = "user";

    size_t currentPartIndex = 0;
    for (const auto &document : documents)
    {
        payload["contents"][currentMessageIndex]["parts"][currentPartIndex]["inlineData"]["mimeType"] = document.mimeType;
        payload["contents"][currentMessageIndex]["parts"][currentPartIndex]["inlineData"]["data"] = document.base64Data;
        ++currentPartIndex;
    }

    payload["contents"][currentMessageIndex]["parts"][currentPartIndex]["text"] = message;
    return payload.dump();
}

GeminiHttpResponse sendGeminiRequest(const std::string &requestBody)
{
    GeminiHttpResponse response;
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        response.error = "Failed to initialize Gemini HTTP client";
        return response;
    }

    std::string responseBody;
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    const std::string apiKeyHeader = "x-goog-api-key: " + GEMINI_API_KEY;
    headers = curl_slist_append(headers, apiKeyHeader.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, GEMINI_ENDPOINT.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(requestBody.size()));
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCurlResponse);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    const CURLcode curlCode = curl_easy_perform(curl);
    if (curlCode != CURLE_OK)
    {
        response.error = curl_easy_strerror(curlCode);
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.statusCode);
    response.body = std::move(responseBody);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return response;
}

std::string extractGeminiReply(const std::string &responseBody)
{
    auto data = crow::json::load(responseBody);
    if (!data || !data.has("candidates") || data["candidates"].size() == 0)
    {
        return "";
    }

    if (!data["candidates"][0].has("content") || !data["candidates"][0]["content"].has("parts"))
    {
        return "";
    }

    std::string reply;
    const auto &parts = data["candidates"][0]["content"]["parts"];
    for (size_t i = 0; i < parts.size(); ++i)
    {
        if (!parts[i].has("text"))
        {
            continue;
        }

        if (!reply.empty())
        {
            reply += "\n";
        }
        reply += parts[i]["text"].s();
    }

    return reply;
}

std::string extractGeminiError(const std::string &responseBody)
{
    auto data = crow::json::load(responseBody);
    if (data && data.has("error") && data["error"].has("message"))
    {
        return data["error"]["message"].s();
    }
    return responseBody;
}

size_t writeCurlResponse(void *contents, size_t size, size_t nmemb, void *userp)
{
    const size_t totalSize = size * nmemb;
    auto *buffer = static_cast<std::string *>(userp);
    buffer->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}