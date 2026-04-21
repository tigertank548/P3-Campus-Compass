#include <catch2/catch_test_macros.hpp>
#include <iostream>

// change if you choose to use a different header name
#include "CampusCompass.hpp"
#include "WeightedGraph.hpp"

/// CSV Tests

TEST_CASE("CSV import - Edges that exist") {
    WeightedGraph graph;
    graph.parseEdgesCSV("../data/edges.csv");

    REQUIRE(!graph.checkEdge(1, 2).first);
    REQUIRE(graph.checkEdge(1, 2).second == 3);

    REQUIRE(!graph.checkEdge(2, 1).first);
    REQUIRE(graph.checkEdge(2, 1).second == 3);

    REQUIRE(!graph.checkEdge(6, 16).first);
    REQUIRE(graph.checkEdge(6, 16).second == 2);

    REQUIRE(graph.checkEdge(6, 1).first);
    REQUIRE(graph.checkEdge(6, 1).second == -1);
}

TEST_CASE("CSV import - Edges that don't exist") {
    WeightedGraph graph;
    graph.parseEdgesCSV("../data/edges.csv");

    graph.toggleEdgeClosure(1, 50);
    REQUIRE(graph.checkEdge(1, 50).first);
    REQUIRE(graph.checkEdge(1, 50).second == 4);

    REQUIRE(graph.checkEdge(50, 1).first);
    REQUIRE(graph.checkEdge(50, 1).second == 4);
}

TEST_CASE("CSV import - Nodes that do and don't exist") {
    WeightedGraph graph;
    graph.parseEdgesCSV("../data/edges.csv");

    REQUIRE(graph.getNodeName(1) == "Hume Hall");
    REQUIRE(graph.getNodeName(20) == "McCarty Hall A");
    REQUIRE(graph.getNodeName(-1) == "NULL");
}

TEST_CASE("CSV import - Courses") {
    CampusCompass test;

    REQUIRE(test.getCourseData("IDS2935") ==
            std::tuple<bool, std::string, int, std::string, std::string>(
                {true, "IDS2935", 7, "16:05", "16:55"}));
    REQUIRE(test.getCourseData("MAC2313") ==
            std::tuple<bool, std::string, int, std::string, std::string>(
                {true, "MAC2313", 17, "12:50", "14:45"}));
}

/// Insert Tests

