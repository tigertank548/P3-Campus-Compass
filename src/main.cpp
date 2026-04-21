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

    // gets rid of the \n stuff of the first line
    std::string dummy;
    std::getline(std::cin, dummy);

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
                current += " " + nextName;
            }
            if (current.front() == '\"' && current.back() == '\"' && current.length() >= 2) {
                current = current.substr(1, current.length() - 2);
            }
            arguments.push_back(current);
        }
        
        int result = compass.parseCommand(command, arguments);
        if (result == 1)
            std::cout << "successful" << std::endl;
        else if (result == 0)
            std::cout << "unsuccessful" << std::endl;
    }

    return 0;
}

