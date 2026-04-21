#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>


class WeightedGraph {
public:
    WeightedGraph() = default;

    WeightedGraph(const WeightedGraph& other);
    WeightedGraph(WeightedGraph&& other) noexcept ;
    WeightedGraph& operator=(const WeightedGraph& other);
    WeightedGraph& operator=(WeightedGraph&& other) noexcept ;
    ~WeightedGraph();

    bool toggleEdgeClosure(const int from, const int to);
    // <exists, isClosed>
    std::pair<bool,bool> checkEdgeStatus(int from, int to);
    // <locationID, <distance, prevLocationID>>
    std::unordered_map<int, std::pair<int, int>> dijkstras(int from);
    int mstOfNodes(const std::unordered_set<int>& subNodes);

    bool isConnected(int from, int to);

    bool insert(int from, int to, const std::string& fromValue, const std::string& toValue, int weight);

    // < isClosed, weight>
    std::pair<bool, int> checkEdge(int from, int to);

    std::string getNodeName(int id);

    bool nodeExists(int id);

    bool parseEdgesCSV(const std::string& edgesFilePath);

private:
    // < Node1<id,value>, Node2<id,value>, weight >
    // std::vector<std::tuple<std::pair<int, std::string>, std::pair<int,std::string>, int>> edgeList;

    struct Node {
        int id;
        std::string value;

        // Ownership of the nodes is done by graph, there is no need for a destructor here.
        // I don't think it would make it any cleaner here.

        // map of <id, <isClosed, weight, Node>>
        std::unordered_map<int, std::tuple<bool, int, Node*>> nextNodes;

        Node(const int id, const std::string& value) {
            this->id = id;
            this->value = value;
        }
        bool addNextNode(Node* next, int weight) {
            if (nextNodes.find(next->id) != nextNodes.end() || next->nextNodes.find(id) != next->nextNodes.end())
                return false;
            nextNodes[next->id] = {false, weight, next};
            next->nextNodes[id] = {false, weight, this};
            return true;
        }

    };

    // map of all nodes with ID as key.
    std::unordered_map<int, Node*> nodes;

    static int kruskals(std::vector<std::pair<int,std::pair<int,int>>> edgeList);

    void clearNodes();
};
