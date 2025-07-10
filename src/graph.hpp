#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>

class Graph
{
    private:
        std::unordered_map<std::string, std::unordered_set<std::string>> adjacencyMatrix;

    public:
        Graph(std::unordered_map<std::string, std::unordered_set<std::string>> adjacencyMatrix);

        bool operator==(const Graph& other) const;

        std::unordered_set<std::string> getNeighbors(std::string vertex) const;
        std::unordered_set<std::string> getCommunity(std::string vertex) const;

        std::unordered_set<std::string> getVertices() const;
        std::vector<std::unordered_set<std::string>> getCommunities() const;

        std::optional<std::vector<std::string>> getPath(std::string vertex1, std::string vertex2) const;\

        std::string toString() const;
        std::string toDOT() const;

        void exportGraph(std::string outputDirPath, std::string outputFileName) const;

        static Graph fromFile(std::string filePath);
};

std::string vecToStr(std::vector<std::string> vec);
std::string setToStr(std::unordered_set<std::string> set);

#endif