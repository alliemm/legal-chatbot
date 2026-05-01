#include "gemini.hpp"

#include <curl/curl.h>
#include <crow.h>
#include <iostream>

static size_t writeCurlResponse(void *contents, size_t size, size_t nmemb, void *userp)
{
    const size_t totalSize = size * nmemb;
    auto *buffer = static_cast<std::string *>(userp);
    buffer->append(static_cast<char *>(contents), totalSize);
    return totalSize;
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
