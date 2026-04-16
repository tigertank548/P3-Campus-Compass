#include <catch2/catch_test_macros.hpp>
#include <iostream>

// change if you choose to use a different header name
#include "CampusCompass.hpp"
#include "WeightedGraph.hpp"

/// CSV Tests

TEST_CASE("CSV import - Edges that exist") {
  WeightedGraph graph;

  REQUIRE(!graph.checkEdge(1, 2).first);
  REQUIRE(graph.checkEdge(1, 2).second == 3);

  REQUIRE(!graph.checkEdge(2, 1).first);
  REQUIRE(graph.checkEdge(2, 1).second == 3);

  REQUIRE(!graph.checkEdge(6, 16).first);
  REQUIRE(graph.checkEdge(6, 16).second == 2);
}
TEST_CASE("CSV import - Edges that exist") {
  WeightedGraph graph;

  REQUIRE(graph.checkEdge(6, 1).first);
  REQUIRE(graph.checkEdge(6, 1).second == -1);
}
TEST_CASE("CSV import - Edges that don't exist") {
  WeightedGraph graph;

  graph.toggleEdgeClosure(1, 50);
  REQUIRE(graph.checkEdge(1, 50).first);
  REQUIRE(graph.checkEdge(1, 50).second == 4);

  REQUIRE(graph.checkEdge(50, 1).first);
  REQUIRE(graph.checkEdge(50, 1).second == 4);
}
TEST_CASE("CSV import - Nodes that do and don't exist") {
  WeightedGraph graph;

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
  // 0 means no student had it, or it was invalid, or blah blah blah
  REQUIRE(test.removeClass("COP3504") == 0);
  REQUIRE(test.removeClass("MAC2312") == 2);
  REQUIRE(test.removeClass("COP3530") == 0);

  // checking if the student exists after removing all classes
  REQUIRE(std::get<4>(test.getStudentData("00000001")) == std::unordered_set<std::string>({"MAC2311"}));
  REQUIRE(std::get<0>(test.getStudentData("00000003")) == false);
  REQUIRE(std::get<0>(test.getStudentData("00000004")) == false);

  // invalid classes
  REQUIRE(test.removeClass("1") == 0);
  REQUIRE(test.removeClass("ABC2000") == 0);

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
  REQUIRE(!test.isConnected(1, 56));
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

  REQUIRE(test.printStudentZone("00000002") == 18);
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

/// TODO:: PLEASE PLEASE DELETE BELOW BEFORE SUBMISSION

// TODO:: duplicate all these test cases using parseCommand
// need to test if the time insertion of students is in correct format
// need to check for failure if the console input has duplicate class for a
// student
// need to test if there is a failure on one of the parameters in
// toggleEdgeClosure that none of the edges were toggled.
//




TEST_CASE("isConnected — both nodes invalid") {
  CampusCompass test;
  REQUIRE(!test.isConnected(-1, -2));
  REQUIRE(!test.isConnected(999, 1000));
}

TEST_CASE("isConnected — long chain connectivity") {
  CampusCompass test;
  // Test connectivity between nodes that require traversing many edges
  // e.g., node 1 to node 56 (requires: 1→50→2→54... or similar long path)
  REQUIRE(test.isConnected(1, 56));
  // Now close a critical bottleneck
  test.toggleEdgeClosure(49, 56);
  REQUIRE(!test.isConnected(1, 56));
}

TEST_CASE("isConnected — disconnected components in full graph") {
  CampusCompass test;
  // Nodes 26 and 36 form their own connected component
  // along with 40 and 51
  // These are NOT connected to the main graph!
  REQUIRE(!test.isConnected(26, 1));
  REQUIRE(!test.isConnected(40, 1));
  REQUIRE(test.isConnected(26, 36));
  REQUIRE(test.isConnected(40, 51));
  REQUIRE(!test.isConnected(26, 40)); // two separate tiny components
}

TEST_CASE("printShortestEdges — lexicographic ordering") {
  CampusCompass test;
  // Spec says classes sorted lexicographically
  test.insert("Name", "00000001", 1, {"MAC2311", "COP3504", "COT3100"});
  auto result = test.printShortestEdges("00000001");
  // Must be in order: COP3504, COT3100, MAC2311
  REQUIRE(result.size() == 3);
  REQUIRE(result[0].substr(0, 6) == "COP350");
  REQUIRE(result[1].substr(0, 6) == "COT310");
  REQUIRE(result[2].substr(0, 6) == "MAC231");
}

TEST_CASE("printShortestEdges — student with all classes at same location") {
  CampusCompass test;
  // COP3504, COP3530, CDA3101, CIS4301, COP4533 all at location 14
  test.insert("Name", "00000001", 14, {"COP3504", "COP3530", "CDA3101"});
  auto result = test.printShortestEdges("00000001");
  // All distances should be 0
  for (auto& s : result) {
    REQUIRE(s.find(": 0") != std::string::npos);
  }
}

TEST_CASE("printShortestEdges — student in disconnected component") {
  CampusCompass test;
  // Node 26 is only connected to 36 — no classes are at 26 or 36
  test.addClass("TST1000", 26, "10:00", "11:00");
  test.addClass("TST1001", 1, "12:00", "13:00");
  test.insert("Name", "00000001", 26, {"TST1000", "TST1001"});
  auto result = test.printShortestEdges("00000001");
  REQUIRE(result[0] == "TST1000: 0");  // same location
  REQUIRE(result[1] == "TST1001: -1"); // unreachable
}

TEST_CASE("printShortestEdges — invalid student ID") {
  CampusCompass test;
  REQUIRE(test.printShortestEdges("invalid").empty());
  REQUIRE(test.printShortestEdges("").empty());
}

TEST_CASE("printStudentZone — student with single class at home") {
  CampusCompass test;
  // Student lives at location 14, only class also at location 14
  test.insert("Name", "00000001", 14, {"COP3504"});
  // Subgraph has just one node, MST cost = 0
  REQUIRE(test.printStudentZone("00000001") == 0);
}

TEST_CASE("printStudentZone — student with all classes at same location") {
  CampusCompass test;
  test.insert("Name", "00000001", 14, {"COP3504", "COP3530", "CDA3101"});
  // All at location 14, same as checking if home=14 connects with cost 0
  // If home is also 14:
  REQUIRE(test.printStudentZone("00000001") == 0);
}

TEST_CASE("printStudentZone — invalid student") {
  CampusCompass test;
  REQUIRE(test.printStudentZone("invalid") == -1);
  REQUIRE(test.printStudentZone("") == -1);
}

TEST_CASE("verifySchedule — classes at same location always feasible") {
    CampusCompass test;
    // COP3504 at 14, 11:45-12:35 and COP3530 at 14, 10:40-11:30
    // Same location, so travel time = 0
    test.insert("Name", "00000001", 1, {"COP3530", "COP3504"});
    auto out = test.verifySchedule("00000001");
    REQUIRE(out.size() == 1);
    REQUIRE(out[0].first == "COP3530 - COP3504");
    REQUIRE(out[0].second == true); // 15 min gap, 0 travel time
}

TEST_CASE("verifySchedule — overlapping classes (time gap = 0 or negative)") {
    CampusCompass test;
    // Create classes with overlapping times
    test.addClass("TST1000", 1, "10:00", "11:00");
    test.addClass("TST1001", 14, "10:30", "11:30"); // starts during TST1000
    test.insert("Name", "00000001", 1, {"TST1000", "TST1001"});
    auto out = test.verifySchedule("00000001");
    REQUIRE(out.size() == 1);
    // Time gap is negative (-30 min), which is < shortest path → unsuccessful
    REQUIRE(out[0].second == false);
}

TEST_CASE("verifySchedule — exactly enough time (boundary)") {
    CampusCompass test;
    // The spec says: time gap >= shortest path → successful
    //                time gap < shortest path → unsuccessful
    test.addClass("TST1000", 1, "10:00", "10:50");
    // Edge 1↔2 has weight 3, so if TST1001 is at location 2:
    test.addClass("TST1001", 2, "10:53", "11:30"); // gap = 3 min, shortest = 3 → successful
    test.insert("Name", "00000001", 1, {"TST1000", "TST1001"});
    auto out = test.verifySchedule("00000001");
    REQUIRE(out[0].second == true); // exactly equal

    // Now test gap = 2, shortest = 3 → unsuccessful
    CampusCompass test2;
    test2.addClass("TST2000", 1, "10:00", "10:50");
    test2.addClass("TST2001", 2, "10:52", "11:30"); // gap = 2, shortest = 3
    test2.insert("Name", "00000002", 1, {"TST2000", "TST2001"});
    auto out2 = test2.verifySchedule("00000002");
    REQUIRE(out2[0].second == false);
}

TEST_CASE("verifySchedule — classes sorted by start time, not code") {
    CampusCompass test;
    // Ensure output is ordered by class start time, not alphabetically
    // ZZZ1000 starts at 08:00, AAA1000 starts at 09:00
    test.addClass("ZZZ1000", 1, "08:00", "08:50");
    test.addClass("AAA1000", 1, "09:00", "09:50");
    test.insert("Name", "00000001", 1, {"AAA1000", "ZZZ1000"});
    auto out = test.verifySchedule("00000001");
    REQUIRE(out[0].first == "ZZZ1000 - AAA1000"); // ordered by start time
}

TEST_CASE("verifySchedule — unreachable class in middle of schedule") {
    CampusCompass test;
    // If ClassCode1→ClassCode2 is unreachable, still calculate ClassCode2→ClassCode3
    test.addClass("TST1000", 1, "08:00", "08:50");
    test.addClass("TST1001", 26, "09:00", "09:50");  // node 26 is isolated!
    test.addClass("TST1002", 1, "10:00", "10:50");
    test.insert("Name", "00000001", 1, {"TST1000", "TST1001", "TST1002"});
    auto out = test.verifySchedule("00000001");
    REQUIRE(out.size() == 2);
    REQUIRE(out[0].second == false);  // can't reach node 26
    // TST1001 → TST1002: from 26 to 1, unreachable too
    REQUIRE(out[1].second == false);
}


TEST_CASE("Remove — verify student is cleaned from course rosters") {
  CampusCompass test;
  test.insert("Name", "00000001", 1, {"COP3504", "MAC2311"});
  test.insert("Name", "00000002", 1, {"COP3504"});
  test.remove("00000001");
  // removeClass COP3504 should now only affect 1 student
  REQUIRE(test.removeClass("COP3504") == 1);
}

TEST_CASE("dropClass — dropping last class removes student") {
  CampusCompass test;
  test.insert("Name", "00000001", 1, {"COP3504"});
  REQUIRE(test.dropClass("00000001", "COP3504"));
  // Student should be removed entirely (0 classes)
  REQUIRE(!std::get<0>(test.getStudentData("00000001")));
  // And can no longer be dropped from
  REQUIRE(!test.dropClass("00000001", "COP3504"));
}

TEST_CASE("dropClass — class exists in system but student doesn't have it") {
  CampusCompass test;
  test.insert("Name", "00000001", 1, {"COP3504"});
  REQUIRE(!test.dropClass("00000001", "MAC2311")); // valid class, student doesn't have it
}

TEST_CASE("replaceClass — replacing with a class at a disconnected node") {
  CampusCompass test;
  // Node 26 is disconnected from main graph
  test.addClass("TST1000", 26, "10:00", "11:00");
  test.insert("Name", "00000001", 1, {"COP3504"});
  // This should still succeed — replaceClass doesn't check connectivity
  REQUIRE(test.replaceClass("00000001", "COP3504", "TST1000"));
}

TEST_CASE("removeClass — removes students who end up with 0 classes") {
  CampusCompass test;
  test.insert("Name", "00000001", 1, {"COP3504"});
  test.insert("Name", "00000002", 1, {"COP3504", "MAC2311"});
  REQUIRE(test.removeClass("COP3504") == 2);
  // Student 00000001 had only COP3504 → should be removed
  REQUIRE(!std::get<0>(test.getStudentData("00000001")));
  // Student 00000002 still has MAC2311
  REQUIRE(std::get<0>(test.getStudentData("00000002")));
}

TEST_CASE("WeightedGraph — insert duplicate edge") {
  WeightedGraph graph;
  REQUIRE(graph.insert(1, 2, "A", "B", 5));
  REQUIRE(!graph.insert(1, 2, "A", "B", 10)); // duplicate edge
  REQUIRE(!graph.insert(2, 1, "B", "A", 10)); // reverse duplicate
}

TEST_CASE("WeightedGraph — dijkstras basic") {
  WeightedGraph graph;
  graph.insert(1, 2, "A", "B", 3);
  graph.insert(2, 3, "B", "C", 4);
  graph.insert(1, 3, "A", "C", 10);
  auto result = graph.dijkstras(1);
  REQUIRE(result[2].first == 3);  // dist to 2
  REQUIRE(result[3].first == 7);  // dist to 3 (via 2, not direct)
}

TEST_CASE("WeightedGraph — dijkstras with closed edge") {
  WeightedGraph graph;
  graph.insert(1, 2, "A", "B", 3);
  graph.insert(2, 3, "B", "C", 4);
  graph.insert(1, 3, "A", "C", 10);
  graph.toggleEdgeClosure(1, 2);
  auto result = graph.dijkstras(1);
  REQUIRE(result[3].first == 10); // must take direct route now
}

TEST_CASE("WeightedGraph — getNodeName for valid/invalid nodes") {
  WeightedGraph graph;
  graph.parseEdgesCSV("../data/edges.csv");
  REQUIRE(graph.getNodeName(1) == "Hume Hall");
  REQUIRE(graph.getNodeName(56) == "New Physics Building");
  REQUIRE(graph.getNodeName(0) == "NULL");
  REQUIRE(graph.getNodeName(100) == "NULL");
}

TEST_CASE("WeightedGraph — nodeExists") {
  WeightedGraph graph;
  graph.parseEdgesCSV("../data/edges.csv");
  REQUIRE(graph.nodeExists(1));
  REQUIRE(graph.nodeExists(56));
  REQUIRE(!graph.nodeExists(0));
  REQUIRE(!graph.nodeExists(100));
  REQUIRE(!graph.nodeExists(-1));
}

TEST_CASE("parseCommand — insert with quoted name") {
  CampusCompass test;
  // Basic valid insert via parseCommand
  REQUIRE(test.parseCommand("insert", {"\"Brandon\"", "45679999", "20", "2", "COP3530", "MAC2311"}));
  REQUIRE(std::get<0>(test.getStudentData("45679999")));
}

TEST_CASE("parseCommand — insert with multi-word quoted name") {
  CampusCompass test;
  REQUIRE(test.parseCommand("insert", {"\"Josh Smith\"", "45679999", "20", "2", "COP3530", "MAC2311"}));
}

TEST_CASE("parseCommand — invalid/misspelled command") {
  CampusCompass test;
  REQUIRE(!test.parseCommand("insrt", {"\"Name\"", "12345678", "1", "1", "COP3504"}));
  REQUIRE(!test.parseCommand("Insert", {"\"Name\"", "12345678", "1", "1", "COP3504"}));
  REQUIRE(!test.parseCommand("INSERT", {"\"Name\"", "12345678", "1", "1", "COP3504"}));
  REQUIRE(!test.parseCommand("", {}));
}

TEST_CASE("parseCommand — all command types") {
  CampusCompass test;
  // remove
  REQUIRE(!test.parseCommand("remove", {"99999999"}));
  // dropClass
  REQUIRE(!test.parseCommand("dropClass", {"12345678", "COP3504"}));
  // replaceClass
  REQUIRE(!test.parseCommand("replaceClass", {"12345678", "COP3504", "COP3502"}));
  // removeClass
  test.parseCommand("removeClass", {"COP3504"}); // should return 0 or "unsuccessful"
  // toggleEdgesClosure
  test.parseCommand("toggleEdgesClosure", {"1", "1", "2"});
  // checkEdgeStatus
  test.parseCommand("checkEdgeStatus", {"1", "2"});
  // isConnected
  test.parseCommand("isConnected", {"1", "4"});
  // printShortestEdges
  test.parseCommand("printShortestEdges", {"12345678"});
  // printStudentZone
  test.parseCommand("printStudentZone", {"12345678"});
  // verifySchedule (extra credit)
  test.parseCommand("verifySchedule", {"12345678"});
}

TEST_CASE("Insert — N count must match actual classes") {
  CampusCompass test;
  // The spec says "There must be N classes after reading N"
  // This is a parseCommand-level check — if N=2 but only 1 class follows
  REQUIRE(!test.parseCommand("insert",
      {"\"Name\"", "12345678", "1", "2", "COP3504"})); // N=2 but only 1 class
  REQUIRE(!test.parseCommand("insert",
      {"\"Name\"", "12345678", "1", "1", "COP3504", "MAC2311"})); // N=1 but 2 classes
}

TEST_CASE("Insert — duplicate class codes in same insert") {
  CampusCompass test;
  // Your TODO at line 377-378 mentions this!
  // What happens when insert has the same class twice?
  // Since you use unordered_set, duplicates collapse — but is that correct per spec?
  // If parseCommand passes N=2 COP3504 COP3504, the set has size 1, not matching N
  REQUIRE(!test.parseCommand("insert",
      {"\"Name\"", "12345678", "1", "2", "COP3504", "COP3504"}));
}

TEST_CASE("Insert — home location exists but is not a residence") {
  CampusCompass test;
  // Student with home=14 (Turlington Hall, a classroom building)
  // Spec says RESIDENCE_LOCATION_ID — should this be restricted to residences only?
  // Likely not based on your implementation, but worth confirming
  REQUIRE(test.insert("Name", "12345678", 14, {"COP3504"}));
}

TEST_CASE("Insert — boundary student ID values") {
  CampusCompass test;
  REQUIRE(test.insert("Name", "00000000", 1, {"COP3504"})); // all zeros
  REQUIRE(test.insert("Name", "99999999", 1, {"COP3504"})); // all nines
}

TEST_CASE("Insert — name with only spaces") {
  CampusCompass test;
  // Your code allows empty name, but what about a name of just spaces?
  REQUIRE(test.insert("   ", "12345678", 1, {"COP3504"}));
}

TEST_CASE("toggleEdgesClosure — batch toggle via parseCommand") {
  CampusCompass test;
  // Close 2 edges: 1↔2 and 1↔4
  test.parseCommand("toggleEdgesClosure", {"2", "1", "2", "1", "4"});
  REQUIRE(test.checkEdgeStatus(1, 2).second);  // now closed
  REQUIRE(test.checkEdgeStatus(1, 4).second);  // now closed
}

TEST_CASE("toggleEdgesClosure — toggle same edge twice reopens it") {
  CampusCompass test;
  test.parseCommand("toggleEdgesClosure", {"2", "1", "2", "1", "2"});
  // Toggled twice = back to open
  REQUIRE(!test.checkEdgeStatus(1, 2).second);
}

TEST_CASE("toggleEdgesClosure — spec says always valid edges") {
  // The spec says "You will always be given valid edges in this method"
  // So no validation needed here, but still test the batch count
  CampusCompass test;
  test.parseCommand("toggleEdgesClosure", {"1", "33", "52"});
  REQUIRE(test.checkEdgeStatus(33, 52).second);
}

TEST_CASE("checkEdgeStatus — self-loop edge") {
  CampusCompass test;
  // Checking an edge from a node to itself
  auto result = test.checkEdgeStatus(1, 1);
  REQUIRE(!result.first); // should not exist (DNE)
}

// ==========================================
// parseCommand INSERT — valid cases
// ==========================================

TEST_CASE("parseCommand insert — basic valid single-word name") {
    CampusCompass test;
    REQUIRE(test.parseCommand("insert",
        {"\"Brandon\"", "45679999", "20", "2", "COP3530", "MAC2311"}));
    auto data = test.getStudentData("45679999");
    REQUIRE(std::get<0>(data) == true);
    REQUIRE(std::get<2>(data) == "Brandon"); // name stored without quotes
    REQUIRE(std::get<3>(data) == 20);        // home
    REQUIRE(std::get<4>(data).count("COP3530") == 1);
    REQUIRE(std::get<4>(data).count("MAC2311") == 1);
}

TEST_CASE("parseCommand insert — multi-word quoted name") {
    CampusCompass test;
    REQUIRE(test.parseCommand("insert",
        {"\"Josh Smith\"", "45679999", "20", "2", "COP3530", "MAC2311"}));
    auto data = test.getStudentData("45679999");
    REQUIRE(std::get<2>(data) == "Josh Smith");
}

TEST_CASE("parseCommand insert — name with multiple spaces") {
    CampusCompass test;
    REQUIRE(test.parseCommand("insert",
        {"\"Ana Maria Garcia\"", "11111111", "1", "1", "COP3504"}));
    auto data = test.getStudentData("11111111");
    REQUIRE(std::get<2>(data) == "Ana Maria Garcia");
}

TEST_CASE("parseCommand insert — single character name") {
    CampusCompass test;
    REQUIRE(test.parseCommand("insert",
        {"\"A\"", "22222222", "1", "1", "COP3504"}));
}

TEST_CASE("parseCommand insert — max classes (6)") {
    CampusCompass test;
    REQUIRE(test.parseCommand("insert",
        {"\"Name\"", "33333333", "1", "6",
         "COP3504", "COT3100", "COP3530", "PHY2048", "PHY2049", "IDS2935"}));
    auto data = test.getStudentData("33333333");
    REQUIRE(std::get<4>(data).size() == 6);
}

TEST_CASE("parseCommand insert — min classes (1)") {
    CampusCompass test;
    REQUIRE(test.parseCommand("insert",
        {"\"Name\"", "44444444", "1", "1", "COP3504"}));
}

// ==========================================
// parseCommand INSERT — invalid cases
// ==========================================

TEST_CASE("parseCommand insert — N=0 classes (below minimum)") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "0"}));
}

