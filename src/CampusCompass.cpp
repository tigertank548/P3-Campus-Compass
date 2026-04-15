#include "CampusCompass.hpp"

#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>


CampusCompass::CampusCompass() {
    parseCSV("../data/edges.csv","../data/classes.csv");
}
CampusCompass::CampusCompass(const WeightedGraph& graph) {
    this->graph = graph;
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



bool CampusCompass::parseCSV(const std::string &edges_filepath, const std::string &classes_filepath) {

    std::ifstream input = std::ifstream(classes_filepath);
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

    return graph.parseCSV(edges_filepath,classes_filepath);
}

