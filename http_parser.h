#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <string>
#include <map>

class HTTPParser {
public:
    enum HTTPMethod {
        GET,
        POST,
        PUT,
        DELETE,
        HEAD,
        OPTIONS,
        UNKNOWN
    };

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

    static bool isHTTP(const unsigned char* data, size_t size);
    static HTTPMessage parseHTTP(const unsigned char* data, size_t size);

private:
    static HTTPMethod stringToMethod(const std::string& method);
};

#endif // HTTP_PARSER_H
