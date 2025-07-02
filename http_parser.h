#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <string>
#include <map>
#include <vector>

// Перечисление HTTP методов - выносим за пределы класса
enum HTTPMethod {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_HEAD,
    HTTP_OPTIONS,
    HTTP_UNKNOWN
};

// Структура для хранения HTTP сообщения - выносим за пределы класса
struct HTTPMessage {
    bool isRequest;
    HTTPMethod method;
    std::string uri;
    std::string version;
    int statusCode;
    std::string statusText;
    std::map<std::string, std::string> headers;
    std::string body;
};

// Класс для разбора HTTP сообщений
class HTTPParser {
public:
    // Проверяет, является ли данный пакет HTTP сообщением
    static bool isHTTP(const unsigned char* data, size_t size);

    // Разбирает HTTP сообщение
    static HTTPMessage parseHTTP(const unsigned char* data, size_t size);

private:
    // Преобразует строку в HTTP метод
    static HTTPMethod stringToMethod(const std::string& method);
};

#endif // HTTP_PARSER_H
