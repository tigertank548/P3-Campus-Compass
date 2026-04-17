#include "CampusCompass.hpp"

#include <algorithm>
#include <string>
#include <fstream>
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

bool CampusCompass::parseCommand(const std::string& command, const std::vector<std::string>& arguments) {
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
            return false;
        }
        if (n + 4 != arguments.size())
            return false;

        for (int i = 4; i < n+4; i++)
            studentCourses.insert(arguments[i]);

        if (studentCourses.size() != n)
            return false;

        return insert(name,studentID,homeID,studentCourses);
    }if (command == "remove") {
        if (arguments.size() != 1)
            return false;
        return remove(arguments[0]);
    }if (command == "dropClass") {
        if (arguments.size() != 2)
            return false;

        return dropClass(arguments[0], arguments[1]);
    }if (command == "replaceClass") {
        if (arguments.size() != 3)
            return false;

        return replaceClass(arguments[0],arguments[1],arguments[2]);
    }if (command == "")
    return false;
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