TEST_CASE("parseCommand insert — N=7 classes (above maximum)") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "7",
         "COP3504", "COT3100", "COP3530", "PHY2048", "PHY2049", "IDS2935", "MAP2302"}));
}

TEST_CASE("parseCommand insert — N count mismatch: fewer classes than N") {
    CampusCompass test;
    // N=3 but only 2 classes follow
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "3", "COP3504", "MAC2311"}));
}

TEST_CASE("parseCommand insert — N count mismatch: more classes than N") {
    CampusCompass test;
    // N=1 but 3 classes follow
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "COP3504", "MAC2311", "COT3100"}));
}

TEST_CASE("parseCommand insert — duplicate class codes in same insert") {
    CampusCompass test;
    // TODO from line 462-463: check for failure with duplicate class
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "2", "COP3504", "COP3504"}));
}

TEST_CASE("parseCommand insert — duplicate class among many") {
    CampusCompass test;
    // Duplicate hidden among valid classes
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "4",
         "COP3504", "MAC2311", "COT3100", "COP3504"}));
}

TEST_CASE("parseCommand insert — duplicate student ID") {
    CampusCompass test;
    REQUIRE(test.parseCommand("insert",
        {"\"Alice\"", "12345678", "1", "1", "COP3504"}));
    REQUIRE(!test.parseCommand("insert",
        {"\"Bob\"", "12345678", "2", "1", "MAC2311"}));
}

