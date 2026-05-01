#pragma once

#include "config.hpp"
#include <string>
#include <vector>

std::string buildGeminiRequestBody(
    const std::string &message,
    const std::vector<ChatMessage> &history,
    const std::vector<StoredDocument> &documents);

GeminiHttpResponse sendGeminiRequest(const std::string &requestBody);
std::string extractGeminiReply(const std::string &responseBody);
std::string extractGeminiError(const std::string &responseBody);
