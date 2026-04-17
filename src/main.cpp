#include <iostream>
#include <sstream>

#include "CampusCompass.hpp"

int main() {
    CampusCompass compass;

    int n;
    std::cin >> n;

    if (std::cin.fail()) {
        std::cout << "unsuccessful" << std::endl;
        return -1;
    }

    for (int i = 0; i < n; i++) {
        std::string line;

        std::getline(std::cin, line);

        std::stringstream lineStream(line);

        std::string command;
        lineStream >> command;

        std::string current;

        std::vector<std::string> arguments;
        while (lineStream >> current) {
            while (current.front() == '\"' && current.back() != '\"') {
                std::string nextName;
                lineStream >> nextName;
                current += nextName;
            }
            arguments.push_back(current);
        }
        bool result = compass.parseCommand(command, arguments);
        if (result)
            std::cout << "successful" << std::endl;
        else
            std::cout << "unsuccessful" << std::endl;
    }

    return 0;
}