TEST_CASE("parseCommand insert — invalid student ID formats") {
    CampusCompass test;
    // Too short
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "1234567", "1", "1", "COP3504"}));
    // Too long
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "123456789", "1", "1", "COP3504"}));
    // Contains letters
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "1234567A", "1", "1", "COP3504"}));
    // Contains special characters
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "1234-678", "1", "1", "COP3504"}));
    // All letters
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "abcdefgh", "1", "1", "COP3504"}));
}

TEST_CASE("parseCommand insert — invalid name formats") {
    CampusCompass test;
    // Name with digits
    REQUIRE(!test.parseCommand("insert",
        {"\"Name123\"", "12345678", "1", "1", "COP3504"}));
    // Name with special characters
    REQUIRE(!test.parseCommand("insert",
        {"\"O'Brien\"", "12345678", "1", "1", "COP3504"}));
    REQUIRE(!test.parseCommand("insert",
        {"\"Name-y\"", "12345678", "1", "1", "COP3504"}));
    REQUIRE(!test.parseCommand("insert",
        {"\"Name@\"", "12345678", "1", "1", "COP3504"}));
}

TEST_CASE("parseCommand insert — invalid class code formats") {
    CampusCompass test;
    // Lowercase letters
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "cop3504"}));
    // Mixed case
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "Cop3504"}));
    // Too short (2 letters + 4 digits = 6 chars)
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "CO3504"}));
    // Too long (4 letters + 4 digits = 8 chars)
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "COPR3504"}));
    // Digits where letters should be
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "1233504"}));
    // Letters where digits should be
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "COPABCD"}));
    // Valid format but class doesn't exist in system
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "ZZZ9999"}));
}

