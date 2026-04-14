#include <iostream>

#include "CampusCompass.h"

int main() {
    // initialize your main project object
    CampusCompass compass;

    // the below is example code for parsing commandline input
    int no_of_lines;
    std::string command;
    std::cin >> no_of_lines;
    std::cin.ignore(); // ignore newline that first cin left over
    for (int i = 0; i < no_of_lines; i++) {
        getline(std::cin, command);

        // parse your commands however you see fit
        compass.parseCommand(command);
    }
}
