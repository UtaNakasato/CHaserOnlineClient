#pragma once

#include "CHaserHttp.hpp"
#include <vector>
#include <string>
#include <string_view>

class Client {
    HttpRequest m_httpRequest;
    std::string m_sessionID = "";
    int m_timeoutSec = 120;
    std::vector<std::string> m_roomNumbers;

    void GetSessionIDFromText(const std::string_view text);
    void GetAvaliableRoomNumbersFromText(const std::string_view text);
    std::string GetReturnCodeFromText(const std::string_view text);

public:
    Client(const Endpoint& dest, int timeoutSec = 120);
    Client(const Endpoint& dest, const Endpoint& proxy, int timeoutSec = 120);

    std::string Command(const std::string_view command, const std::string_view params);
    const std::vector<std::string>& UserCheck(const std::string_view user, const std::string_view pass);
    void RoomNumberCheck(const std::string_view roomNumber);
    std::string GetReadyCheck(const std::string_view getReady);
    std::string CommandCheck(const std::string_view command);
    void EndCommandCheck();
    std::string FullCommandCheck(const std::string_view command);
};