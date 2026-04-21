#include "WeightedGraph.hpp"

#include <algorithm>
#include <fstream>
#include <queue>
#include <sstream>
#include <stack>



std::pair<bool, int> WeightedGraph::checkEdge(const int from, const int to) {
    if (nodes.find(from) == nodes.end() || nodes.find(to) == nodes.end())
        return {true, -1};
    if (nodes[from]->nextNodes.find(to) == nodes[from]->nextNodes.end())
        return {true, -1};
    return {std::get<0>(nodes[from]->nextNodes[to]), std::get<1>(nodes[from]->nextNodes[to])};
}

std::string WeightedGraph::getNodeName(const int id) {
    if (nodes.find(id) == nodes.end())
        return "NULL";
    return nodes[id]->value;
}


WeightedGraph::WeightedGraph(const WeightedGraph& other) {
    // go through every edge and insert a copy... it's slow and bad, but I don't know if there's a better way
    // maybe with that edge list I have commented out, but I already got rid of it, it's gone now
    for (const std::pair<int, Node*> node : other.nodes)
        for (const std::pair<int, std::tuple<bool, int, Node*>> edge : node.second->nextNodes)
            insert(node.second->id, edge.first, node.second->value, std::get<2>(edge.second)->value, std::get<1>(edge.second));
}
WeightedGraph::WeightedGraph(WeightedGraph&& other) noexcept {
    nodes = std::move(other.nodes);
}
WeightedGraph& WeightedGraph::operator=(const WeightedGraph& other) {
    if (this == &other)
        return *this;

    clearNodes();

    for (const std::pair<int, Node*> node : other.nodes)
        for (const std::pair<int, std::tuple<bool, int, Node*>> edge : node.second->nextNodes)
            insert(node.second->id, edge.first, node.second->value, std::get<2>(edge.second)->value, std::get<1>(edge.second));
    return *this;
}
WeightedGraph& WeightedGraph::operator=(WeightedGraph&& other) noexcept {
    if (this == &other)
        return *this;

    clearNodes();

    nodes = std::move(other.nodes);
    return *this;
}
WeightedGraph::~WeightedGraph() {
    clearNodes();
}

bool WeightedGraph::toggleEdgeClosure(const int from, const int to) {
    if (nodes.find(from) == nodes.end() || nodes.find(to) == nodes.end())
        return false;
    if (nodes[from]->nextNodes.find(to) == nodes[from]->nextNodes.end() || nodes[to]->nextNodes.find(from) == nodes[to]->nextNodes.end())
        return false;
    std::get<0>(nodes[from]->nextNodes[to]) = !std::get<0>(nodes[from]->nextNodes[to]);
    std::get<0>(nodes[to]->nextNodes[from]) = !std::get<0>(nodes[to]->nextNodes[from]);
    return true;
}

std::pair<bool, bool> WeightedGraph::checkEdgeStatus(const int from, const int to) {
    if (nodes.find(from) == nodes.end() || nodes.find(to) == nodes.end())
        return {false,false};
    if (nodes[from]->nextNodes.find(to) == nodes[from]->nextNodes.end())
        return {false,false};
    return {true,std::get<0>(nodes[from]->nextNodes[to])};
}

// < nodeID, < dist, last >>
std::unordered_map<int, std::pair<int, int>> WeightedGraph::dijkstras(int from) {
    std::unordered_map<int, std::pair<int,int>> out;
    std::unordered_set<int> visited;
    // < dist, nodeID >
    std::priority_queue<
        std::pair<int, int>,
        std::vector<std::pair<int, int>>,
        std::greater<std::pair<int, int>>
    > toVisit;

    out[from] = {0, -1};
    toVisit.emplace(0, from);

    while (!toVisit.empty()) {
        auto [dist, nodeID] = toVisit.top();
        toVisit.pop();

        if (visited.count(nodeID)) continue;
        visited.insert(nodeID);

        for (std::pair edge: nodes[nodeID]->nextNodes) {
            if (std::get<0>(edge.second)) // if the edge is closed don't go to it
                continue;

            int neighborID = edge.first;
            int newDist = dist + std::get<1>(edge.second);

            if (visited.find(neighborID) == visited.end()) {
                if (out.find(neighborID) == out.end() || out[neighborID].first > newDist) {
                    out[neighborID] = {newDist, nodeID};
                    toVisit.emplace(newDist, neighborID);
                }
            }
        }
    }
    return out;
}


