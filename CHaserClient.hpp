#pragma once

#include "CHaserHttp.hpp"
#include <vector>
#include <string>
#include <string_view>
#include <iostream>

class Client {
    HttpRequest m_httpRequest;
    std::string m_sessionID = "";
    int m_timeoutSec = 120;
    bool m_noTimeoutForGetReady = true;
    std::vector<std::string> m_roomNumbers;

    void GetSessionIDFromText(const std::string_view text);
    void GetAvaliableRoomNumbersFromText(const std::string_view text);
    std::string GetReturnCodeFromText(const std::string_view text);

public:
    /// <summary>
    /// CHaserサーバーに接続するためのクライアント
    /// </summary>
    /// <param name="dest">CHaserサーバーのIPアドレスとポートの構造体</param>
    /// <param name="timeoutSec">サーバーからの応答を待機する時間の長さ（秒）</param>
    /// <param name="noTimeoutForGetReady">
    /// GetReadyにタイムアウトを適用するかどうかのフラグ
    /// 二人以上の対戦でほかのユーザーを待機する場合は、trueにすることを推奨
    /// </param>
    Client(const Endpoint& dest, int timeoutSec = 120, bool noTimeoutForGetReady = true);

    /// <summary>
    /// プロキシを介してCHaserサーバーに接続するためのクライアント
    /// </summary>
    /// <param name="dest">CHaserサーバーのIPアドレスとポートの構造体</param>
    /// <param name="proxy">プロキシサーバーのIPアドレスとポートの構造体</param>
    /// <param name="timeoutSec">サーバーからの応答を待機する時間の長さ（秒）</param>
    /// <param name="noTimeoutForGetReady">
    /// GetReadyにタイムアウトを適用するかどうかのフラグ
    /// 二人以上の対戦でほかのユーザーを待機する場合は、trueにすることを推奨
    /// </param>
    Client(const Endpoint& dest, const Endpoint& proxy, int timeoutSec = 120, bool noTimeoutForGetReady = true);

    std::string Command(const std::string_view command, const std::string_view params);

    /// <summary>
    /// ユーザー認証とセッションIDの取得用メソッド
    /// </summary>
    /// <param name="user">ユーザー名（例）cool15</param>
    /// <param name="pass">パスワード（例）cool</param>
    /// <returns>利用可能なルーム番号のリスト</returns>
    const std::vector<std::string>& UserCheck(const std::string_view user, const std::string_view pass);

    /// <summary>
    /// ルーム接続用メソッド
    /// </summary>
    /// <param name="roomNumber">ルーム番号</param>
    void RoomNumberCheck(const std::string_view roomNumber);

    /// <summary>
    /// 準備（GetReady）コマンドを送信するメソッド
    /// </summary>
    /// <param name="getReady">grやgruなど</param>
    /// <returns>周囲情報</returns>
    std::string GetReadyCheck(const std::string_view getReady);

    /// <summary>
    /// バグ防止のため、代わりにFullCommandCheckを使うことを推奨
    /// </summary>
    /// <param name="command"></param>
    /// <returns></returns>
    std::string CommandCheck(const std::string_view command);
    void EndCommandCheck();

    /// <summary>
    /// 行動（Action）コマンドとターン終了コマンドをまとめて送信するメソッド
    /// </summary>
    /// <param name="command">wuやwdなど</param>
    /// <returns>周囲情報</returns>
    std::string FullCommandCheck(const std::string_view command);
};