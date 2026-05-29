#pragma once

#include "CHaserCommon.hpp"
#include <string>
#include <string_view>

class HttpRequest {
    static const int m_recvLen = 4096;
    struct addrinfo* m_result = nullptr;
    struct addrinfo* m_target = nullptr;
    struct addrinfo m_hints {};
    Endpoint m_dest;

    void InitAddrinfo(const Endpoint& ep);
    void EndpointEmptyCheck(const Endpoint& ep);
    SOCKET ConnectToTarget();
    std::string GetStatusCodeFromText(const std::string_view text) const;

public:
    struct Response {
        std::string statusCode;
        std::string content;
    };

    HttpRequest(const Endpoint& dest);
    HttpRequest(const Endpoint& dest, const Endpoint& proxy);
    ~HttpRequest();

    Response Get(const std::string_view headers = "", const std::string_view path = "", const std::string_view params = "");
};