TEST_CASE("parseCommand insert — invalid home location") {
    CampusCompass test;
    // Non-existent node
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "999", "1", "COP3504"}));
    // Negative node
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "-1", "1", "COP3504"}));
    // Zero (not a valid node in the graph)
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "0", "1", "COP3504"}));
}

TEST_CASE("parseCommand insert — non-numeric home location") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "abc", "1", "COP3504"}));
}

TEST_CASE("parseCommand insert — non-numeric N value") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "two", "COP3504"}));
}

TEST_CASE("parseCommand insert — negative N value") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "-1", "COP3504"}));
}

TEST_CASE("parseCommand insert — empty arguments") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("insert", {}));
}

TEST_CASE("parseCommand insert — missing arguments (only name and ID)") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("insert",
        {"\"Name\"", "12345678"}));
}

// ==========================================
// parseCommand REMOVE
// ==========================================

TEST_CASE("parseCommand remove — valid removal") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Brandon\"", "45679999", "1", "2", "COP3504", "MAC2311"});
    REQUIRE(test.parseCommand("remove", {"45679999"}));
    REQUIRE(!std::get<0>(test.getStudentData("45679999")));
}

TEST_CASE("parseCommand remove — non-existent student") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("remove", {"99999999"}));
}

TEST_CASE("parseCommand remove — invalid ID format") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("remove", {"1234"}));      // too short
    REQUIRE(!test.parseCommand("remove", {"abcdefgh"}));  // letters
    REQUIRE(!test.parseCommand("remove", {""}));          // empty
}

