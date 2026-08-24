#include "CHaser.hpp"

int main() {
	//さくらサーバーに接続する
	//httpなのでポート80
	//10秒間サーバーから応答がなかったらエラーで終了する
	Client client({ "www7019ug.sakura.ne.jp", "80" }, 10);
	int roomIndex = 0;

	while (1) {
		try {
			//cool24:coolでアカウント認証
			//利用可能なルーム番号のベクターを保存
			auto rooms = client.UserCheck("cool24", "cool");
			std::cout << "ユーザー認証に成功しました" << std::endl;

			//利用可能なルームをすべて探索したらプログラムを終了する
			if (rooms.size() <= roomIndex) {
				std::cout << "すべてのルームを探索しました" << std::endl;
				break;
			}
			
			//ルーム接続
			client.RoomNumberCheck(rooms[roomIndex++]);
			std::cout << "ルーム接続に成功しました" << std::endl;

			//ぐるぐる回るだけ
			while (1) {
				//GetReadyで周囲情報を取得
				std::cout << client.GetReadyCheck("gr") << std::endl;
				//WalkUpで移動
				std::cout << client.FullCommandCheck("wu") << std::endl;
				//GetReadyとActionを交互に実行
				std::cout << client.GetReadyCheck("gr") << std::endl;
				std::cout << client.FullCommandCheck("wl") << std::endl;
				std::cout << client.GetReadyCheck("gr") << std::endl;
				std::cout << client.FullCommandCheck("wd") << std::endl;
				std::cout << client.GetReadyCheck("gr") << std::endl;
				std::cout << client.FullCommandCheck("wr") << std::endl;
			}
		}
		//ゲームが終わるとここに処理が行く
		catch (const GameOverException&) {
			std::cout << "ゲーム終了" << std::endl;
		}
		//ユーザー認証やルーム接続に失敗するとここに処理が行く
		catch (const std::exception& ex) {
			//ex.what()で失敗した理由を取得できる
			std::cerr << ex.what() << std::endl;
		}
	}

	return 0;
}