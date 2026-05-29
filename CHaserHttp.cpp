#include "CHaserHttp.hpp"
#include <stdexcept>
#include <format>

void HttpRequest::InitAddrinfo(const Endpoint& ep) {
    m_hints.ai_family = AF_INET;
    m_hints.ai_socktype = SOCK_STREAM;
    m_hints.ai_protocol = IPPROTO_TCP;

    int ret = getaddrinfo(ep.host.data(), ep.port.data(), &m_hints, &m_result);
    if (ret) {
        throw WSAException("getaddrinfo エラー", ret);
    }
}

void HttpRequest::EndpointEmptyCheck(const Endpoint& ep) {
    if (ep.host.empty()) throw std::invalid_argument("ホストが空です");
    if (ep.port.empty()) throw std::invalid_argument("ポートが空です");
}

SOCKET HttpRequest::ConnectToTarget() {
    SOCKET sock = INVALID_SOCKET;

    for (m_target = m_result; m_target != nullptr; m_target = m_target->ai_next) {
        sock = socket(m_target->ai_family, m_target->ai_socktype, m_target->ai_protocol);
        if (sock == INVALID_SOCKET) {
            continue;
        }

        int res = connect(sock, m_target->ai_addr, static_cast<int>(m_target->ai_addrlen));
        if (res == SOCKET_ERROR) {
            closesocket(sock);
            sock = INVALID_SOCKET;
            continue;
        }
        break;
    }

    if (sock == INVALID_SOCKET) {
        throw WSAException("socket / connect エラー");
    }
    return sock;
}

std::string HttpRequest::GetStatusCodeFromText(const std::string_view text) const {
    size_t codeStart = text.find("HTTP/1.1 ");
    if (codeStart == std::string::npos) {
        throw ParsingException("ステータスコードの始端文字列が見つかりません");
    }

    codeStart += 9;
    size_t codeEnd = text.find(' ', codeStart);
    if (codeEnd == std::string::npos) {
        throw ParsingException("ステータスコードの終端文字が見つかりません");
    }

    return std::string(text.substr(codeStart, codeEnd - codeStart));
}

HttpRequest::HttpRequest(const Endpoint& dest) : m_dest(dest) {
    WSA::Init();
    EndpointEmptyCheck(dest);
    InitAddrinfo(dest);
}

HttpRequest::HttpRequest(const Endpoint& dest, const Endpoint& proxy) : m_dest(dest) {
    WSA::Init();
    EndpointEmptyCheck(dest);
    EndpointEmptyCheck(proxy);
    InitAddrinfo(proxy);
}

HttpRequest::~HttpRequest() {
    if (m_result) {
        freeaddrinfo(m_result);
        m_result = nullptr;
    }
}

HttpRequest::Response HttpRequest::Get(const std::string_view headers, const std::string_view path, const std::string_view params) {
    SOCKET sock = ConnectToTarget();
    int res;

    std::string sendBuf = std::format("GET http://{}:{}{}{} HTTP/1.1\r\nHost: {}:{}\r\n{}", m_dest.host, m_dest.port, path, params, m_dest.host, m_dest.port, headers);

    res = send(sock, sendBuf.data(), static_cast<int>(sendBuf.length()), 0);
    if (res == SOCKET_ERROR) {
        closesocket(sock);
        throw WSAException("send エラー");
    }

    char tmpBuf[m_recvLen];
    std::string recvBuf;

    do {
        res = recv(sock, tmpBuf, m_recvLen, 0);
        if (res > 0) {
            recvBuf.append(tmpBuf, res);
        }
        else if (res < 0) {
            closesocket(sock);
            throw WSAException("recv エラー");
        }
    } while (res > 0);

    closesocket(sock);
    return { GetStatusCodeFromText(recvBuf), recvBuf };
}