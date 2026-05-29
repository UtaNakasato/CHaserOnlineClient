#include "CHaser.hpp"

#include <iostream>

int main() {
	try {
		Client client({ "www7019ug.sakura.ne.jp", "80" }, 10);
		client.UserCheck("cool24", "cool");
		client.RoomNumberCheck("1");

		std::cout << "success." << std::endl;

		while (1) {
			std::cout << client.GetReadyCheck("gr") << std::endl;
			std::cout << client.FullCommandCheck("wu") << std::endl;
			client.GetReadyCheck("gr");
			client.FullCommandCheck("wl");
			client.GetReadyCheck("gr");
			client.FullCommandCheck("wd");
			client.GetReadyCheck("gr");
			client.FullCommandCheck("wr");
		}
	}
	catch (const GameOverException& ex) {
		std::cout << "gameover" << std::endl;
	}
	catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}

	return 0;
}
