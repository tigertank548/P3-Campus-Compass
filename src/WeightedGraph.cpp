#include "WeightedGraph.hpp"

#include <fstream>
#include <sstream>

WeightedGraph::WeightedGraph() {
}

std::pair<bool, int> WeightedGraph::checkEdge(int from, int to) {
    if (nodes.find(from) == nodes.end() || nodes.find(to) == nodes.end())
        return {true, -1};
    if (nodes[from]->nextNodes.find(to) == nodes[from]->nextNodes.end())
        return {true, -1};
    return {std::get<0>(nodes[from]->nextNodes[to]), std::get<1>(nodes[from]->nextNodes[to])};
}


bool WeightedGraph::toggleEdgeClosure(int from, int to) {
    if (nodes.find(from) == nodes.end() || nodes.find(to) == nodes.end())
        return false;
    std::get<0>(nodes[from]->nextNodes[to]) = !std::get<0>(nodes[from]->nextNodes[to]);
    std::get<0>(nodes[to]->nextNodes[from]) = !std::get<0>(nodes[to]->nextNodes[from]);
    return true;
}


bool WeightedGraph::insert(int from, int to, std::string fromValue, std::string toValue, int weight) {
    if (nodes.find(from) == nodes.end())
        nodes[from] = new Node(from, fromValue);
    if (nodes.find(to) == nodes.end())
        nodes[to] = new Node(to, toValue);
    const bool out = nodes[from]->addNextNode(nodes[to],weight);
    if (out)
        edgeList.push_back({{from,fromValue},{to, toValue}, weight});
    return out;
}


bool WeightedGraph::parseCSV(const std::string &edges_filepath, const std::string &classes_filepath) {
    std::ifstream input(edges_filepath);

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

bool WeightedGraph::nodeExists(int id) {
    return nodes.find(id) != nodes.end();
}