TEST_CASE("parseCommand remove — double remove") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "COP3504"});
    REQUIRE(test.parseCommand("remove", {"12345678"}));
    REQUIRE(!test.parseCommand("remove", {"12345678"}));
}

TEST_CASE("parseCommand remove — no arguments") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("remove", {}));
}

// ==========================================
// parseCommand DROPCLASS
// ==========================================

TEST_CASE("parseCommand dropClass — valid drop") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "2", "COP3504", "MAC2311"});
    REQUIRE(test.parseCommand("dropClass", {"12345678", "COP3504"}));
    auto data = test.getStudentData("12345678");
    REQUIRE(std::get<4>(data).count("COP3504") == 0);
    REQUIRE(std::get<4>(data).count("MAC2311") == 1);
}

TEST_CASE("parseCommand dropClass — drop last class removes student") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "COP3504"});
    REQUIRE(test.parseCommand("dropClass", {"12345678", "COP3504"}));
    // Student should be gone
    REQUIRE(!std::get<0>(test.getStudentData("12345678")));
}

TEST_CASE("parseCommand dropClass — student doesn't exist") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("dropClass", {"99999999", "COP3504"}));
}

TEST_CASE("parseCommand dropClass — student doesn't have class") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "COP3504"});
    REQUIRE(!test.parseCommand("dropClass", {"12345678", "MAC2311"}));
}

TEST_CASE("parseCommand dropClass — class doesn't exist in system") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "COP3504"});
    REQUIRE(!test.parseCommand("dropClass", {"12345678", "ZZZ9999"}));
}

TEST_CASE("parseCommand dropClass — invalid class code format") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "COP3504"});
    REQUIRE(!test.parseCommand("dropClass", {"12345678", "invalid"}));
    REQUIRE(!test.parseCommand("dropClass", {"12345678", "cop3504"}));
}

TEST_CASE("parseCommand dropClass — drop same class twice") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "2", "COP3504", "MAC2311"});
    REQUIRE(test.parseCommand("dropClass", {"12345678", "COP3504"}));
    REQUIRE(!test.parseCommand("dropClass", {"12345678", "COP3504"}));
}

TEST_CASE("parseCommand dropClass — no arguments") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("dropClass", {}));
}

TEST_CASE("parseCommand dropClass — only one argument") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("dropClass", {"12345678"}));
}

// ==========================================
// parseCommand REPLACECLASS
// ==========================================

TEST_CASE("parseCommand replaceClass — valid replacement") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "2", "COP3504", "MAC2311"});
    REQUIRE(test.parseCommand("replaceClass",
        {"12345678", "COP3504", "COP3502"}));
    auto data = test.getStudentData("12345678");
    REQUIRE(std::get<4>(data).count("COP3504") == 0);
    REQUIRE(std::get<4>(data).count("COP3502") == 1);
    REQUIRE(std::get<4>(data).count("MAC2311") == 1);
}

TEST_CASE("parseCommand replaceClass — student doesn't exist") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("replaceClass",
        {"99999999", "COP3504", "MAC2311"}));
}

TEST_CASE("parseCommand replaceClass — student doesn't have CLASSCODE_1") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "COP3504"});
    REQUIRE(!test.parseCommand("replaceClass",
        {"12345678", "MAC2311", "COT3100"}));
}

TEST_CASE("parseCommand replaceClass — student already has CLASSCODE_2") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "2", "COP3504", "MAC2311"});
    REQUIRE(!test.parseCommand("replaceClass",
        {"12345678", "COP3504", "MAC2311"}));
    // Verify nothing changed
    auto data = test.getStudentData("12345678");
    REQUIRE(std::get<4>(data).count("COP3504") == 1);
    REQUIRE(std::get<4>(data).count("MAC2311") == 1);
}

TEST_CASE("parseCommand replaceClass — CLASSCODE_2 doesn't exist in system") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "COP3504"});
    REQUIRE(!test.parseCommand("replaceClass",
        {"12345678", "COP3504", "ZZZ9999"}));
    // Verify original class still there
    auto data = test.getStudentData("12345678");
    REQUIRE(std::get<4>(data).count("COP3504") == 1);
}

