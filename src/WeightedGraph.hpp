#pragma once
#include <vector>
#include <string>
#include <unordered_map>


class WeightedGraph {
public:
    WeightedGraph();

    bool toggleEdgeClosure(int from, int to);
    std::pair<bool,bool> checkEdgeStatus(int from, int to);
    // <locationID, <distance, prevLocationID>>
    std::unordered_map<int, std::pair<int, int>> dijkstras(int from);

    bool isConnected(int from, int to);

    bool insert(int from, int to, std::string fromValue, std::string toValue, int weight);

    // returns if the edge is closed and what the weight is
    std::pair<bool, int> checkEdge(int from, int to);

    bool parseCSV(const std::string &edges_filepath, const std::string &classes_filepath);

    //TODO:: have a function that makes the subgraph and finds a mst... i think have an mst function that takes in some sort of datastructure of node pointers.


private:
    // < Node1<id,value>, Node2<id,value>, weight >
    std::vector<std::tuple<std::pair<int, std::string>, std::pair<int,std::string>, int>> edgeList;

    struct Node {
        int id;
        std::string value;

        // map of <id, <isClosed, weight, Node>> TODO:: Consider if using an ordered map / priority queue is better
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
};
