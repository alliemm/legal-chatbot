#pragma once

#include "config.hpp"
#include <crow.h>
#include <memory>
#include <string>
#include <vector>
#include <pqxx/pqxx>

std::unique_ptr<pqxx::connection> connectToDatabase();
void initDatabase();
bool checkPassword(const std::string &email, const std::string &password);
pqxx::result getUserPreference(const std::string &email);
std::vector<ChatMessage> getChatHistory(const std::string &email, const std::string &chatid);
std::vector<std::string> getSourceHistory(const std::string &email, const std::string &chatid);
bool storeChatMessage(const std::string &email, const std::string &chatid, const std::string &role, const std::string &message, const std::string &title = "");
std::string getChatTitle(const std::string &email, const std::string &chatid);
crow::json::wvalue getNotebooks(const std::string &email);

std::vector<StoredDocument> loadDocuments(
    const std::string &email,
    const std::vector<std::string> &requestedNames,
    std::vector<std::string> &missingDocuments);

bool savePreferences(const std::string &email, const std::string &preferencesJson);
std::string getPreferences(const std::string &email);
bool deletePreferences(const std::string &email);

bool saveSurvey(
    const std::string &email,
    const std::string &docFrequency,
    const std::string &docTypes,
    const std::string &concerns,
    const std::string &jargonComfort,
    const std::string &workedWithLawyer);

bool updatePassword(const std::string &email, const std::string &newPassword);
std::string gatherUserContext(const std::string &email);
