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

CampusCompass::CampusCompass(CampusCompass& other) {
    graph = other.graph;
    students = other.students;
    courses = other.courses;
}
CampusCompass::CampusCompass(CampusCompass&& other) noexcept {
    graph = std::move(other.graph);
    students = std::move(other.students);
    courses = std::move(other.courses);
}
CampusCompass& CampusCompass::operator=(const CampusCompass& other) {
    if (this == &other)
        return *this;
    graph = other.graph;
    students = other.students;
    courses = other.courses;
   return *this;
}
CampusCompass& CampusCompass::operator=(CampusCompass&& other) noexcept {
    if (this == &other)
        return *this;
    graph = std::move(other.graph);
    students = std::move(other.students);
    courses = std::move(other.courses);
    return *this;
}
CampusCompass::~CampusCompass() {
    for (const std::pair<std::string, Student*> pair : students)
        delete pair.second;
}

int CampusCompass::parseCommand(const std::string& command, const std::vector<std::string>& arguments) {
    if (command == "insert") {
        if (arguments.size() < 4)
            return 0;
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
        if (n + 4 != static_cast<int>(arguments.size()))
            return 0;

        for (int i = 4; i < n+4; i++)
            studentCourses.insert(arguments[i]);

        if (static_cast<int>(studentCourses.size()) != n)
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
        const int result = removeClass(arguments[0]);
        if (result == -1 || result == 0)
            return 0;

        std::cout << result << std::endl;

        return -1;
    }if (command == "toggleEdgesClosure") {
        std::vector<std::pair<int,int>> edgesToToggle;
        try {
            const int n = std::stoi(arguments[0]);
            if (static_cast<int>(arguments.size() - 1) != n)
                return 0;
            for (std::size_t i = 1; i < arguments.size(); i+=2)
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
        if (students.find(arguments[0]) == students.end())
            return 0;

        const std::vector<std::string> times = printShortestEdges(arguments[0]);

        std::cout << "Time For Shortest Edges: " << students[arguments[0]]->name << std::endl;
        for (std::string s : times)
            std::cout << s << std::endl;
        return -1;
    }if (command == "printStudentZone") {
        if (arguments.size() != 1)
            return 0;
        if (students.find(arguments[0]) == students.end())
            return 0;

        std::cout << "Student Zone Cost For " << students[arguments[0]]->name << ":" << printStudentZone(arguments[0]) << std::endl;
        return -1;
    }if (command == "verifySchedule") {
        if (arguments.size() != 1)
            return 0;
        if (students.find(arguments[0]) == students.end())
            return 0;

        std::vector<std::pair<std::string,bool>> result = verifySchedule(arguments[0]);

        if (result.empty())
            return 0;

        std::cout << "Schedule Check for " << students[arguments[0]]->name << ":" << std::endl;

        for (const std::pair<std::string,bool>& pair : result) {
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
    if (courseCodes.empty() || courseCodes.size() > 6)
        return false;
    if (!graph.nodeExists(home))
        return false;

    // student doesnt already exist
    if (students.find(studentID) != students.end())
        return false;

    // courses exist
    for (const std::string& course : courseCodes)
        if (courses.find(course) == courses.end())
            return false;

    students[studentID] = new Student{studentID, name, home, courseCodes};
    for (const std::string& course : courseCodes)
        courses[course].students.insert(students[studentID]);

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

bool CampusCompass::dropClass(const std::string& studentID, const std::string& classCode) {
    if (students.find(studentID) == students.end())
        return false;

    Student* student = students[studentID];

    if (student->courses.find(classCode) == student->courses.end())
        return false;

    student->courses.erase(classCode);
    courses[classCode].students.erase(student);

    if (student->courses.empty()) {
        students.erase(studentID);
        delete student;
    }

    return true;
}

bool CampusCompass::replaceClass(const std::string& studentID, const std::string& dropped, const std::string& added) {
    if (students.find(studentID) == students.end())
        return false;

    Student* student = students[studentID];

    if (student->courses.find(dropped) == student->courses.end())
        return false;
    if (student->courses.find(added) != student->courses.end())
        return false;
    if (courses.find(added) == courses.end())
        return false;

    student->courses.erase(dropped);
    courses[dropped].students.erase(student);

    student->courses.insert(added);
    courses[added].students.insert(student);

    return true;
}

int CampusCompass::removeClass(const std::string& classCode) {
    if (courses.find(classCode) == courses.end())
        return -1;

    for (auto iter = students.begin(); iter != students.end();) {
        Student* student = iter->second;
        student->courses.erase(classCode);
        if (student->courses.empty()) {
            iter = students.erase(iter);
            delete student;
        }else
            ++iter;
    }

    const int out = static_cast<int>(courses[classCode].students.size());
    courses[classCode].students.clear();
    return out;
}

void CampusCompass::addClass(const std::string& courseCode,const int& locationID,const std::string& startTime,const std::string& endTime) {
    courses[courseCode] = {courseCode, locationID, startTime, endTime, {}};
}

bool CampusCompass::toggleEdgeClosure(int from, int to) {
    return graph.toggleEdgeClosure(from,to);
}

std::tuple<bool, std::string, std::string, int, std::unordered_set<std::string>> CampusCompass::getStudentData(const std::string& id) {
    if (students.find(id) == students.end())
        return {false,"","",0,{}};

    Student* student = students[id];
    return {true, student->studentID, student->name, student->homeID, student->courses};
}

std::tuple<bool, std::string, int, std::string, std::string> CampusCompass::getCourseData(const std::string& id) {
    if (courses.find(id) == courses.end())
        return {false, "", 0, "", ""};

    Course* course = &courses[id];
    return {true, course->courseCode, course->location, course->startTime, course->endTime};
}

std::pair<bool, bool> CampusCompass::checkEdgeStatus(const int from, const int to) {
    return graph.checkEdgeStatus(from,to);
}

bool CampusCompass::isConnected(int from, int to) {
    return graph.isConnected(from, to);
}

std::vector<std::string> CampusCompass::printShortestEdges(const std::string& studentID) {
    if (students.find(studentID) == students.end())
        return {};

    Student* student = students[studentID];

    std::unordered_map<int, std::pair<int,int>> result = graph.dijkstras(student->homeID);

    std::vector<std::string> out;

    for (const std::string& course : student->courses) {
        if (result.find(courses[course].location) == result.end())
            out.push_back(course + ": -1");
        else
            out.push_back(course + ": " + std::to_string(result[courses[course].location].first));
    }

    std::sort(out.begin(), out.end());

    return out;
}

int CampusCompass::printStudentZone(const std::string& studentID) {
    if (students.find(studentID) == students.end())
        return -1;

    std::unordered_set<int> nodes;

    nodes.insert(students[studentID]->homeID);

    for (const std::string& course : students[studentID]->courses)
        nodes.insert(courses[course].location);

    return graph.mstOfNodes(nodes);
}

std::vector<std::pair<std::string, bool>> CampusCompass::verifySchedule(const std::string& studentID) {
    if (students.find(studentID) == students.end())
        return {};

    Student* student  = students[studentID];

    if (student->courses.size() == 1)
        return {};

    struct courseTime {
        int startTime;
        int endTime;
        std::string course;


        bool operator>(const courseTime& other) const {
            return startTime > other.startTime;
        }
        bool operator<(const courseTime& other) const {
            return startTime < other.startTime;
        }
        bool operator>=(const courseTime& other) const {
            return startTime >= other.startTime;
        }
        bool operator<=(const courseTime& other) const {
            return startTime < other.startTime;
        }
    };

    std::vector<courseTime> times;
    for (const std::string& course : student->courses) {
        const Course& c = courses[course];

        std::string startString = c.startTime;
        std::string endString = c.endTime;

        int startTime = std::stoi(startString.substr(0,2))*60 + std::stoi(startString.substr(3,2));
        int endTime = std::stoi(endString.substr(0,2))*60 + std::stoi(endString.substr(3,2));
        times.push_back({startTime,endTime,course});
    }

    std::sort(times.begin(), times.end());

    std::vector<std::pair<std::string,bool>> out;

    for (std::size_t i = 0; i < times.size() - 1; i++) {
        const Course& currentClass = courses[times[i].course];
        const Course& nextClass = courses[times[i + 1].course];
        std::unordered_map<int, std::pair<int,int>> result = graph.dijkstras(currentClass.location);
        int timeToWalk;
        if (result.find(nextClass.location) == result.end())
            timeToWalk = -1;
        else
            timeToWalk = result[nextClass.location].first;
        if (timeToWalk == -1)
            out.emplace_back(times[i].course + " - " + times[i + 1].course, false);
        else if (timeToWalk > times[i + 1].startTime - times[i].endTime)
            out.emplace_back(times[i].course + " - " + times[i + 1].course, false);
        else
            out.emplace_back(times[i].course + " - " + times[i + 1].course, true);
    }
    return out;
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

        // Makes sure that there are no \r or other wacky end characters
        while (endTime.size() != 5)
            endTime.pop_back();


        try {
            if (courses.find(course) != courses.end())
                return false;
            courses[course] = {course, std::stoi(location), startTime, endTime, {}};
        }catch (const std::invalid_argument& e) {
            return false;
        }
    }

    return true;
}

