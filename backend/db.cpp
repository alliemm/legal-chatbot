#include "db.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sodium.h>
#include <crow.h>

static std::string normalizeChatRole(const std::string &role)
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

static std::string guessMimeType(const std::string &fileName)
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

static std::string base64Encode(const unsigned char *data, size_t size)
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

std::unique_ptr<pqxx::connection> connectToDatabase()
{
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
        std::cerr << e.what() << std::endl;
        return nullptr;
    }
}

bool checkPassword(const std::string &email, const std::string &password)
{
    try
    {
        auto conn = connectToDatabase();
        if (!conn)
        {
            return false;
        }
        pqxx::nontransaction nonTransaction(*conn);
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
                return false;
            }
            else
                return true;
        }
    }
    catch (std::exception &e)
    {
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
        pqxx::result survey = nonTransaction.exec("SELECT * FROM surveys WHERE email = $1", pqxx::params{email});
        return survey;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return pqxx::result();
    }
}

std::vector<ChatMessage> getChatHistory(const std::string &email, const std::string &chatid)
{
    try
    {
        auto conn = connectToDatabase();
        pqxx::nontransaction nonTransaction(*conn);
        pqxx::result messages = nonTransaction.exec("SELECT * FROM chats WHERE email = $1 AND chatid = $2 ORDER BY time ASC", pqxx::params{email, chatid});
        std::vector<ChatMessage> chatHistory;
        for (const auto &message : messages)
        {
            ChatMessage newMessage;
            newMessage.role = message["role"].as<std::string>();
            newMessage.text = message["content"].as<std::string>();
            chatHistory.push_back(newMessage);
        }
        return chatHistory;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return std::vector<ChatMessage>();
    }
}
std::vector<std::string> getSourceHistory(const std::string &email, const std::string &chatid)
{
    try
    {
        auto conn = connectToDatabase();
        pqxx::nontransaction nonTransaction(*conn);
        pqxx::result sources = nonTransaction.exec("SELECT * FROM documents WHERE email = $1 AND chatid = $2", pqxx::params{email, chatid});
        std::vector<std::string> sourceHistory;
        for (const auto &source : sources)
        {
            sourceHistory.push_back(source["name"].as<std::string>());
        }
        return sourceHistory;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return std::vector<std::string>();
    }
}
bool storeChatMessage(const std::string &email, const std::string &chatid, const std::string &role, const std::string &message, const std::string &title)
{
    try
    {
        auto conn = connectToDatabase();
        pqxx::work transaction(*conn);
        pqxx::result messages = transaction.exec("INSERT INTO chats (chatid, email, role, content, title) VALUES ($1, $2, $3, $4, $5)", pqxx::params{chatid, email, role, message, title});
        transaction.commit();
        return true;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

std::string getChatTitle(const std::string &email, const std::string &chatid)
{
    try
    {
        auto conn = connectToDatabase();
        pqxx::nontransaction nonTransaction(*conn);
        pqxx::result title = nonTransaction.exec("SELECT title FROM chats WHERE email = $1 AND chatid = $2", pqxx::params{email, chatid});
        return title[0][0].as<std::string>();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return "";
    }
}

crow::json::wvalue getNotebooks(const std::string &email)
{
    crow::json::wvalue body;
    try
    {
        auto conn = connectToDatabase();
        if (!conn)
        {
            return body;
        }
        pqxx::nontransaction nonTransaction(*conn);
        pqxx::result chatIDs = nonTransaction.exec("SELECT chatid FROM chats WHERE email = $1", pqxx::params{email});
        for (auto const &row : chatIDs)
        {
            std::string chatid = row[0].c_str();
            pqxx::result notebookInfo = nonTransaction.exec(
                "SELECT title, TO_CHAR(MIN(time), 'Mon DD, YYYY') FROM chats WHERE chatid = $1 GROUP BY title",
                pqxx::params{chatid});
            if (!notebookInfo.empty())
            {
                std::string titleString = notebookInfo[0][0].as<std::string>();
                std::string dateString = notebookInfo[0][1].as<std::string>();
                pqxx::result sources = nonTransaction.exec("SELECT COUNT(name) FROM documents WHERE chatid = $1", pqxx::params{chatid});
                int numSources = sources[0][0].as<int>();
                body[chatid] = crow::json::wvalue::list({titleString, dateString, numSources});
            }
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return body;
}

void initDatabase()
{
    try
    {
        auto conn = connectToDatabase();
        if (!conn)
        {
            std::cerr << "Failed to connect to database for initialization" << std::endl;
            return;
        }
        pqxx::work transaction(*conn);
        transaction.exec(
            "CREATE TABLE IF NOT EXISTS preferences ("
            "email TEXT PRIMARY KEY,"
            "preferences JSON"
            ")");
        transaction.exec(
            "CREATE TABLE IF NOT EXISTS surveys ("
            "email TEXT,"
            "doc_frequency TEXT,"
            "doc_types TEXT,"
            "concerns TEXT,"
            "jargon_comfort TEXT,"
            "worked_with_lawyer TEXT"
            ")");
        transaction.commit();
        std::cout << "Database tables initialized" << std::endl;
    }
    catch (std::exception &e)
    {
        std::cerr << "Failed to initialize database: " << e.what() << std::endl;
    }
}

bool savePreferences(const std::string &email, const std::string &preferencesJson)
{
    try
    {
        auto conn = connectToDatabase();
        if (!conn)
        {
            return false;
        }
        pqxx::work transaction(*conn);
        pqxx::result res = transaction.exec(
            "SELECT * FROM preferences WHERE email = $1", pqxx::params{email});
        if (res.empty())
        {
            transaction.exec(
                "INSERT INTO preferences VALUES ($1, $2)",
                pqxx::params{email, preferencesJson});
        }
        else
        {
            transaction.exec(
                "UPDATE preferences SET preferences = $1 WHERE email = $2",
                pqxx::params{preferencesJson, email});
        }
        transaction.commit();
        return true;
    }
    catch (std::exception &e)
    {
        std::cerr << "Failed to save preferences: " << e.what() << std::endl;
        return false;
    }
}

std::string getPreferences(const std::string &email)
{
    try
    {
        auto conn = connectToDatabase();
        if (!conn)
        {
            return "";
        }
        pqxx::nontransaction nonTransaction(*conn);
        pqxx::result res = nonTransaction.exec(
            "SELECT preferences FROM preferences WHERE email = $1", pqxx::params{email});
        if (!res.empty())
        {
            return res[0]["preferences"].c_str();
        }
        return "";
    }
    catch (std::exception &e)
    {
        std::cerr << "Failed to get preferences: " << e.what() << std::endl;
        return "";
    }
}

bool deletePreferences(const std::string &email)
{
    try
    {
        auto conn = connectToDatabase();
        if (!conn)
        {
            return false;
        }
        pqxx::work transaction(*conn);
        transaction.exec(
            "DELETE FROM preferences WHERE email = $1", pqxx::params{email});
        transaction.commit();
        return true;
    }
    catch (std::exception &e)
    {
        std::cerr << "Failed to delete preferences: " << e.what() << std::endl;
        return false;
    }
}

bool saveSurvey(
    const std::string &email,
    const std::string &docFrequency,
    const std::string &docTypes,
    const std::string &concerns,
    const std::string &jargonComfort,
    const std::string &workedWithLawyer)
{
    try
    {
        auto conn = connectToDatabase();
        if (!conn)
        {
            return false;
        }
        pqxx::work transaction(*conn);
        transaction.exec(
            "INSERT INTO surveys VALUES ($1, $2, $3, $4, $5, $6)",
            pqxx::params{email, docFrequency, docTypes, concerns, jargonComfort, workedWithLawyer});
        transaction.commit();
        return true;
    }
    catch (std::exception &e)
    {
        std::cerr << "Failed to save survey: " << e.what() << std::endl;
        return false;
    }
}

bool updatePassword(const std::string &email, const std::string &newPassword)
{
    try
    {
        auto conn = connectToDatabase();
        if (!conn)
        {
            return false;
        }
        char hashed[crypto_pwhash_STRBYTES];
        int hashRes = crypto_pwhash_str(
            hashed, newPassword.c_str(), newPassword.length(),
            crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE);
        if (hashRes != 0)
        {
            std::cerr << "Failed to hash password" << std::endl;
            return false;
        }
        std::string finalHash = std::string(hashed);
        pqxx::work transaction(*conn);
        transaction.exec(
            "UPDATE users SET password = $1 WHERE email = $2",
            pqxx::params{finalHash, email});
        transaction.commit();
        return true;
    }
    catch (std::exception &e)
    {
        std::cerr << "Failed to update password: " << e.what() << std::endl;
        return false;
    }
}

static std::string getJsonString(const crow::json::rvalue &obj, const std::string &key)
{
    if (!obj.has(key)) return "";
    if (obj[key].t() == crow::json::type::String)
        return obj[key].s();
    return "";
}

static std::string getJsonBool(const crow::json::rvalue &obj, const std::string &key)
{
    if (!obj.has(key)) return "";
    if (obj[key].t() == crow::json::type::True) return "Yes";
    if (obj[key].t() == crow::json::type::False) return "No";
    return "";
}

static std::string getJsonArrayString(const crow::json::rvalue &obj, const std::string &key)
{
    if (!obj.has(key) || obj[key].t() != crow::json::type::List) return "";
    std::string result;
    const auto &arr = obj[key];
    for (size_t i = 0; i < arr.size(); ++i)
    {
        if (i > 0) result += ", ";
        result += arr[i].s();
    }
    return result;
}

std::string gatherUserContext(const std::string &email)
{
    std::string context;

    std::string prefsJson = getPreferences(email);
    if (!prefsJson.empty())
    {
        auto prefs = crow::json::load(prefsJson);
        if (prefs)
        {
            context += "USER PREFERENCES:\n";

            std::string nickname = getJsonString(prefs, "nickname");
            if (!nickname.empty())
                context += "- Name: " + nickname + "\n";

            std::string userType = getJsonString(prefs, "userType");
            if (!userType.empty())
                context += "- User type: " + userType + "\n";

            std::string legalExp = getJsonString(prefs, "legalExperience");
            if (!legalExp.empty())
                context += "- Legal experience: " + legalExp + "\n";

            std::string primaryGoal = getJsonString(prefs, "primaryGoal");
            if (!primaryGoal.empty())
                context += "- Primary goal: " + primaryGoal + "\n";

            std::string focusAreas = getJsonArrayString(prefs, "focusAreas");
            if (!focusAreas.empty())
                context += "- Focus areas: " + focusAreas + "\n";

            std::string explStyle = getJsonString(prefs, "explanationStyle");
            if (!explStyle.empty())
                context += "- Explanation style: " + explStyle + "\n";

            std::string detailLevel = getJsonString(prefs, "detailLevel");
            if (!detailLevel.empty())
                context += "- Detail level: " + detailLevel + "\n";

            std::string riskSens = getJsonString(prefs, "riskSensitivity");
            if (!riskSens.empty())
                context += "- Risk sensitivity: " + riskSens + "\n";

            std::string lang = getJsonString(prefs, "language");
            if (!lang.empty() && lang != "Other")
                context += "- Language: " + lang + "\n";

            if (prefs.has("behaviourFlags"))
            {
                const auto &flags = prefs["behaviourFlags"];
                context += "- Show risk rating: " + getJsonBool(flags, "showRiskRating") + "\n";
                context += "- Recommend lawyer: " + getJsonBool(flags, "recommendLawyer") + "\n";
                context += "- Show deadlines: " + getJsonBool(flags, "showDeadlines") + "\n";
                context += "- Show TL;DR: " + getJsonBool(flags, "showTldr") + "\n";
                context += "- Ask clarifying questions: " + getJsonBool(flags, "askClarifying") + "\n";
            }
        }
    }

    try
    {
        auto conn = connectToDatabase();
        if (conn)
        {
            pqxx::nontransaction nonTransaction(*conn);
            pqxx::result survey =
                nonTransaction.exec("SELECT * FROM surveys WHERE email = $1", pqxx::params{email});
            if (!survey.empty())
            {
                context += "\nUSER SURVEY:\n";

                std::string docFreq = survey[0]["doc_frequency"].c_str();
                if (!docFreq.empty())
                    context += "- Document frequency: " + docFreq + "\n";

                std::string docTypes = survey[0]["doc_types"].c_str();
                if (!docTypes.empty())
                    context += "- Documents: " + docTypes + "\n";

                std::string concerns = survey[0]["concerns"].c_str();
                if (!concerns.empty())
                    context += "- Concerns: " + concerns + "\n";

                std::string jargon = survey[0]["jargon_comfort"].c_str();
                if (!jargon.empty())
                    context += "- Jargon comfort: " + jargon + "\n";

                std::string lawyer = survey[0]["worked_with_lawyer"].c_str();
                if (!lawyer.empty())
                    context += "- Worked with lawyer: " + lawyer + "\n";
            }
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Failed to gather survey context: " << e.what() << std::endl;
    }

    return context;
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
                                 base64Encode(reinterpret_cast<const unsigned char *>(binaryData.data()), binaryData.size())});
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
                             base64Encode(reinterpret_cast<const unsigned char *>(binaryData.data()), binaryData.size())});
    }

    return documents;
}
