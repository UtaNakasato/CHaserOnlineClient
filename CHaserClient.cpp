#include "CHaserClient.hpp"
#include <chrono>
#include <format>

void Client::GetSessionIDFromText(const std::string_view text) {
    size_t sessionStart = text.find("JSESSIONID=");
    if (sessionStart == std::string::npos) {
        throw ParsingException("セッションの始端文字列が見つかりません");
    }

    sessionStart += 11;
    size_t sessionEnd = text.find(";", sessionStart);
    if (sessionEnd == std::string::npos) {
        throw ParsingException("セッションの終端文字が見つかりません");
    }

    m_sessionID = text.substr(sessionStart, sessionEnd - sessionStart);
}

void Client::GetAvaliableRoomNumbersFromText(const std::string_view text) {
    size_t roomStart = text.find("利用可能ルーム番号=");
    if (roomStart == std::string::npos) {
        throw ParsingException("利用可能ルーム番号の始端文字列が見つかりません");
    }

    roomStart += 28;
    size_t roomEnd = text.find("<form", roomStart);
    if (roomEnd == std::string::npos) {
        throw ParsingException("利用可能ルーム番号の終端文字列が見つかりません");
    }

    std::string strRooms = std::string(text.substr(roomStart, roomEnd - roomStart));

    size_t offset = 0;
    while (true) {
        size_t next = strRooms.find(",\n\t\t\t\n\t\t\t\t", offset);
        if (next == std::string::npos) {
            break;
        }

        m_roomNumbers.push_back(strRooms.substr(offset, next - offset));
        offset = next + 10;
    }
}

std::string Client::GetReturnCodeFromText(const std::string_view text) {
    size_t retStart = text.find("ReturnCode=");
    if (retStart == std::string::npos) {
        throw ParsingException("周囲情報の始端文字列が見つかりません");
    }

    retStart += 11;
    size_t retEnd = text.find("\n", retStart);
    if (retEnd == std::string::npos) {
        throw ParsingException("周囲情報の終端文字が見つかりません");
    }

    return std::string(text.substr(retStart, retEnd - retStart));
}

Client::Client(const Endpoint& dest, int timeoutSec, bool noTimeoutForGetReady)
    : m_httpRequest(dest), m_timeoutSec(timeoutSec), m_noTimeoutForGetReady(noTimeoutForGetReady) {
}

Client::Client(const Endpoint& dest, const Endpoint& proxy, int timeoutSec, bool noTimeoutForGetReady)
    : m_httpRequest(dest, proxy), m_timeoutSec(timeoutSec), m_noTimeoutForGetReady(noTimeoutForGetReady) {
}

std::string Client::Command(const std::string_view command, const std::string_view params) {
    std::string sessionParam = "";
    if (!m_sessionID.empty()) {
        sessionParam = std::format(";jsessionid={}", m_sessionID);
    }

    HttpRequest::Response res = m_httpRequest.Get(
        std::format("Cookie: jsession={}\r\nUser-Agent: CHaserOnlineClient/2026\r\nConnection: close\r\n\r\n", m_sessionID),
        std::format("/CHaserOnline003/user/{}{}", command, sessionParam), params
    );

    if (res.statusCode == "500") {
        throw GameOverException("ゲームが終了しました");
    }
    else if (res.statusCode != "200") {
        throw HttpRequestException(command, res.statusCode);
    }

    return res.content;
}

const std::vector<std::string>& Client::UserCheck(const std::string_view user, const std::string_view pass) {
    std::string content;
    auto start = std::chrono::system_clock::now();
    while (true) {
        content = Command("UserCheck", std::format("?user={}&pass={}", user, pass));

        try {
            if (m_sessionID.empty()) {
                GetSessionIDFromText(content);
            }
        }
        catch (const ParsingException&) {
            std::cerr << "セッションIDの取得に失敗しました。" << std::endl;
        }

        try {
            if (content.find("roomNumber=") != std::string::npos) {
                GetAvaliableRoomNumbersFromText(content);
                break;
            }
        }
        catch (const ParsingException&) {
            std::cerr << "利用可能なルーム番号の取得に失敗しました。" << std::endl;
        }

        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() >= m_timeoutSec) {
            throw ClientException("ログインに失敗しました");
        }
    }

    return m_roomNumbers;
}

void Client::RoomNumberCheck(const std::string_view roomNumber) {
    std::string content;
    auto start = std::chrono::system_clock::now();
    while (true) {
        content = Command("RoomNumberCheck", std::format("?roomNumber={}", roomNumber));
        if (content.find("command1=") != std::string::npos) {
            break;
        }

        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() >= m_timeoutSec) {
            throw ClientException("ルーム接続に失敗しました");
        }
    }
}

std::string Client::GetReadyCheck(const std::string_view getReady) {
    std::string ret;
    auto start = std::chrono::system_clock::now();
    while (true) {
        std::string content = Command("GetReadyCheck", std::format("?command1={}", getReady));

        if (content.find("user=") != std::string::npos) {
            throw GameOverException("ゲームが終了しました");
        }

        try {
            ret = GetReturnCodeFromText(content);
            if (ret.find(",") != std::string::npos) {
                break;
            }
        }
        catch (const ParsingException&) {
            std::cerr << "ほかのユーザーの接続を待機しているか、周囲情報の取得に失敗しました。" << std::endl;
            if (m_noTimeoutForGetReady) {
                continue;
            }
        }

        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() >= m_timeoutSec) {
            throw ClientException("GetReadyコマンドの送信に失敗しました");
        }
    }
    return ret;
}

std::string Client::CommandCheck(const std::string_view command) {
    std::string ret;
    auto start = std::chrono::system_clock::now();
    while (true) {
        std::string content = Command("CommandCheck", std::format("?command2={}", command));

        if (content.find("user=") != std::string::npos) {
            throw GameOverException("ゲームが終了しました");
        }

        try {
            ret = GetReturnCodeFromText(content);
            if (ret.find(",") != std::string::npos) {
                break;
            }
        }
        catch (const ParsingException&) {
            std::cerr << "周囲情報の取得に失敗しました。" << std::endl;
        }

        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() >= m_timeoutSec) {
            throw ClientException("Actionコマンドの送信に失敗しました");
        }
    }
    return ret;
}

void Client::EndCommandCheck() {
    std::string content;
    auto start = std::chrono::system_clock::now();
    while (true) {
        content = Command("EndCommandCheck", "?command3=%23");
        if (content.find("command1=") != std::string::npos) {
            break;
        }

        if (content.find("user=") != std::string::npos ||
            std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() >= m_timeoutSec) {
            throw GameOverException("ゲームが終了しました");
        }
    }
}

std::string Client::FullCommandCheck(const std::string_view command) {
    std::string ret = CommandCheck(command);
    EndCommandCheck();
    return ret;
}