TEST_CASE("parseCommand replaceClass — replace with same class") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "COP3504"});
    // Replacing COP3504 with COP3504 — already has it
    REQUIRE(!test.parseCommand("replaceClass",
        {"12345678", "COP3504", "COP3504"}));
}

TEST_CASE("parseCommand replaceClass — invalid class code formats") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "COP3504"});
    REQUIRE(!test.parseCommand("replaceClass",
        {"12345678", "COP3504", "bad"}));
    REQUIRE(!test.parseCommand("replaceClass",
        {"12345678", "bad", "COP3502"}));
}

TEST_CASE("parseCommand replaceClass — no arguments / partial arguments") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("replaceClass", {}));
    REQUIRE(!test.parseCommand("replaceClass", {"12345678"}));
    REQUIRE(!test.parseCommand("replaceClass", {"12345678", "COP3504"}));
}

// ==========================================
// parseCommand REMOVECLASS
// ==========================================

TEST_CASE("parseCommand removeClass — valid removal with count") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Alice\"", "11111111", "1", "1", "COP3504"});
    test.parseCommand("insert",
        {"\"Bob\"", "22222222", "1", "2", "COP3504", "MAC2311"});
    test.parseCommand("insert",
        {"\"Carol\"", "33333333", "1", "1", "COP3504"});
    // Should print "3" — 3 students had COP3504
    REQUIRE(test.parseCommand("removeClass", {"COP3504"}));
    // Alice and Carol had ONLY COP3504 → removed
    REQUIRE(!std::get<0>(test.getStudentData("11111111")));
    REQUIRE(!std::get<0>(test.getStudentData("33333333")));
    // Bob still exists with MAC2311
    REQUIRE(std::get<0>(test.getStudentData("22222222")));
    REQUIRE(std::get<4>(test.getStudentData("22222222")).count("COP3504") == 0);
}

TEST_CASE("parseCommand removeClass — class exists but no students enrolled") {
    CampusCompass test;
    // COP3502 exists in the CSV but no students are enrolled
    REQUIRE(!test.parseCommand("removeClass", {"COP3502"}));
}

TEST_CASE("parseCommand removeClass — class doesn't exist") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("removeClass", {"ZZZ9999"}));
}

TEST_CASE("parseCommand removeClass — invalid class code format") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("removeClass", {"invalid"}));
    REQUIRE(!test.parseCommand("removeClass", {"cop3504"}));
    REQUIRE(!test.parseCommand("removeClass", {"1234567"}));
    REQUIRE(!test.parseCommand("removeClass", {""}));
}

TEST_CASE("parseCommand removeClass — double removeClass") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "2", "COP3504", "MAC2311"});
    REQUIRE(test.parseCommand("removeClass", {"COP3504"}));
    // Second time: no students have it anymore
    REQUIRE(!test.parseCommand("removeClass", {"COP3504"}));
}

TEST_CASE("parseCommand removeClass — no arguments") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("removeClass", {}));
}

// ==========================================
// parseCommand TOGGLEEDGESCLOSURE (batch)
// ==========================================

TEST_CASE("parseCommand toggleEdgesClosure — single edge") {
    CampusCompass test;
    test.parseCommand("toggleEdgesClosure", {"1", "1", "2"});
    REQUIRE(test.checkEdgeStatus(1, 2).second);  // closed
    REQUIRE(test.checkEdgeStatus(2, 1).second);  // symmetric
}

TEST_CASE("parseCommand toggleEdgesClosure — multiple edges") {
    CampusCompass test;
    // Close 3 edges: 1↔2, 1↔4, 33↔52
    test.parseCommand("toggleEdgesClosure",
        {"3", "1", "2", "1", "4", "33", "52"});
    REQUIRE(test.checkEdgeStatus(1, 2).second);
    REQUIRE(test.checkEdgeStatus(1, 4).second);
    REQUIRE(test.checkEdgeStatus(33, 52).second);
}

TEST_CASE("parseCommand toggleEdgesClosure — reopen already closed edge") {
    CampusCompass test;
    // Close it
    test.parseCommand("toggleEdgesClosure", {"1", "1", "2"});
    REQUIRE(test.checkEdgeStatus(1, 2).second);
    // Toggle again → open
    test.parseCommand("toggleEdgesClosure", {"1", "1", "2"});
    REQUIRE(!test.checkEdgeStatus(1, 2).second);
}

TEST_CASE("parseCommand toggleEdgesClosure — same edge toggled twice in one batch") {
    CampusCompass test;
    // N=2 but both edges are 1↔2 → net effect = no change
    test.parseCommand("toggleEdgesClosure",
        {"2", "1", "2", "1", "2"});
    REQUIRE(!test.checkEdgeStatus(1, 2).second); // toggled twice = back to open
}

TEST_CASE("parseCommand toggleEdgesClosure — close and verify connectivity impact") {
    CampusCompass test;
    REQUIRE(test.isConnected(33, 52));
    test.parseCommand("toggleEdgesClosure", {"1", "33", "52"});
    // 33 and 52 are only connected by a single edge
    REQUIRE(!test.isConnected(33, 52));
    // Reopen
    test.parseCommand("toggleEdgesClosure", {"1", "33", "52"});
    REQUIRE(test.isConnected(33, 52));
}

TEST_CASE("parseCommand toggleEdgesClosure — close all edges to isolate a node") {
    CampusCompass test;
    // Node 43 connects to 7 and 6
    test.parseCommand("toggleEdgesClosure",
        {"2", "43", "7", "43", "6"});
    REQUIRE(!test.isConnected(43, 1));
    REQUIRE(!test.isConnected(43, 7));
    REQUIRE(!test.isConnected(43, 6));
}

TEST_CASE("parseCommand toggleEdgesClosure — N=0 is a no-op") {
    CampusCompass test;
    // Spec doesn't explicitly forbid this — should just do nothing
    test.parseCommand("toggleEdgesClosure", {"0"});
    // All edges should remain open
    REQUIRE(!test.checkEdgeStatus(1, 2).second);
}

// ==========================================
// parseCommand CHECKEDGESTATUS
// ==========================================

TEST_CASE("parseCommand checkEdgeStatus — open edge") {
    CampusCompass test;
    // Edge 1↔2 exists and is open by default
    // Should print "open"
    REQUIRE(test.parseCommand("checkEdgeStatus", {"1", "2"}));
}

