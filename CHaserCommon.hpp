#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <string_view>
#include <exception>

class CommandException : public std::exception {
    std::string m_msg;
public:
    CommandException(const std::string_view command, const std::string_view msg);
    const char* what() const noexcept override;
};

class UserCheckException : public CommandException {
public:
    UserCheckException(const std::string_view msg);
};

class RoomNumberCheckException : public CommandException {
public:
    RoomNumberCheckException(const std::string_view msg);
};

class GetReadyException : public CommandException {
public:
    GetReadyException(const std::string_view msg);
};

class CommandCheckException : public CommandException {
public:
    CommandCheckException(const std::string_view msg);
};

class EndCommandCheckException : public CommandException {
public:
    EndCommandCheckException(const std::string_view msg);
};

class ClientException : public std::exception {
    std::string m_msg;
public:
    ClientException(const std::string_view msg);
    const char* what() const noexcept override;
};

class GameOverException : public std::exception {
    std::string m_msg;
public:
    GameOverException(const std::string_view msg);
    const char* what() const noexcept override;
};

class ParsingException : public std::exception {
    std::string m_msg;
public:
    ParsingException(const std::string_view msg);
    const char* what() const noexcept override;
};

class HttpRequestException : public std::exception {
    std::string m_msg;
public:
    HttpRequestException(const std::string_view msg, const std::string_view statusCode);
    const char* what() const noexcept override;
};

class WSAException : public std::exception {
    std::string m_msg;
public:
    WSAException(const std::string_view msg, int err = WSAGetLastError());
    const char* what() const noexcept override;
};

class WSA {
    WSADATA m_wsaData{};
    WSA();
    ~WSA();
public:
    static const WSA& Init();
};

struct Endpoint {
    std::string_view host;
    std::string_view port;
};