TEST_CASE("Insert - Valid inserts") {
    CampusCompass test;

    REQUIRE(test.insert("Namey", "12345678", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.insert("Name y", "84845315", 2, {"COP3504", "MAC2311"}));

    REQUIRE(test.insert("", "00000014", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.insert("   ", "00000015", 1, {"COP3504", "MAC2311"}));

    REQUIRE(test.insert("Name", "00000000", 1, {"COP3504"}));
    REQUIRE(test.insert("Name", "99999999", 1, {"COP3504"}));
}

TEST_CASE("Insert - Invalid Inserts") {
    CampusCompass test;

    // Invalid course codes
    REQUIRE(!test.insert("Namey", "00000001", 1, {"COP3504", "MAC2311C"}));
    REQUIRE(!test.insert("Namey", "00000002", 1, {"COP35a4", "MAC2311"}));
    REQUIRE(!test.insert("Namey", "00000003", 1, {"COP3504", "MAC23115"}));
    REQUIRE(!test.insert("Name", "00000010", 1, {"COp3504", "MAC2311"}));
    REQUIRE(!test.insert("Namey", "00000011", 1, {"COP350", "MAC2311"}));
    REQUIRE(!test.insert("Namey", "00000012", 1, {"CO3504", "MAC2311"}));

    // testing that the studentID isn't consumed from the bad insertion
    REQUIRE(test.insert("Namey", "00000004", 1, {"COP3504", "MAC2311"}));
}

TEST_CASE("Insert - varying number of courses ") {
    CampusCompass test;

    REQUIRE(!test.insert("Namey", "00000004", 1, {}));
    REQUIRE(test.insert(
        "Namey", "00000005", 1,
        {"COP3504", "COT3100", "COP3530", "PHY2048", "PHY2049", "IDS2935"}));
    REQUIRE(!test.insert("Namey", "00000006", 1,
        {"COP3504", "COT3100", "COP3530", "PHY2048", "PHY2049",
        "IDS2935", "MAP2302"}));
}

TEST_CASE("Insert - Invalid Names") {
    CampusCompass test;

    REQUIRE(!test.insert("Namey 7", "00000008", 1, {"COP3504", "MAC2311"}));
    REQUIRE(!test.insert("Name-y", "00000009", 1, {"COP3504", "MAC2311"}));
    REQUIRE(!test.insert("Name's", "00000013", 1, {"COP3504", "MAC2311"}));
}

TEST_CASE("Insert - Invalid Student ID's") {
    CampusCompass test;

    REQUIRE(!test.insert("Name y", "8484531", 2, {"COP3504", "MAC2311"}));
    REQUIRE(!test.insert("Name y", "848453155", 2, {"COP3504", "MAC2311"}));
    REQUIRE(!test.insert("Name y", "848A5315", 2, {"COP3504", "MAC2311"}));
    REQUIRE(!test.insert("Name y", "84845-15", 2, {"COP3504", "MAC2311"}));
}

TEST_CASE("Insert - misc") {
    CampusCompass test;

    // we have a student
    REQUIRE(test.insert("Namey", "12345678", 1, {"COP3504", "MAC2311"}));
    // cant have a duplicate student
    REQUIRE(!test.insert("Namey", "12345678", 1, {"COP3504", "MAC2311"}));
    // cant have a student with a non-existent home
    REQUIRE(!test.insert("Namey", "12345608", -1, {"COP3504", "MAC2311"}));
}

/// Remove tests

TEST_CASE("Remove - double remove") {
    CampusCompass test;

    REQUIRE(test.insert("Namey", "12345678", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.remove("12345678"));
    REQUIRE(!test.remove("12345678"));

    REQUIRE(std::get<0>(test.getStudentData("12345678")) == false);

    REQUIRE(test.insert("Namey", "12345678", 1, {"COP3504", "MAC2311"}));
}

TEST_CASE("Remove - remove non-existant") {
    CampusCompass test;

    REQUIRE(!test.remove("00000001"));

    REQUIRE(!test.remove("letters"));
    REQUIRE(!test.remove("12345678910"));
    REQUIRE(!test.remove("1"));
}

TEST_CASE("Remove - making sure remove removes from courses") {
    CampusCompass test;

    test.insert("Namey", "00000001", 1, {"COP3504", "MAC2312"});
    test.insert("Namey", "00000002", 1, {"COP3504", "MAC2312"});
    REQUIRE(test.remove("00000001"));
    REQUIRE(test.removeClass("MAC2312") == 1);
}

/// dropClass

TEST_CASE("dropClass") {
    CampusCompass test;

    REQUIRE(test.insert("Namey", "00000001", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.insert("Namey", "00000002", 1, {"COP3504", "MAC2311"}));

    // drop the same class twice
    REQUIRE(test.dropClass("00000001", "COP3504"));
    REQUIRE(!test.dropClass("00000001", "COP3504"));

    // student and class exist but student doesn't have that class
    REQUIRE(!test.dropClass("00000002", "COP3503"));

    // gets rid of the student from dropping all classes
    REQUIRE(test.dropClass("00000001", "MAC2311"));
    REQUIRE(std::get<0>(test.getStudentData("00000001")) == false);

    // non-existent course
    REQUIRE(!test.dropClass("00000002", "ABC1000"));

    // deleted student
    REQUIRE(!test.dropClass("00000001", "COP3502"));

    // non-existent student
    REQUIRE(!test.dropClass("00000000", "COP3504"));
}

TEST_CASE("dropClass - invalid arguments") {
    CampusCompass test;

    REQUIRE(!test.dropClass("100", "COP3504"));
    REQUIRE(!test.dropClass("00000002", "fire1"));
}

/// replaceClass

TEST_CASE("replaceClass") {
    CampusCompass test;

    REQUIRE(test.insert("Namey", "00000001", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.insert("Namey", "00000002", 1, {"COP3504", "MAC2311"}));

    REQUIRE(test.replaceClass("00000001", "COP3504", "COP3502"));
    REQUIRE(!test.replaceClass("00000001", "COP3504", "COP3502"));
    REQUIRE(test.replaceClass("00000001", "COP3502", "COP3504"));
    REQUIRE(!test.replaceClass("00000001", "COP3503", "COT3100"));

    REQUIRE(!test.replaceClass("00000003", "COP3504", "COP3504"));

    test.insert("m", "00000004", 1, {"COP3504", "COP3502"});
    REQUIRE(!test.replaceClass("00000004", "COP3504", "COP3502"));
    REQUIRE(std::get<4>(test.getStudentData("00000004")) ==
        std::unordered_set<std::string>({"COP3504", "COP3502"}));
    REQUIRE(!test.replaceClass("00000004", "COP3504", "ABC1500"));
    REQUIRE(std::get<4>(test.getStudentData("00000004")) ==
        std::unordered_set<std::string>({"COP3504", "COP3502"}));
    REQUIRE(!test.replaceClass("00000004", "COP3504", "COP3504"));
    REQUIRE(std::get<4>(test.getStudentData("00000004")) ==
        std::unordered_set<std::string>({"COP3504", "COP3502"}));
    REQUIRE(!test.replaceClass("00000004", "COP3504", "bs"));
}

/// removeClass

TEST_CASE("removeClass") {
    CampusCompass test;

    REQUIRE(test.insert("Namey", "00000001", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.insert("Namey", "00000002", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.insert("Namey", "00000003", 1, {"COP3504", "MAC2312"}));
    REQUIRE(test.insert("Namey", "00000004", 1, {"COP3504", "MAC2312"}));

    REQUIRE(test.removeClass("COP3504") == 4);
    REQUIRE(test.removeClass("COP3504") == 0);
    REQUIRE(test.removeClass("MAC2312") == 2);
    REQUIRE(test.removeClass("COP3530") == 0);

    // checking if the student exists after removing all classes
    REQUIRE(std::get<4>(test.getStudentData("00000001")) == std::unordered_set<std::string>({"MAC2311"}));
    REQUIRE(std::get<0>(test.getStudentData("00000003")) == false);
    REQUIRE(std::get<0>(test.getStudentData("00000004")) == false);

    // invalid classes — removeClass returns -1 for classes not in courses map
    REQUIRE(test.removeClass("1") == -1);
    REQUIRE(test.removeClass("ABC2000") == -1);

    // lil swappy wacky thing
    REQUIRE(test.replaceClass("00000001", "MAC2311", "MAC2312"));
    REQUIRE(test.insert("Namey", "00000003", 1, {"COP3504", "MAC2312"}));
    REQUIRE(test.removeClass("MAC2312") == 2);
}

/// edge closures and status

TEST_CASE("testing toggle edge closure and check edge status") {
    CampusCompass test;

    REQUIRE(test.checkEdgeStatus(33, 52).first);
    REQUIRE(test.checkEdgeStatus(52, 33).first);
    REQUIRE(!test.checkEdgeStatus(33, 52).second);
    REQUIRE(!test.checkEdgeStatus(52, 33).second);

    REQUIRE(test.toggleEdgeClosure(33, 52));
    REQUIRE(test.checkEdgeStatus(33, 52).first);
    REQUIRE(test.checkEdgeStatus(52, 33).first);
    REQUIRE(test.checkEdgeStatus(33, 52).second);
    REQUIRE(test.checkEdgeStatus(52, 33).second);

    REQUIRE(test.toggleEdgeClosure(33, 52));
    REQUIRE(test.checkEdgeStatus(33, 52).first);
    REQUIRE(!test.checkEdgeStatus(33, 52).second);

    REQUIRE(!test.checkEdgeStatus(33, 42).first);
    REQUIRE(!test.toggleEdgeClosure(33, 42));

    REQUIRE(!test.checkEdgeStatus(-1, 3).first);
    REQUIRE(!test.toggleEdgeClosure(-1, 3));
}

/// isConnected

TEST_CASE("isConnected - without closures") {
    CampusCompass test;

    REQUIRE(test.isConnected(33, 52));
    REQUIRE(test.isConnected(53, 29));
    REQUIRE(test.isConnected(16, 4));
    REQUIRE(test.isConnected(56, 10));
    REQUIRE(test.isConnected(10, 56));
    REQUIRE(!test.isConnected(33, 32));
    REQUIRE(test.isConnected(1, 4));
    REQUIRE(test.isConnected(56, 10));
    REQUIRE(test.isConnected(56, 56));
    REQUIRE(!test.isConnected(56, -1));
    REQUIRE(!test.isConnected(26, 1));
    REQUIRE(!test.isConnected(40, 1));
    REQUIRE(test.isConnected(26, 36));
    REQUIRE(test.isConnected(40, 51));
    REQUIRE(!test.isConnected(26, 40));
    REQUIRE(test.isConnected(1, 56));
}

TEST_CASE("isConnected - with closures") {
    CampusCompass test;

    test.toggleEdgeClosure(33, 52);
    REQUIRE(!test.isConnected(52, 33));

    REQUIRE(test.toggleEdgeClosure(1, 4));
    REQUIRE(test.isConnected(4, 1));

    REQUIRE(test.toggleEdgeClosure(56, 49));
    REQUIRE(!test.isConnected(10, 56));

    test.toggleEdgeClosure(43, 6);
    test.toggleEdgeClosure(43, 7);
    REQUIRE(!test.isConnected(10, 43));
    REQUIRE(!test.isConnected(43, 10));

    test.toggleEdgeClosure(49, 56);
    REQUIRE(test.isConnected(1, 56));
}

/// shortestPath

TEST_CASE("printShortestEdges") {
    CampusCompass test;

    REQUIRE(test.insert("Namey", "00000001", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.insert("Namey", "00000002", 1, {"MAC2311", "COP3504"}));

    std::vector<std::string> shortestPath = test.printShortestEdges("00000001");
    REQUIRE(shortestPath ==
        std::vector<std::string>({"COP3504: 23", "MAC2311: 18"}));

    shortestPath = test.printShortestEdges("00000002");
    REQUIRE(
        shortestPath ==
        std::vector<std::string>(
            {"COP3504: 23",
            "MAC2311: 18"})); // kind a silly test because it's an unordered set

    REQUIRE(test.insert("Namey", "00000003", 33, {"PHY2048", "MAC2311"}));

    shortestPath = test.printShortestEdges("00000003");
    REQUIRE(shortestPath ==
        std::vector<std::string>({"MAC2311: -1", "PHY2048: -1"}));

    REQUIRE(test.insert("Namey", "00000004", 18, {"COP3504", "MAC2311"}));

    shortestPath = test.printShortestEdges("00000004");
    REQUIRE(shortestPath ==
        std::vector<std::string>({"COP3504: 9", "MAC2311: 0"}));

    REQUIRE(test.insert("Namey", "00000005", 1, {"IDS2935", "MAC2313"}));
    shortestPath = test.printShortestEdges("00000005");
    REQUIRE(shortestPath ==
        std::vector<std::string>({"IDS2935: 13", "MAC2313: 21"}));

    REQUIRE(test.insert("Namey", "00000006", 1, {"PHY2048"}));
    shortestPath = test.printShortestEdges("00000006");
    REQUIRE(shortestPath == std::vector<std::string>({"PHY2048: 20"}));
    test.toggleEdgeClosure(49, 56);
    REQUIRE(test.printShortestEdges("00000006")[0] == "PHY2048: -1");
    REQUIRE(test.printShortestEdges("00000000").empty());
    REQUIRE(test.printShortestEdges("0").empty());
}

/// printStudentZone

TEST_CASE("Testing print student zone") {
    WeightedGraph graph;

    graph.insert(1, 2, "a", "a", 5);
    graph.insert(1, 3, "a", "a", 1);
    graph.insert(1, 4, "a", "a", 66);
    graph.insert(3, 2, "a", "a", 2);
    graph.insert(4, 18, "a", "a", 6);
    graph.insert(4, 3, "a", "a", 5);
    graph.insert(3, 6, "a", "a", 6);
    graph.insert(6, 18, "a", "a", 2);
    graph.insert(2, 14, "a", "a", 3);
    graph.insert(2, 10, "a", "a", 1);
    graph.insert(10, 11, "a", "a", 2);
    graph.insert(11, 8, "a", "a", 2);
    graph.insert(8, 9, "a", "a", 2);
    graph.insert(4, 8, "a", "a", 7);
    graph.insert(3, 9, "a", "a", 10);
    graph.insert(14, 6, "a", "a", 2);

    CampusCompass test;

    REQUIRE(test.insert("Namey", "00000001", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.insert("Namey", "00000002", 1, {"MAC2311", "COP3504"}));

    REQUIRE(test.printStudentZone("00000001") == 25);

    test.addClass("ABC1000", 56, "01:11", "01:12");

    REQUIRE(test.insert("Namey", "00000003", 1, {"ABC1000"}));

    REQUIRE(test.printStudentZone("00000003") == 20);

    test = CampusCompass(graph);

    REQUIRE(test.insert("Namey", "00000001", 1, {"COP3504", "MAC2311"}));

    REQUIRE(test.printStudentZone("00000001") == 11);

    test.toggleEdgeClosure(6, 14);

    REQUIRE(test.printStudentZone("00000001") == 14);

    REQUIRE(test.printStudentZone("00000000") == -1);

    REQUIRE(test.insert("Namey", "00000002", 1, {"MAC2312", "MAC2311"}));

    REQUIRE(test.printStudentZone("00000002") == 9);
}

/// verifySchedule

TEST_CASE("Testing Verify Schedule") {
    CampusCompass test;

    test.addClass("ABC1000", 1, "01:00", "01:45");
    test.addClass("ABC1100", 14, "02:00", "02:45");
    test.addClass("ABC1200", 14, "03:00", "03:45");
    test.addClass("ABC1300", 18, "03:15", "03:45");

    test.insert("Name", "00000001", 14,
                {"ABC1100", "ABC1000", "ABC1300", "ABC1200"});

    std::vector<std::pair<std::string, bool>> out =
        test.verifySchedule("00000001");
    REQUIRE(out[0].first == "ABC1000 - ABC1100");
    REQUIRE(out[0].second == false);
    REQUIRE(out[1].first == "ABC1100 - ABC1200");
    REQUIRE(out[1].second == true);
    REQUIRE(out[2].first == "ABC1200 - ABC1300");
    REQUIRE(out[2].second == false);
    REQUIRE(out.size() == 3);

    test.insert("Name", "00000002", 1, {"ABC1000"});

    REQUIRE(test.verifySchedule("00000002").empty());
    REQUIRE(test.verifySchedule("1").empty());

    test.addClass("ABC1400", 14, "02:08", "02:55");

    test.insert("name", "00000003", 1, {"ABC1000", "ABC1400"});
    out = test.verifySchedule("00000003");
    REQUIRE(out[0].first == "ABC1000 - ABC1400");
    REQUIRE(out[0].second == true);

    test.toggleEdgeClosure(15, 13);

    out = test.verifySchedule("00000003");
    REQUIRE(out[0].first == "ABC1000 - ABC1400");
    REQUIRE(out[0].second == false);
}
