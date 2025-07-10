#include <catch2/catch_all.hpp>

#include "../src/graph.hpp"

TEST_CASE("GRAPH") {
    /*
    
            a-b
            |/|
            c d-e
            |\  /
            f g

            h-i-j
            \  /
             k

            l-m
    
    */

    Graph graph({
        { "a", { "b", "c" } },
        { "b", { "a", "c", "d" } },
        { "c", { "a", "b", "f", "g" } },
        { "d", { "b", "e" } },
        { "e", { "d", "g" } },
        { "f", { "c" } },
        { "g", { "c", "e" } },

        { "h", { "i", "k" } },
        { "i", { "h", "j" } },
        { "j", { "i", "k" } },
        { "k", { "h", "j" } },

        { "l", { "m" } },
        { "m", { "l" } }
    });

    std::vector<std::unordered_set<std::string>> communities = {
        { "a", "b", "c", "d", "e", "f", "g" },
        { "h", "i", "j", "k" },
        { "l", "m" }
    };

    SECTION("COMMUNITY") {
        for (auto community : communities) for (auto vertex : community) REQUIRE(graph.getCommunity(vertex) == community);
    }

    SECTION("COMMUNITIES") {
        REQUIRE(graph.getCommunities().size() == communities.size());
    }

    SECTION("PATH") {
        REQUIRE(graph.getPath("b", "f").value() == std::vector<std::string> { "b", "c", "f" });
        REQUIRE(graph.getPath("a", "e").value().size() == 4);
    }

    SECTION("FROM FILE") {
        REQUIRE(Graph::fromFile("./tests/test.graph") == graph);
    }
}