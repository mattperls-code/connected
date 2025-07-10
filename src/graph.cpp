#include "graph.hpp"

#include <stdexcept>
#include <queue>
#include <fstream>
#include <filesystem>
#include <cstdlib>

#include "../lib/parsing/parser.hpp"

Graph::Graph(std::unordered_map<std::string, std::unordered_set<std::string>> adjacencyMatrix)
{
    for (auto [vertex, neighbors] : adjacencyMatrix) {
        for (auto neighbor : neighbors) if (!adjacencyMatrix[neighbor].contains(vertex)) throw std::runtime_error("Graph constructor: adjacencyMatrix should be symmetric");
    }

    this->adjacencyMatrix = adjacencyMatrix;
};

bool Graph::operator==(const Graph& other) const
{
    return this->adjacencyMatrix == other.adjacencyMatrix;
};

std::unordered_set<std::string> Graph::getNeighbors(std::string vertex) const
{
    if (!this->adjacencyMatrix.contains(vertex)) throw std::runtime_error("Graph getNeighbors: vertex is not in the graph");

    return this->adjacencyMatrix.at(vertex);
};

std::unordered_set<std::string> Graph::getCommunity(std::string vertex) const
{
    std::queue<std::string> queue;
    queue.push(vertex);

    std::unordered_set<std::string> visited;

    while (!queue.empty()) {
        auto currentVertex = queue.front();
        queue.pop();

        if (visited.contains(currentVertex)) continue;
        visited.insert(currentVertex);
        
        for (auto neighbor : this->getNeighbors(currentVertex)) queue.push(neighbor);
    }

    return visited;
};

std::unordered_set<std::string> Graph::getVertices() const
{
    std::unordered_set<std::string> vertices;

    for (auto [vertex, _] : this->adjacencyMatrix) vertices.insert(vertex);

    return vertices;
};

std::vector<std::unordered_set<std::string>> Graph::getCommunities() const
{
    std::vector<std::unordered_set<std::string>> communities;

    auto unvisited = this->getVertices();

    while (!unvisited.empty()) {
        auto root = *unvisited.begin();

        auto community = this->getCommunity(root);

        communities.push_back(community);
        
        for (auto vertex : community) unvisited.erase(vertex);
    }

    return communities;
};

std::optional<std::vector<std::string>> Graph::getPath(std::string vertex1, std::string vertex2) const
{
    std::queue<std::string> queue;
    queue.push(vertex1);

    std::unordered_map<std::string, std::string> parentMatrix;

    while (!queue.empty()) {
        auto currentVertex = queue.front();
        queue.pop();

        for (auto neighbor : this->getNeighbors(currentVertex)) if (!parentMatrix.contains(neighbor)) {
            queue.push(neighbor);

            parentMatrix[neighbor] = currentVertex;
        }

        if (parentMatrix.contains(vertex2)) {
            std::vector<std::string> path = { vertex2 };

            while (path.back() != vertex1) path.push_back(parentMatrix[path.back()]);
            
            std::reverse(path.begin(), path.end());

            return path;
        }
    }

    return std::nullopt;
};

std::string Graph::toString() const
{
    std::string output;

    for (auto [vertex, neighbors] : this->adjacencyMatrix) {
        output += "\"" + vertex + "\": { ";

        for (auto neighbor : neighbors) output += "\"" + neighbor + "\", ";

        output.pop_back();
        if (!neighbors.empty()) output.pop_back();
        
        output += " }\n";
    }

    return output;
};

std::string Graph::toDOT() const
{
    std::string output;
    
    output += "graph FiniteAutomata {\n";

    auto undoubled = this->adjacencyMatrix;

    for (auto [vertex, neighbors] : undoubled) for (auto neighbor : neighbors) undoubled[neighbor].erase(vertex);

    for (auto [vertex, neighbors] : undoubled) for (auto neighbor : neighbors) output += "\t\"" + vertex + "\" -- \"" + neighbor + "\" [];\n";

    output += "}";

    return output;
};

void Graph::exportGraph(std::string outputDirPath, std::string outputFileName) const
{
    std::filesystem::create_directories(outputDirPath);

    std::string dotOutputFilePath = outputDirPath + "/" + outputFileName + ".dot";

    std::ofstream dotOutputFile(dotOutputFilePath);

    dotOutputFile << this->toDOT();

    dotOutputFile.close();

    std::string renderDotFileCommand = "dot -Tpng " + dotOutputFilePath + " -o " + outputDirPath + "/" + outputFileName + ".png";

    std::system(renderDotFileCommand.c_str());
};

Graph fileContentToGraph(std::string fileContent)
{
    auto vertexName = sequence("VERTEX", {
        string("\""),
        repetition("NAME", satisfy("CHAR", negate(is('\"')))),
        string("\"")
    });

    auto spaces = satisfy(is(' ')).repeatedly();

    auto newline = satisfy(is('\n')).surroundedBy(spaces).repeatedly(1);
    
    auto edge = satisfy(is('-')).surroundedBy(spaces);

    auto connection = sequence("CONNECTION", {
        vertexName,
        edge,
        vertexName
    });

    auto grammar = strictlySequence("CONTENT", {
        connection.repeatedlyWithDelimeter(newline),
        newline.optionally()
    });

    auto result = parse(fileContent, grammar);
    
    if (getResultType(result) == PARSER_FAILURE) throw std::runtime_error("fileContentToGraph: failed to parse");

    std::unordered_map<std::string, std::unordered_set<std::string>> adjacencyMatrix;

    auto connectionTokens = getTokenFromResult(result).getNestingContent();

    for (auto connectionToken : connectionTokens) {
        auto vertex1 = connectionToken.getNestingContent()[0].contentString();
        auto vertex2 = connectionToken.getNestingContent()[1].contentString();

        adjacencyMatrix[vertex1].insert(vertex2);
        adjacencyMatrix[vertex2].insert(vertex1);
    }
    
    return Graph(adjacencyMatrix);
};

Graph Graph::fromFile(std::string filePath)
{
    std::ifstream file(filePath);

    if (!file.good()) throw std::runtime_error("Graph fromFile: unable to open file");

    std::string fileContent, line;

    while (getline(file, line)) fileContent += line + "\n";
    
    return fileContentToGraph(fileContent);
};

std::string vecToStr(std::vector<std::string> vec)
{
    std::string output;

    for (int i = 0;i<vec.size() - 1;i++) output += vec[i] + ", ";

    if (!vec.empty()) output += vec.back();

    return output;
};

std::string setToStr(std::unordered_set<std::string> set)
{
    return vecToStr(std::vector<std::string>(set.begin(), set.end()));
};