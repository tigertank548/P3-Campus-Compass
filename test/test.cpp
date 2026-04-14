#include <catch2/catch_test_macros.hpp>
#include <iostream>

// change if you choose to use a different header name
#include "CampusCompass.h"
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

    REQUIRE(!test.insert("Namey", "00000001", 1, {"COP3504", "MAC2311C"}));
    REQUIRE(!test.insert("Namey", "00000002", 1, {"COP35a4", "MAC2311"}));
    REQUIRE(!test.insert("Namey", "00000003", 1, {"COP3504", "MAC23115"}));
    REQUIRE(!test.insert("Namey", "00000004", 1, {}));
    REQUIRE(!test.insert("Namey", "00000005", 1, {"COP3504","COT3100", "COP3530", "PHY2048","PHY2049","IDS3935"}));
    REQUIRE(!test.insert("Namey", "00000006", 1, {"COP3504","COT3100", "COP3530", "PHY2048","PHY2049","IDS3935","MAP2302"}));
    REQUIRE(!test.insert("Namey", "00000007", 1, {"COP3504","COP3504"}));

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

TEST_CASE("Testing remove") {
    CampusCompass test;

    REQUIRE(test.insert("Namey", "12345678", 1, {"COP3504", "MAC2311"}));
    REQUIRE(test.remove("12345678"));
    REQUIRE(test.insert("Namey", "12345678", 1, {"COP3504", "MAC2311"}));


}

