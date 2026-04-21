#pragma once
#include <string>
#include <unordered_set>

#include "WeightedGraph.hpp"


class CampusCompass {
public:
    CampusCompass();
    explicit CampusCompass(const WeightedGraph& graph);

    CampusCompass(CampusCompass& other);
    CampusCompass(CampusCompass&& other) noexcept ;
    CampusCompass& operator=(const CampusCompass& other);
    CampusCompass& operator=(CampusCompass&& other) noexcept ;
    ~CampusCompass();

    int parseCommand(const std::string& command, const std::vector<std::string>& arguments);

    bool insert(std::string name, std::string studentID, int home, const std::unordered_set<std::string>& courseCodes);
    bool remove(const std::string& studentID);
    bool dropClass(const std::string& studentID, const std::string& classCode);
    bool replaceClass(const std::string& studentID, const std::string& dropped, const std::string& added);
    int removeClass(const std::string& classCode);
    void addClass(const std::string& courseCode,const int& locationID,const std::string& startTime,const std::string& endTime);
    bool toggleEdgeClosure(int from, int to);
    // < studentExists, id, name, home, courses >
    std::tuple<bool, std::string, std::string, int, std::unordered_set<std::string>> getStudentData(const std::string& id);
    // < courseExists, courseCode, location, startTime, endTime >
    std::tuple<bool, std::string, int, std::string, std::string> getCourseData(const std::string& id);
    // <exists, isClosed>
    std::pair<bool,bool> checkEdgeStatus(int from, int to);
    bool isConnected(int from, int to);
    std::vector<std::string> printShortestEdges(const std::string& studentID);
    int printStudentZone(const std::string& studentID);

    // list of course code and whether it was successful, it will be in order of class times
    std::vector<std::pair<std::string,bool>> verifySchedule(const std::string& studentID);


private:
    bool parseClassesCSV(const std::string &classesFilePath);


    WeightedGraph graph;
    struct Student {
        std::string studentID; // this must also be accessible via the map of classes for removal from the students map just in case.
        std::string name;
        int homeID;
        std::unordered_set<std::string> courses;
    };
    // the key here is the student's student ID
    std::unordered_map<std::string, Student*> students;

    struct Course {
        std::string courseCode;
        int location;
        std::string startTime;
        std::string endTime;
        std::unordered_set<Student*> students;
    };

    // this is a map with classCodes as keys and values of the class struct.
    std::unordered_map<std::string, Course> courses;

};
