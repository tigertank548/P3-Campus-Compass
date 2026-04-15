#include <catch2/catch_test_macros.hpp>
#include <iostream>

// change if you choose to use a different header name
#include "CampusCompass.hpp"
#include "WeightedGraph.hpp"

TEST_CASE("Checking CSV importation") {
    WeightedGraph graph;
    graph.parseCSV("../data/edges.csv","..data/classes.csv");

    REQUIRE(!graph.checkEdge(1,2).first);
    REQUIRE(graph.checkEdge(1,2).second == 3);

    REQUIRE(!graph.checkEdge(6,16).first);
    REQUIRE(graph.checkEdge(6,16).second == 2);

    REQUIRE(graph.checkEdge(6,1).first);
    REQUIRE(graph.checkEdge(6,1).second == -1);

    graph.toggleEdgeClosure(1,50);
    REQUIRE(graph.checkEdge(1,50).first);
    REQUIRE(graph.checkEdge(1,50).second == 4);
}

TEST_CASE("Test Insert Validation") {
    CampusCompass test;

    // correct
    REQUIRE(test.insert("Namey", "12345678", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.insert("Name y", "84845315", 2, {"COP3504", "MAC2311"}));


    // TODO:: check inserting a student that's id already exists
    REQUIRE(!test.insert("Namey", "00000001", 1, {"COP3504", "MAC2311C"}));
    REQUIRE(!test.insert("Namey", "00000002", 1, {"COP35a4", "MAC2311"}));
    REQUIRE(!test.insert("Namey", "00000003", 1, {"COP3504", "MAC23115"}));
    REQUIRE(!test.insert("Namey", "00000004", 1, {}));
    REQUIRE(!test.insert("Namey", "00000005", 1, {"COP3504","COT3100", "COP3530", "PHY2048","PHY2049","IDS3935"}));
    REQUIRE(!test.insert("Namey", "00000006", 1, {"COP3504","COT3100", "COP3530", "PHY2048","PHY2049","IDS3935","MAP2302"}));

    REQUIRE(!test.insert("Namey 7", "00000008", 1, {"COP3504", "MAC2311"}));

}

TEST_CASE("Testing uniqueness") {
    CampusCompass test;

    // we have a student
    REQUIRE(test.insert("Namey", "12345678", 1, {"COP3504", "MAC2311"}));
    // cant have a duplicate student
    REQUIRE(!test.insert("Namey", "12345678", 1, {"COP3504", "MAC2311"}));
    // cant have a student with a non-existent home
    REQUIRE(!test.insert("Namey", "12345608", -1, {"COP3504", "MAC2311"}));
}

TEST_CASE("Testing student remove") {
    CampusCompass test;

    REQUIRE(test.insert("Namey", "12345678", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.remove("12345678"));
    REQUIRE(test.insert("Namey", "12345678", 1, {"COP3504", "MAC2311"}));

    REQUIRE(!test.remove("00000001"));

    // TODO:: I am almost certain there isn't enough testing here
}

TEST_CASE("Testing class dropping") {
    CampusCompass test;

    REQUIRE(test.insert("Namey", "00000001", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.insert("Namey", "00000002", 1, {"COP3504", "MAC2311"}));

    REQUIRE(test.dropClass("00000001", "COP3504"));
    REQUIRE(!test.dropClass("00000001", "COP3504"));
    REQUIRE(!test.dropClass("00000002", "COP3503"));
    REQUIRE(test.dropClass( "00000001", "MAC2311"));
    REQUIRE(std::get<0>(test.getStudentData("00000001")));
}

TEST_CASE("Testing replace class") {
    CampusCompass test;

    REQUIRE(test.insert("Namey", "00000001", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.insert("Namey", "00000002", 1, {"COP3504", "MAC2311"}));

    REQUIRE(test.replaceClass("00000001", "COP3504", "COP3502"));
    REQUIRE(!test.replaceClass("00000001", "COP3504", "COP3502"));
    REQUIRE(test.replaceClass("00000001", "COP3502", "COP3504"));
    REQUIRE(!test.replaceClass("00000001", "COP3503", "COT3100"));

    REQUIRE(!test.replaceClass("00000003", "COP3504", "COP3504"));
}

TEST_CASE("Testing remove class") {
    CampusCompass test;

    REQUIRE(test.insert("Namey", "00000001", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.insert("Namey", "00000002", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.insert("Namey", "00000001", 1, {"COP3504", "MAC2312"}));
    REQUIRE(test.insert("Namey", "00000002", 1, {"COP3504", "MAC2312"}));

    REQUIRE(test.removeClass("COP3504") == 4);
    // 0 means no student had it, or it was invalid, or blah blah blah
    REQUIRE(test.removeClass("COP3504") == 0);
    REQUIRE(test.removeClass("MAC2312") == 0);

}

TEST_CASE("testing toggle edge closure and check edge status") {
    CampusCompass test;

    REQUIRE(test.checkEdgeStatus(33,52).first);
    REQUIRE(test.checkEdgeStatus(52,33).first);
    REQUIRE(!test.checkEdgeStatus(33,52).second);
    REQUIRE(!test.checkEdgeStatus(52,33).second);

    REQUIRE(test.toggleEdgeClosure(33,52));
    REQUIRE(test.checkEdgeStatus(33,52).first);
    REQUIRE(test.checkEdgeStatus(52,33).first);
    REQUIRE(test.checkEdgeStatus(33,52).second);
    REQUIRE(test.checkEdgeStatus(52,33).second);

    REQUIRE(test.toggleEdgeClosure(33,52));
    REQUIRE(test.checkEdgeStatus(33,52).first);
    REQUIRE(!test.checkEdgeStatus(33,52).second);

    REQUIRE(!test.checkEdgeStatus(33,42).first);
    REQUIRE(!test.toggleEdgeClosure(33,42));
}

TEST_CASE("Testing is connectedness") {
    CampusCompass test;

    REQUIRE(test.isConnected(33,52));
    REQUIRE(test.isConnected(53,29));
    REQUIRE(test.isConnected(16,4));
    REQUIRE(test.isConnected(56,10));
    REQUIRE(!test.isConnected(33,32));
    test.toggleEdgeClosure(33,52);
    REQUIRE(!test.isConnected(52,33));
    // TODO:: check if there is a closure between nodes, but not directly(edges between the closed one and open ones)
}

TEST_CASE("testing shortest path") {
    CampusCompass test;

    REQUIRE(test.insert("Namey", "00000001", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.insert("Namey", "00000002", 1, { "MAC2311","COP3504"}));

    std::vector<std::string> shortestPath = test.printShortestEdges("0000001");
    REQUIRE(shortestPath == std::vector<std::string>({"COP3504: 23", "MAC2311: 18"}));

    shortestPath = test.printShortestEdges("0000002");
    REQUIRE(shortestPath == std::vector<std::string>({"COP3504: 23", "MAC2311: 18"}));//kind a silly test because it's an unordered set

    REQUIRE(test.insert("Namey", "00000003", 33, {"PHY2048", "MAC2311"}));

    shortestPath = test.printShortestEdges("0000003");
    REQUIRE(shortestPath == std::vector<std::string>({"MAC2311: -1", "PHY2048: -1"}));

    REQUIRE(test.insert("Namey", "00000004", 18, {"COP3504", "MAC2311"}));

    shortestPath = test.printShortestEdges("0000004");
    REQUIRE(shortestPath == std::vector<std::string>({"COP3504: 9", "MAC2311: 0"}));

    REQUIRE(test.insert("Namey", "00000005", 1, {"IDS2935", "MAC2313"}));
    REQUIRE(shortestPath == std::vector<std::string>({"IDS2935: 13", "MAC2313: 21"}));
}

TEST_CASE("Testing print student zone") {
    WeightedGraph graph;

    graph.insert(1,2,"a","a", 5);
    graph.insert(1,3,"a","a", 1);
    graph.insert(1,4,"a","a", 66);
    graph.insert(3,2,"a","a", 2);
    graph.insert(4,18,"a","a", 6);
    graph.insert(4,3,"a","a", 5);
    graph.insert(3,6,"a","a", 6);
    graph.insert(6,18,"a","a", 2);
    graph.insert(2,14,"a","a", 3);
    graph.insert(2,10,"a","a", 1);
    graph.insert(10,11,"a","a", 2);
    graph.insert(11,8,"a","a", 2);
    graph.insert(8,9,"a","a", 2);
    graph.insert(4,8,"a","a", 7);
    graph.insert(3,9,"a","a", 10);
    graph.insert(14,6,"a","a", 2);

    CampusCompass test;

    REQUIRE(test.insert("Namey", "00000001", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.insert("Namey", "00000002", 1, { "MAC2311","COP3504"}));

    REQUIRE(test.printStudentZone("00000001") == 25);

    test = CampusCompass(graph);

    REQUIRE(test.insert("Namey", "00000001", 1, {"COP3504", "MAC2311"}));

    REQUIRE(test.printStudentZone("00000001") == 10);
}

TEST_CASE("Testing Verify Schedule") {
    CampusCompass test;

    test.addClass("ABC1000", 1, "01:00", "01:45");
    test.addClass("ABC1100",14,"02:00","02:45");
    test.addClass("ABC1200",14,"03:00","03:45");
    test.addClass("ABC1300", 18, "03:15", "03:45");

    test.insert("Name", "00000001", 14, {"ABC1000","ABC1100", "ABC1200", "ABC1300"});

    std::vector<std::pair<std::string,bool>> out = test.verifySchedule("00000001");
    REQUIRE(out[0].first == "ABC1000");
    REQUIRE(out[0].second == true);
    REQUIRE(out[1].first == "ABC1100");
    REQUIRE(out[1].second == false);
    REQUIRE(out[2].first == "ABC1200");
    REQUIRE(out[2].second == true);
    REQUIRE(out[3].first == "ABC1300");
    REQUIRE(out[3].second == false);
    REQUIRE(out.size() == 4);
}

// TODO:: duplicate all these test cases using parseCommand