#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <set>

typedef std::pair<int, int> Edge;
typedef std::set<Edge> EdgeVec;

class Graph
{
public:
    EdgeVec edges;
    int num_edges = 0;
    int num_vertex = 0;

    Graph(EdgeVec edges, int num_vertex)
    {
        this->edges = edges;
        this->num_vertex = num_vertex;
        this->num_edges = edges.size();
    }

    bool has_edge(int i, int j) {
        int min = std::min(i, j);
        int max = std::max(i, j);
        return this->edges.find({min, max}) != this->edges.end();
    }
};

#endif