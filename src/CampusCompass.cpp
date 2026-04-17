#include "CampusCompass.hpp"

#include <algorithm>
#include <complex>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>


CampusCompass::CampusCompass() {
    const std::string edgesFilePath = "../data/edges.csv";
    const std::string classesFilePath = "../data/classes.csv";
    parseClassesCSV(classesFilePath);
    graph.parseEdgesCSV(edgesFilePath);
}
CampusCompass::CampusCompass(const WeightedGraph& graph) {
    this->graph = graph;
    parseClassesCSV("../data/classes.csv");
}

int CampusCompass::parseCommand(const std::string& command, const std::vector<std::string>& arguments) {
    if (command == "insert") {
        const std::string& name = arguments[0];
        const std::string& studentID = arguments[1];
        const std::string& homeIDString = arguments[2];
        const std::string& nString = arguments [3];
        std::unordered_set<std::string> studentCourses;
        int homeID, n;

        try {
            n = std::stoi(nString);
            homeID = std::stoi(homeIDString);
        }catch (std::invalid_argument& e) {
            return 0;
        }
        if (n + 4 != arguments.size())
            return 0;

        for (int i = 4; i < n+4; i++)
            studentCourses.insert(arguments[i]);

        if (studentCourses.size() != n)
            return 0;

        return insert(name,studentID,homeID,studentCourses) ? 1 : 0;
    }if (command == "remove") {
        if (arguments.size() != 1)
            return 0;
        return remove(arguments[0]) ? 1 : 0;
    }if (command == "dropClass") {
        if (arguments.size() != 2)
            return 0;

        return dropClass(arguments[0], arguments[1]) ? 1 : 0;
    }if (command == "replaceClass") {
        if (arguments.size() != 3)
            return 0;

        return replaceClass(arguments[0],arguments[1],arguments[2]) ? 1 : 0;
    }if (command == "removeClass") {
        if (arguments.size() != 1)
            return 0;

        return removeClass(arguments[0]);
    }if (command == "toggleEdgesClosure") {
        std::vector<std::pair<int,int>> edgesToToggle;
        try {
            int n;
            n = std::stoi(arguments[0]);
            if (arguments.size() - 1 != n)
                return 0;
            for (int i = 1; i < arguments.size(); i+=2)
                edgesToToggle.emplace_back(std::stoi(arguments[i]), std::stoi(arguments[i+1]));
        }catch (std::invalid_argument& e) {
            return 0;
        }

        std::vector<std::pair<int,int>> edgesToggled;

        bool success = true;

        for (std::pair<int,int> pair : edgesToToggle) {
            success = toggleEdgeClosure(pair.first,pair.second);
            if (!success)
                break;
            edgesToggled.push_back(pair);
        }

        if (!success)
            for (std::pair pair : edgesToggled)
                toggleEdgeClosure(pair.first, pair.second);

        return success ? 1 : 0;
    }if (command == "checkEdgeStatus") {
        if (arguments.size() != 2)
            return 0;
        try {
            std::pair<bool,bool> state = checkEdgeStatus(std::stoi(arguments[0]),std::stoi(arguments[1]));
            if (!state.first) {
                std::cout << "DNE" << std::endl;
            }else if (state.second) {
                std::cout << "closed" << std::endl;
            }else
                std::cout << "open" << std::endl;

            return -1;
        }catch (std::invalid_argument& e){
            return 0;
        }
    }if (command == "isConnected") {
        if (arguments.size() != 2)
            return 0;
        try {
            return isConnected(std::stoi(arguments[0]), std::stoi(arguments[1])) ? 1 : 0;
        }catch (std::invalid_argument& e) {
            return 0;
        }
    }if (command == "printShortestEdges") {
        if (arguments.size() != 1)
            return 0;
        std::vector<std::string> times = printShortestEdges(arguments[0]);

        std::cout << "Time For Shortest Edges: " << students[arguments[0]]->name << std::endl;
        for (std::string s : times)
            std::cout << s << std::endl;
        return -1;
    }if (command == "printStudentZone") {
        if (arguments.size() != 1)
            return 0;

        std::cout << "Student Zone Cost For " << students[arguments[0]]->name << ":" << printStudentZone(arguments[0]) << std::endl;
        return -1;
    }if (command == "verifySchedule") {
        if (arguments.size() != 1)
            return 0;

        std::vector<std::pair<std::string,bool>> result = verifySchedule(arguments[0]);

        if (result.empty())
            return 0;

        std::cout << "Schedule Check for " << students[arguments[0]]->name << ":" << std::endl;

        for (std::pair<std::string,bool> pair : result) {
            std::cout << pair.first << ": " << (pair.second ? "successful" : "unsuccessful") << std::endl;
        }
        return -1;
    }
    return 0;
}


bool CampusCompass::insert(std::string name, std::string studentID, const int home, const std::unordered_set<std::string>& courseCodes) {
    if (studentID.length() != 8)
        return false;
    if (!std::all_of(studentID.begin(), studentID.end(), ::isdigit))
        return false;
    // testing if the chars are all letters or spaces
    if (!std::all_of(name.begin(), name.end(), [](const char& letter)->bool {
        return std::isalpha(letter) || letter == ' ';
    }) && !name.empty())
        return false;
    if (courseCodes.size() < 1 || courseCodes.size() > 6)
        return false;
    if (!graph.nodeExists(home))
        return false;

    // student doesnt already exist
    if (students.find(studentID) != students.end())
        return false;

    // courses exist
    for (std::string course : courseCodes)
        if (courses.find(course) == courses.end())
            return false;

    students[studentID] = new Student{studentID, name, home, courseCodes};
    return true;
}

bool CampusCompass::remove(const std::string& studentID) {
    if (students.find(studentID) == students.end())
        return false;
    // iterate through all the courses and remove the student from them all
    for (const std::string& course : students[studentID]->courses)
        courses[course].students.erase(students[studentID]);
    Student* temp = students[studentID];
    students.erase(studentID);
    delete temp;
    return true;
}



bool CampusCompass::parseClassesCSV(const std::string& classesFilePath) {

    std::ifstream input = std::ifstream(classesFilePath);
    std::string line;

    std::getline(input, line); // discards headers
    while (std::getline(input, line)) {
        std::istringstream lineStream(line);


        std::string course, location, startTime, endTime;


        std::getline(lineStream, course, ',');
        std::getline(lineStream, location, ',');
        std::getline(lineStream, startTime, ',');
        std::getline(lineStream, endTime, ',');

        try {
            if (courses.find(course) != courses.end())
                return false;
            courses[course] = {course, std::stoi(location), startTime, endTime};
        }catch (const std::invalid_argument& e) {
            return false;
        }
    }

    return true;
}

