#include "routes.hpp"
#include "db.hpp"
#include "gemini.hpp"

#include <iostream>
#include <sodium.h>

void setupRoutes(crow::App<crow::CORSHandler, crow::CookieParser, Session> &app)
{
    initDatabase();

    CROW_ROUTE(app, "/signup").methods("POST"_method)([&app](const crow::request &req)
    {
        auto data = crow::json::load(req.body);
        if (!data || !data.has("password") || !data.has("email") || !data.has("name"))
        {
            return crow::response(400, "Missing signup information");
        }
        std::string password = data["password"].s();
        std::string email = data["email"].s();
        std::string name = data["name"].s();
        auto conn = connectToDatabase();
        if (!conn)
        {
            return crow::response(500, "Failed to connect to database");
        }
        pqxx::work transaction(*conn);
        pqxx::result res = transaction.exec("SELECT * FROM users WHERE email = $1", pqxx::params{email});
        if (res.empty())
        {
            char hashed[crypto_pwhash_STRBYTES];
            int hashRes = crypto_pwhash_str(hashed, password.c_str(), password.length(),
                                            crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE);
            if (hashRes != 0)
            {
                std::cerr << "Failed to hash password" << std::endl;
                return crow::response(500, "Failed to hash password");
            }
            std::string finalHash = std::string(hashed);
            transaction.exec("INSERT INTO users VALUES ($1, $2, $3)", pqxx::params{email, finalHash, name});
            transaction.commit();
            crow::json::wvalue body;
            body["token"] = email;
            body["status"] = "success";
            return crow::response(201, body);
        }
        else
        {
            transaction.abort();
            crow::json::wvalue body;
            body["token"] = "";
            body["status"] = "failed";
            return crow::response(400, body);
        }
    });

    CROW_ROUTE(app, "/login").methods("POST"_method)([&app](const crow::request &req)
    {
        auto data = crow::json::load(req.body);
        if (!data || !data.has("email") || !data.has("password"))
        {
            return crow::response(400, "Missing login information");
        }

        std::string email = data["email"].s();
        std::string password = data["password"].s();
        if (checkPassword(email, password))
        {
            crow::json::wvalue body;
            body["token"] = email;
            body["status"] = "success";
            return crow::response(202, body);
        }
        else
        {
            crow::json::wvalue body;
            body["token"] = "";
            body["status"] = "failed";
            return crow::response(400, body);
        }
    });

    CROW_ROUTE(app, "/logout")([&app](const crow::request &req)
    {
        std::string email = req.get_header_value("Authorization");
        if (email.empty())
        {
            return crow::response(400, "Already logged out");
        }
        std::cout << "Logging out" << std::endl;
        return crow::response(200, "User logged out");
    });

    CROW_ROUTE(app, "/deactivate").methods("DELETE"_method)([&app](const crow::request &req)
    {
        std::string email = req.get_header_value("Authorization");
        if (email.empty())
        {
            return crow::response(401, "Unauthorized");
        }
        try
        {
            auto conn = connectToDatabase();
            if (!conn)
            {
                return crow::response(500, "Failed to connect to database");
            }
            pqxx::work transaction(*conn);
            transaction.exec("DELETE FROM users WHERE email = $1", pqxx::params{email});
            transaction.exec("DELETE FROM surveys WHERE email = $1", pqxx::params{email});
            transaction.exec("DELETE FROM documents WHERE email = $1", pqxx::params{email});
            transaction.exec("DELETE FROM preferences WHERE email = $1", pqxx::params{email});
            transaction.commit();
            return crow::response(200, "Account Deleted");
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << std::endl;
            return crow::response(500, "Failed to delete account");
        }
    });

    CROW_ROUTE(app, "/survey").methods("POST"_method)([&app](const crow::request &req)
    {
        std::string email = req.get_header_value("Authorization");
        if (email.empty())
        {
            return crow::response(401, "Unauthorized");
        }
        auto data = crow::json::load(req.body);
        if (!data || !data.has("docFrequency") || !data.has("docTypes") || !data.has("concerns") ||
            !data.has("jargonComfort") || !data.has("workedWithLawyer"))
        {
            return crow::response(400, "Missing survey information");
        }
        std::string docFrequency = data["docFrequency"].s();
        std::string docTypes = data["docTypes"].s();
        std::string concerns = data["concerns"].s();
        std::string jargonComfort = data["jargonComfort"].s();
        std::string workedWithLawyer = data["workedWithLawyer"].s();
        if (saveSurvey(email, docFrequency, docTypes, concerns, jargonComfort, workedWithLawyer))
        {
            return crow::response(200, "Saved survey results");
        }
        return crow::response(500, "Failed to save survey results");
    });

    CROW_ROUTE(app, "/upload").methods("POST"_method)([&app](const crow::request &req)
    {
        std::string email = req.get_header_value("Authorization");
        if (email.empty())
        {
            return crow::response(401, "Unauthorized");
        }
        std::string chatid;
        crow::multipart::message_view fileMessage(req);
        for (const auto &part : fileMessage.part_map)
        {
            const auto &partName = part.first;
            const auto &partValue = part.second;
            if ("chatid" == partName)
            {
                chatid = partValue.body;
            }
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
                const std::string fileName{params->second};
                try
                {
                    auto conn = connectToDatabase();
                    if (!conn)
                    {
                        return crow::response(500, "Failed to connect to database");
                    }
                    pqxx::work transaction(*conn);
                    auto fileData = pqxx::bytes_view{reinterpret_cast<const std::byte *>(part.second.body.data()), part.second.body.size()};
                    pqxx::result res = transaction.exec("INSERT INTO documents VALUES ($1, $2, $3, $4)",
                                                        pqxx::params{email, fileName, fileData, chatid});
                    transaction.commit();
                    break;
                }
                catch (std::exception &e)
                {
                    std::cerr << e.what() << std::endl;
                    return crow::response(500, "Internal error");
                }
            }
        }
        return crow::response(202, "Successfully uploaded file");
    });

    CROW_ROUTE(app, "/profile").methods("GET"_method)([&app](const crow::request &req)
    {
        std::string email = req.get_header_value("Authorization");
        if (email.empty())
        {
            return crow::response(401, "Unauthorized");
        }
        try
        {
            auto conn = connectToDatabase();
            pqxx::nontransaction nonTransaction(*conn);
            pqxx::result docs = nonTransaction.exec("SELECT name FROM documents WHERE email = $1", pqxx::params{email});
            pqxx::result survey = nonTransaction.exec("SELECT * FROM surveys WHERE email = $1", pqxx::params{email});
            crow::json::wvalue response;
            response["email"] = email;

            std::vector<std::string> documents;
            for (auto const &row : docs)
            {
                documents.push_back(row["name"].c_str());
            }
            response["documents"] = documents;

            std::string prefsJson = getPreferences(email);
            if (!prefsJson.empty())
            {
                auto prefsData = crow::json::load(prefsJson);
                if (prefsData)
                {
                    for (const auto &key : prefsData.keys())
                    {
                        response[key] = prefsData[key];
                    }
                }
            }

            if (!survey.empty())
            {
                crow::json::wvalue surveyData;
                surveyData["docFrequency"] = survey[0]["howoften"].c_str();
                surveyData["docTypes"] = survey[0]["typesofdocuments"].c_str();
                surveyData["concerns"] = survey[0]["biggestconcerns"].c_str();
                surveyData["jargonComfort"] = survey[0]["jargonunderstanding"].c_str();
                surveyData["workedWithLawyer"] = survey[0]["workedwithlawyer"].c_str();
                response["survey"] = std::move(surveyData);
            }
            else
            {
                response["survey"] = nullptr;
            }
            return crow::response(response);
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << std::endl;
            return crow::response(500, "Failed to access profile information");
        }
    });

    CROW_ROUTE(app, "/profile").methods("POST"_method)([&app](const crow::request &req)
    {
        std::string email = req.get_header_value("Authorization");
        if (email.empty())
        {
            return crow::response(401, "Unauthorized");
        }
        auto data = crow::json::load(req.body);
        if (!data)
        {
            return crow::response(400, "Missing profile data");
        }
        if (savePreferences(email, req.body))
        {
            return crow::response(200, "Preferences saved");
        }
        return crow::response(500, "Failed to save preferences");
    });

    CROW_ROUTE(app, "/account/password").methods("POST"_method)([&app](const crow::request &req)
    {
        std::string email = req.get_header_value("Authorization");
        if (email.empty())
        {
            return crow::response(401, "Unauthorized");
        }
        auto data = crow::json::load(req.body);
        if (!data || !data.has("currentPassword") || !data.has("newPassword"))
        {
            return crow::response(400, "Missing password information");
        }
        std::string currentPassword = data["currentPassword"].s();
        std::string newPassword = data["newPassword"].s();
        if (!checkPassword(email, currentPassword))
        {
            return crow::response(400, "Current password is incorrect");
        }
        if (updatePassword(email, newPassword))
        {
            return crow::response(200, "Password updated");
        }
        return crow::response(500, "Failed to update password");
    });

    CROW_ROUTE(app, "/notebooks").methods("GET"_method)([&app](const crow::request &req)
    {
        std::string email = req.get_header_value("Authorization");
        if (email.empty())
        {
            return crow::response(401, "Unauthorized");
        }
        crow::json::wvalue body = getNotebooks(email);
        return crow::response(200, body);
    });

    CROW_ROUTE(app, "/chatHistory").methods("GET"_method)([&app](const crow::request &req)
    {
        std::string email = req.get_header_value("Authorization");
        if (email.empty())
        {
            return crow::response(401, "Unauthorized");
        }
        std::string chatid = req.get_header_value("ChatID");
        std::vector<ChatMessage> history = getChatHistory(email, chatid);
        std::vector<crow::json::wvalue> chatHistory;
        for (const auto &message : history)
        {
            crow::json::wvalue::list jsonMessage = {message.role, message.text};
            chatHistory.push_back(std::move(jsonMessage));
        }
        crow::json::wvalue result = std::move(chatHistory);
        return crow::response(200, result);
    });

    CROW_ROUTE(app, "/userPreferences").methods("GET"_method)([&app](const crow::request &req)
    {
        std::string email = req.get_header_value("Authorization");
        if (email.empty())
        {
            return crow::response(401, "Unauthorized");
        }
        pqxx::result result = getUserPreference(email);
        crow::json::wvalue userPreferences;
        if (!result.empty())
        {
            const auto &row = result[0];
            userPreferences["docFrequency"] = row["howoften"].c_str();
            userPreferences["docTypes"] = row["typesofdocuments"].c_str();
            userPreferences["concerns"] = row["biggestconcerns"].c_str();
            userPreferences["jargonComfort"] = row["jargonunderstanding"].c_str();
            userPreferences["workedWithLawyer"] = row["workedwithlawyer"].c_str();
        }
        return crow::response(200, userPreferences);
    });

    CROW_ROUTE(app, "/chat").methods("POST"_method)([&app](const crow::request &req)
    {
        std::string email = req.get_header_value("Authorization");
        if (email.empty())
        {
            return crow::response(401, "Unauthorized");
        }

        auto data = crow::json::load(req.body);
        if (!data || !data.has("message"))
        {
            return crow::response(400, "Missing chat message");
        }
        const std::string chatid = req.get_header_value("ChatID");
        if (chatid.empty())
        {
            return crow::response(400, "Missing chat id");
        }
        const std::string title = data["title"].s();
        const std::string message = data["message"].s();
        if (message.empty())
        {
            return crow::response(400, "Chat message cannot be empty");
        }
        if (GEMINI_API_KEY == "replace-with-gemini-api-key")
        {
            return crow::response(500, "Gemini API key placeholder has not been replaced");
        }
        std::vector<ChatMessage> history = getChatHistory(email, chatid);
        storeChatMessage(email, chatid, "user", message, title);

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

            storeChatMessage(email, chatid, "model", reply, title);

            std::vector<std::string> usedDocuments;
            usedDocuments.reserve(documents.size());
            for (const auto &document : documents)
            {
                usedDocuments.push_back(document.name);
            }

            crow::json::wvalue response;
            response["reply"] = reply;
            response["usedDocuments"] = usedDocuments;
            return crow::response(response);
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << std::endl;
            return crow::response(500, "Failed to process chat request");
        }
    });
}
