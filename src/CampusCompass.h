#pragma once
#include <string>
#include <unordered_set>

#include "WeightedGraph.hpp"


class CampusCompass {
public:
    // Think about what helper functions you will need in the algorithm
    CampusCompass(); // constructor
    bool parseCommand(const std::string &command);

    bool insert(std::string name, std::string studentID, int home, const std::unordered_set<std::string>& courseCodes);
    bool remove(const std::string& studentID);
    bool dropClass(std::string studentID, std::string classCode);
    bool replaceClass(std::string studentID, std::string dropped, std::string added);
    bool removeClass(std::string classCode);
    bool toggleEdgeClosure(int from, int to);
    // <exists, isClosed>
    std::pair<bool,bool> checkEdgeStatus(int from, int to);
    bool isConnected(int from, int to);
    // TODO:: should this return be a string?, how will this work?
    std::string printShortestEdges(int studentID);
    std::string printStudentZone(int studentID);

    //std::string verifySchedule(int studentID);


private:
    bool parseCSV(const std::string &edges_filepath, const std::string &classes_filepath);


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