int WeightedGraph::mstOfNodes(const std::unordered_set<int>& subNodes) {
    // go through the nodes and add all nodes in the shortest paths
    // just pick the first as the origin
    // once we have a set of all the nodes required
    // find all the edges that connect those nodes
    // then apply kruskals with disjoint set
    // just define disjoint set in a kruskals helper that takes in an edge list

    // O(ElogV)
    std::unordered_map<int, std::pair<int,int>> result = dijkstras(*subNodes.begin());
    std::unordered_set<int> allSubNodes;
    // O(V)
    for (int node : subNodes) {
        while (node != -1 && allSubNodes.find(node) == allSubNodes.end()) {
            allSubNodes.insert(node);
            node = result[node].second;
        }
    }

    // < weight, < node1, node2 > >
    std::vector<std::pair<int, std::pair<int,int>>> edgeList;
    std::unordered_set<int> visited;

    // O(E)
    for (int node : allSubNodes) {
        std::unordered_map<int, std::tuple<bool, int, Node*>> nexts = nodes[node]->nextNodes;
        for (std::pair<int, std::tuple<bool, int, Node*>> next : nexts) {
            if (std::get<0>(next.second))
                continue;
            // testing if the edge we are looking at is going to a required node, and testing if that node has been visited before
            // we cant add edges to nodes that are already visited or else we will have duplicate edges in the list
            if (allSubNodes.find(next.first) != allSubNodes.end() && visited.find(next.first) == visited.end())
                edgeList.push_back({std::get<1>(next.second), {node, next.first}});
        }
        visited.insert(node);
    }

    return kruskals(edgeList);
}


bool WeightedGraph::isConnected(int from, int to) {
    if (nodes.find(from) == nodes.end() || nodes.find(to) == nodes.end())
        return false;
    if (from == to)
        return true;


    std::unordered_set<int> visited;
    std::stack<int> stack;
    stack.push(from);
    while (!stack.empty()) {
        const int current = stack.top();
        stack.pop();
        visited.insert(current);
        for (const std::pair<int, std::tuple<bool, int, Node*>> next : nodes[current]->nextNodes) {
            int num = std::get<2>(next.second)->id;
            if (visited.find(num) != visited.end())
                continue;
            if (std::get<0>(next.second))
                continue;
            if (num == to)
                return true;
            stack.push(num);
        }
        visited.insert(current);
    }
    return false;
}


bool WeightedGraph::insert(int from, int to, const std::string& fromValue, const std::string& toValue, int weight) {
    if (nodes.find(from) == nodes.end())
        nodes[from] = new Node(from, fromValue);
    if (nodes.find(to) == nodes.end())
        nodes[to] = new Node(to, toValue);
    const bool out = nodes[from]->addNextNode(nodes[to],weight);
    // if (out)
    //     edgeList.push_back({{from,fromValue},{to, toValue}, weight});
    return out;
}


bool WeightedGraph::parseEdgesCSV(const std::string& edgesFilePath) {
    std::ifstream input(edgesFilePath);

    std::string line;
    std::getline(input, line);//discards headers
    while (std::getline(input, line)) {
        std::istringstream lineStream(line);


        std::string from, to, fromName, toName, weight;


        std::getline(lineStream, from, ',');
        std::getline(lineStream, to, ',');
        std::getline(lineStream, fromName, ',');
        std::getline(lineStream, toName, ',');
        std::getline(lineStream, weight, ',');

        try {
            if (!insert(std::stoi(from), std::stoi(to), fromName, toName, std::stoi(weight)))
                return false;
        }catch (const std::invalid_argument& e) {
            return false;
        }
    }

    return true;
}

int WeightedGraph::kruskals(std::vector<std::pair<int, std::pair<int, int>>> edgeList) {
    std::sort(edgeList.begin(),edgeList.end());

    struct DisjointSet {
        // < number, < parent, numChildren>>
        std::unordered_map<int, std::pair<int,int>> parents;

        void insert(int toInsert) {
            if (parents.find(toInsert) == parents.end())
                parents[toInsert] = {toInsert, 1};
        }

        int find(int toFind) {
            if (parents[toFind].first == toFind)
                return toFind;
            return parents[toFind].first = find(parents[toFind].first);
        }

        void unionize(const int num1, const int num2){
            const int num1Parent = find(num1);
            const int num2Parent = find(num2);
            if (num1Parent == num2Parent)
                return;
            const int bigger = parents[num1Parent].second >= parents[num2Parent].second ? num1Parent : num2Parent;
            const int smaller = parents[num1Parent].second < parents[num2Parent].second ? num1Parent : num2Parent;
            parents[smaller].first = bigger;
            parents[bigger].second += parents[smaller].second;
        }

        bool inSameSet(const int num1, const int num2) {
            return find(num1) == find(num2);
        }
    };

    DisjointSet disjointSet;

    for (std::pair pair : edgeList) {
        disjointSet.insert(pair.second.first);
        disjointSet.insert(pair.second.second);
    }

    int total = 0;
    for (std::pair pair : edgeList) {
        if (!disjointSet.inSameSet(pair.second.first, pair.second.second)) {
            disjointSet.unionize(pair.second.first, pair.second.second);
            total += pair.first;
        }
    }

    return total;
}

void WeightedGraph::clearNodes() {
    for (const std::pair<int, Node*> node : nodes)
        delete node.second;
    nodes.clear();// need to make sure they don't get accessed
}

bool WeightedGraph::nodeExists(int id) {
    return nodes.find(id) != nodes.end();
}