TEST_CASE("parseCommand checkEdgeStatus — closed edge") {
    CampusCompass test;
    test.toggleEdgeClosure(1, 2);
    // Should print "closed"
    REQUIRE(test.parseCommand("checkEdgeStatus", {"1", "2"}));
}

TEST_CASE("parseCommand checkEdgeStatus — edge does not exist (DNE)") {
    CampusCompass test;
    // Edge 1↔6 doesn't exist in edges.csv
    // Should print "DNE"
    REQUIRE(test.parseCommand("checkEdgeStatus", {"1", "6"}));
    // NOTE: depending on your parseCommand return value convention,
    // DNE might return false. Adjust accordingly.
}

TEST_CASE("parseCommand checkEdgeStatus — symmetry") {
    CampusCompass test;
    // checkEdgeStatus 1 2 and checkEdgeStatus 2 1 should give same result
    test.parseCommand("checkEdgeStatus", {"1", "2"});
    test.parseCommand("checkEdgeStatus", {"2", "1"});
    REQUIRE(test.checkEdgeStatus(1, 2) == test.checkEdgeStatus(2, 1));
}

TEST_CASE("parseCommand checkEdgeStatus — invalid node IDs") {
    CampusCompass test;
    // Non-existent nodes → DNE
    test.parseCommand("checkEdgeStatus", {"-1", "2"});
    test.parseCommand("checkEdgeStatus", {"999", "1000"});
}

TEST_CASE("parseCommand checkEdgeStatus — self-loop") {
    CampusCompass test;
    // Node to itself → DNE
    test.parseCommand("checkEdgeStatus", {"1", "1"});
    auto result = test.checkEdgeStatus(1, 1);
    REQUIRE(!result.first); // should not exist
}

TEST_CASE("parseCommand checkEdgeStatus — no arguments") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("checkEdgeStatus", {}));
}

TEST_CASE("parseCommand checkEdgeStatus — non-numeric arguments") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("checkEdgeStatus", {"abc", "def"}));
}

// ==========================================
// parseCommand ISCONNECTED
// ==========================================

TEST_CASE("parseCommand isConnected — connected nodes") {
    CampusCompass test;
    REQUIRE(test.parseCommand("isConnected", {"1", "4"}));
}

TEST_CASE("parseCommand isConnected — disconnected nodes") {
    CampusCompass test;
    // Nodes 26 and 1 are in separate components
    REQUIRE(!test.parseCommand("isConnected", {"26", "1"}));
}

TEST_CASE("parseCommand isConnected — same node") {
    CampusCompass test;
    REQUIRE(test.parseCommand("isConnected", {"1", "1"}));
}

TEST_CASE("parseCommand isConnected — after closing critical edge") {
    CampusCompass test;
    // Node 33↔52 is a single-edge pair
    REQUIRE(test.parseCommand("isConnected", {"33", "52"}));
    test.parseCommand("toggleEdgesClosure", {"1", "33", "52"});
    REQUIRE(!test.parseCommand("isConnected", {"33", "52"}));
}

TEST_CASE("parseCommand isConnected — non-existent nodes") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("isConnected", {"-1", "1"}));
    REQUIRE(!test.parseCommand("isConnected", {"1", "999"}));
}

TEST_CASE("parseCommand isConnected — no arguments") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("isConnected", {}));
}

// ==========================================
// parseCommand PRINTSHORESTEDGES
// ==========================================

TEST_CASE("parseCommand printShortestEdges — valid student") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Manav\"", "45679999", "1", "2", "COP3504", "MAC2311"});
    REQUIRE(test.parseCommand("printShortestEdges", {"45679999"}));
}

TEST_CASE("parseCommand printShortestEdges — non-existent student") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("printShortestEdges", {"99999999"}));
}

TEST_CASE("parseCommand printShortestEdges — invalid student ID format") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("printShortestEdges", {"invalid"}));
    REQUIRE(!test.parseCommand("printShortestEdges", {"123"}));
}

TEST_CASE("parseCommand printShortestEdges — after edge closure makes class unreachable") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "PHY2048"});
    // PHY2048 is at node 56 — reachable via 49↔56
    REQUIRE(test.parseCommand("printShortestEdges", {"12345678"}));
    // Close the only path to 56
    test.parseCommand("toggleEdgesClosure", {"1", "49", "56"});
    // Should now show -1 for PHY2048
    REQUIRE(test.parseCommand("printShortestEdges", {"12345678"}));
}

TEST_CASE("parseCommand printShortestEdges — student at class location (distance 0)") {
    CampusCompass test;
    // Home = 14, COP3504 is at 14
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "14", "1", "COP3504"});
    REQUIRE(test.parseCommand("printShortestEdges", {"12345678"}));
    auto result = test.printShortestEdges("12345678");
    REQUIRE(result[0] == "COP3504: 0");
}

TEST_CASE("parseCommand printShortestEdges — no arguments") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("printShortestEdges", {}));
}

// ==========================================
// parseCommand PRINTSTUDENTZONE
// ==========================================

TEST_CASE("parseCommand printStudentZone — valid student") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "2", "COP3504", "MAC2311"});
    REQUIRE(test.parseCommand("printStudentZone", {"12345678"}));
}

TEST_CASE("parseCommand printStudentZone — non-existent student") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("printStudentZone", {"99999999"}));
}

TEST_CASE("parseCommand printStudentZone — student with home at class location") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "14", "1", "COP3504"});
    REQUIRE(test.parseCommand("printStudentZone", {"12345678"}));
    REQUIRE(test.printStudentZone("12345678") == 0);
}

TEST_CASE("parseCommand printStudentZone — no arguments") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("printStudentZone", {}));
}

// ==========================================
// parseCommand VERIFYSCHEDULE (Extra Credit)
// ==========================================

TEST_CASE("parseCommand verifySchedule — valid student with feasible schedule") {
    CampusCompass test;
    // COP3530 at 14, 10:40-11:30 → COP3504 at 14, 11:45-12:35
    // Same building, 15 min gap, 0 travel time → successful
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "2", "COP3530", "COP3504"});
    REQUIRE(test.parseCommand("verifySchedule", {"12345678"}));
}

