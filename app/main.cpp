#include <iostream>

#include "../src/graph.hpp"

bool poll(Graph& graph)
{
    std::cout << "Enter a command (neighbors, community, communities, vertices, path, exit): ";

    std::string command;
    std::cin >> command;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << std::endl;

    if (command == "neighbors") {
        std::cout << "Enter a vertex: ";
        
        std::string vertex;
        getline(std::cin, vertex);
        
        std::cout << std::endl;

        std::cout << "Neighbors of " << vertex << ": " << setToStr(graph.getNeighbors(vertex)) << std::endl;
    }
    else if (command == "community") {
        std::cout << "Enter a vertex: ";

        std::string vertex;
        getline(std::cin, vertex);
        
        std::cout << "Community of " << vertex << ": " << setToStr(graph.getCommunity(vertex)) << std::endl;
    }
    else if (command == "communities") {
        auto communities = graph.getCommunities();

        for (auto community : communities) std::cout << "Community: " << setToStr(community) << std::endl;
    }
    else if (command == "vertices") {
        std::cout << "Vertices: " << setToStr(graph.getVertices()) << std::endl;
    }
    else if (command == "path") {
        std::cout << "Enter start vertex: ";
        
        std::string vertex1;
        getline(std::cin, vertex1);

        std::cout << std::endl;

        std::cout << "Enter end vertex: ";

        std::string vertex2;
        getline(std::cin, vertex2);

        std::cout << "Path from " << vertex1 << " to " << vertex2 << ": ";
        
        auto path = graph.getPath(vertex1, vertex2);

        if (path.has_value()) std::cout << vecToStr(path.value()) << std::endl;
        else std::cout << "(NA)" << std::endl;
    }
    else if (command == "exit") return false;
    else {
        std::cout << "Unrecognized command." << std::endl;
    }

    return true;
};

int main()
{
    std::string graphName = "camp";

    Graph graph = Graph::fromFile("./app/examples/" + graphName + ".graph");
    graph.exportGraph("./results/" + graphName, "graph");

    while (true) {
        bool running = poll(graph);

        if (!running) break;
    }

    return 0;
};