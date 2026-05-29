#include "CHaser.hpp"

#undef main

int main(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {

		std::string_view arg = argv[i];

		if (arg[0] == '-' && i + 1 < argc) {
			//ToDo
		}
		else if (arg.find("http") != std::string::npos) {
			//ToDo
		}
	}

	return CHaserMain();
}