TEST_CASE("parseCommand verifySchedule — non-existent student") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("verifySchedule", {"99999999"}));
}

TEST_CASE("parseCommand verifySchedule — student with 1 class") {
    CampusCompass test;
    test.parseCommand("insert",
        {"\"Name\"", "12345678", "1", "1", "COP3504"});
    // Spec says 1 class → print "unsuccessful"
    REQUIRE(!test.parseCommand("verifySchedule", {"12345678"}));
}

TEST_CASE("parseCommand verifySchedule — no arguments") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("verifySchedule", {}));
}

// ==========================================
// INVALID / MISSPELLED COMMANDS
// ==========================================

TEST_CASE("parseCommand — misspelled commands") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("insrt", {"\"N\"", "12345678", "1", "1", "COP3504"}));
    REQUIRE(!test.parseCommand("reomve", {"12345678"}));
    REQUIRE(!test.parseCommand("drapClass", {"12345678", "COP3504"}));
    REQUIRE(!test.parseCommand("replcaeClass", {"12345678", "COP3504", "COP3502"}));
    REQUIRE(!test.parseCommand("removeClas", {"COP3504"}));
    REQUIRE(!test.parseCommand("toogleEdgesClosure", {"1", "1", "2"}));
    REQUIRE(!test.parseCommand("checkEdgStatus", {"1", "2"}));
    REQUIRE(!test.parseCommand("isConnectd", {"1", "2"}));
    REQUIRE(!test.parseCommand("printShortEdges", {"12345678"}));
    REQUIRE(!test.parseCommand("printStudntZone", {"12345678"}));
    REQUIRE(!test.parseCommand("verifyScehdule", {"12345678"}));
}

TEST_CASE("parseCommand — case sensitivity") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("Insert", {"\"N\"", "12345678", "1", "1", "COP3504"}));
    REQUIRE(!test.parseCommand("INSERT", {"\"N\"", "12345678", "1", "1", "COP3504"}));
    REQUIRE(!test.parseCommand("REMOVE", {"12345678"}));
    REQUIRE(!test.parseCommand("Remove", {"12345678"}));
    REQUIRE(!test.parseCommand("DropClass", {"12345678", "COP3504"}));
    REQUIRE(!test.parseCommand("DROPCLASS", {"12345678", "COP3504"}));
    REQUIRE(!test.parseCommand("Removeclass", {"COP3504"}));
    REQUIRE(!test.parseCommand("ISCONNECTED", {"1", "2"}));
}

TEST_CASE("parseCommand — completely invalid commands") {
    CampusCompass test;
    REQUIRE(!test.parseCommand("", {}));
    REQUIRE(!test.parseCommand("help", {}));
    REQUIRE(!test.parseCommand("exit", {}));
    REQUIRE(!test.parseCommand("delete", {"12345678"}));
    REQUIRE(!test.parseCommand("add", {"\"Name\"", "12345678", "1", "1", "COP3504"}));
    REQUIRE(!test.parseCommand("123", {"456"}));
}

// ==========================================
// FULL INTEGRATION: multi-command sequences via parseCommand
// ==========================================

TEST_CASE("parseCommand — full workflow: insert, modify, query, remove") {
    CampusCompass test;

    // Insert two students
    REQUIRE(test.parseCommand("insert",
        {"\"Brandon\"", "45679999", "20", "2", "COP3530", "MAC2311"}));
    REQUIRE(test.parseCommand("insert",
        {"\"Brian\"", "35459999", "21", "3", "COP3530", "CDA3101", "MAC2311"}));

    // Replace a class for Brian
    REQUIRE(test.parseCommand("replaceClass",
        {"35459999", "COP3530", "COP3504"}));
    auto data = test.getStudentData("35459999");
    REQUIRE(std::get<4>(data).count("COP3530") == 0);
    REQUIRE(std::get<4>(data).count("COP3504") == 1);

    // Remove COP3530 from all students (only Brandon has it now)
    REQUIRE(test.parseCommand("removeClass", {"COP3530"}));
    // Brandon had 2 classes, now 1 (MAC2311)
    REQUIRE(std::get<0>(test.getStudentData("45679999")));
    REQUIRE(std::get<4>(test.getStudentData("45679999")).size() == 1);

    // Query shortest edges for Brian
    REQUIRE(test.parseCommand("printShortestEdges", {"35459999"}));

    // Drop Brian's last two classes one by one
    REQUIRE(test.parseCommand("dropClass", {"35459999", "CDA3101"}));
    REQUIRE(test.parseCommand("dropClass", {"35459999", "COP3504"}));
    // Brian had MAC2311 left
    REQUIRE(test.parseCommand("dropClass", {"35459999", "MAC2311"}));
    // Brian should be removed (0 classes)
    REQUIRE(!std::get<0>(test.getStudentData("35459999")));

    // Remove Brandon
    REQUIRE(test.parseCommand("remove", {"45679999"}));
    REQUIRE(!std::get<0>(test.getStudentData("45679999")));
}

TEST_CASE("parseCommand — sample input/output from spec") {
    CampusCompass test;

    // Replicate the spec's sample I/O:
    // insert "Brandon" 45679999 20 2 COP3530 MAC2311
    REQUIRE(test.parseCommand("insert",
        {"\"Brandon\"", "45679999", "20", "2", "COP3530", "MAC2311"}));

    // insert "Brian" 35459999 21 3 COP3530 CDA3101 MAC2311
    REQUIRE(test.parseCommand("insert",
        {"\"Brian\"", "35459999", "21", "3", "COP3530", "CDA3101", "MAC2311"}));

    // insert "Briana" 87879999 22 3 CDA3101 MAC2311 EEL3701
    REQUIRE(test.parseCommand("insert",
        {"\"Briana\"", "87879999", "22", "3", "CDA3101", "MAC2311", "EEL3701"}));

    // removeClass COP3530
    REQUIRE(test.parseCommand("removeClass", {"COP3530"}));

    // remove 87879999
    REQUIRE(test.parseCommand("remove", {"87879999"}));

    // removeClass CDA3101
    REQUIRE(test.parseCommand("removeClass", {"CDA3101"}));

    // printShortestEdges 35459999
    REQUIRE(test.parseCommand("printShortestEdges", {"35459999"}));
}