#include "http_parser.h"
#include <sstream>
#include <algorithm>

bool HTTPParser::isHTTP(const unsigned char* data, size_t size) {
    if (size < 10) return false;

    std::string start(reinterpret_cast<const char*>(data), std::min(size_t(20), size));

    // Проверка запроса HTTP
    if (start.find("GET ") == 0 || start.find("POST ") == 0 ||
        start.find("PUT ") == 0 || start.find("DELETE ") == 0 ||
        start.find("HEAD ") == 0 || start.find("OPTIONS ") == 0) {
        return true;
    }

    // Проверка ответа HTTP
    if (start.find("HTTP/1.") == 0 || start.find("HTTP/2") == 0) {
        return true;
    }

    return false;
}

HTTPParser::HTTPMethod HTTPParser::stringToMethod(const std::string& method) {
    if (method == "GET") return GET;
    if (method == "POST") return POST;
    if (method == "PUT") return PUT;
    if (method == "DELETE") return DELETE;
    if (method == "HEAD") return HEAD;
    if (method == "OPTIONS") return OPTIONS;
    return UNKNOWN;
}

HTTPParser::HTTPMessage HTTPParser::parseHTTP(const unsigned char* data, size_t size) {
    HTTPMessage message;
    std::string httpData(reinterpret_cast<const char*>(data), size);
    std::istringstream stream(httpData);
    std::string line;

    // Разбор первой строки
    if (std::getline(stream, line)) {
        if (line.size() > 0 && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }

        std::istringstream firstLineStream(line);
        std::string firstToken;
        firstLineStream >> firstToken;

        if (firstToken.find("HTTP/") == 0) {
            // Это ответ HTTP
            message.isRequest = false;
            message.version = firstToken;
            firstLineStream >> message.statusCode;
            std::getline(firstLineStream, message.statusText);
            if (!message.statusText.empty() && message.statusText[0] == ' ') {
                message.statusText = message.statusText.substr(1);
            }
        } else {
            // Это запрос HTTP
            message.isRequest = true;
            message.method = stringToMethod(firstToken);
            firstLineStream >> message.uri;
            firstLineStream >> message.version;
        }
    }

    // Разбор заголовков
    while (std::getline(stream, line)) {
        if (line.size() > 0 && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }

        if (line.empty()) {
            break;  // Пустая строка означает конец заголовков
        }

        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);

            // Удаление начальных пробелов из значения
            while (!value.empty() && value[0] == ' ') {
                value.erase(0, 1);
            }

            message.headers[key] = value;
        }
    }

    // Разбор тела сообщения
    std::string body;
    while (std::getline(stream, line)) {
        body += line + "\n";
    }
    message.body = body;

    return message;
}
