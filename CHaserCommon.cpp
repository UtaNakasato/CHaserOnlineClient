#include "CHaserCommon.hpp"
#include <format>

#pragma comment(lib, "Ws2_32.lib")

CommandException::CommandException(const std::string_view command, const std::string_view msg)
    : m_msg(std::format("{}コマンド: {}", command, msg)) {
}

const char* CommandException::what() const noexcept { return m_msg.c_str(); }

UserCheckException::UserCheckException(const std::string_view msg) : CommandException("UserCheck", msg) {}
RoomNumberCheckException::RoomNumberCheckException(const std::string_view msg) : CommandException("RoomNumberCheck", msg) {}
GetReadyException::GetReadyException(const std::string_view msg) : CommandException("GetReadyCheck", msg) {}
CommandCheckException::CommandCheckException(const std::string_view msg) : CommandException("CommandCheck", msg) {}
EndCommandCheckException::EndCommandCheckException(const std::string_view msg) : CommandException("EndCommandCheck", msg) {}

ClientException::ClientException(const std::string_view msg) : m_msg(msg) {}
const char* ClientException::what() const noexcept { return m_msg.c_str(); }

GameOverException::GameOverException(const std::string_view msg) : m_msg(msg) {}
const char* GameOverException::what() const noexcept { return m_msg.c_str(); }

ParsingException::ParsingException(const std::string_view msg) : m_msg(msg) {}
const char* ParsingException::what() const noexcept { return m_msg.c_str(); }

HttpRequestException::HttpRequestException(const std::string_view msg, const std::string_view statusCode)
    : m_msg(std::format("{} ステータスコード: {}", msg, statusCode)) {
}
const char* HttpRequestException::what() const noexcept { return m_msg.c_str(); }

WSAException::WSAException(const std::string_view msg, int err) {
    LPVOID lpMsgBuf{};
    if (FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&lpMsgBuf, 0, nullptr
    )) {
        m_msg = std::format("{}: {}", msg, (LPSTR)lpMsgBuf);
    }
    else {
        m_msg = std::format("{}: {}", msg, err);
    }
    LocalFree(lpMsgBuf);
}
const char* WSAException::what() const noexcept { return m_msg.c_str(); }

WSA::WSA() {
    int ret = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
    if (ret) {
        throw WSAException("WSAStartup エラー", ret);
    }
}

WSA::~WSA() {
    WSACleanup();
}

const WSA& WSA::Init() {
    static WSA wsa;
    return wsa;
}