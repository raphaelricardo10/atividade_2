#ifndef GRAPH_FILE_H
#define GRAPH_FILE_H

#include <set>
#include <vector>
#include <string>
#include <fstream>

#include "matrix.hpp"
#include "graph.hpp"

class ColFile
{
private:
    int number_of_entries;

    std::vector<std::string> split_line(std::string line)
    {
        size_t pos = 0;
        std::string token;
        char delimiter = ' ';
        std::vector<std::string> all_tokens;

        while ((pos = line.find(delimiter)) != std::string::npos)
        {
            token = line.substr(0, pos);
            all_tokens.push_back(token);
            line.erase(0, pos + 1);
        }

        all_tokens.push_back(line);

        return all_tokens;
    }

public:
    std::string filename;

    ColFile(std::string filename)
    {
        this->filename = filename;
        this->number_of_entries = 0;
    }

    Graph read_file()
    {
        std::string fileLine;
        std::ifstream fileStream(this->filename);

        getline(fileStream, fileLine);

        std::vector<std::string> header = this->split_line(fileLine);

        EdgeVec edges;
        int num_vertex = atoi(header[2].c_str());

        while (getline(fileStream, fileLine))
        {
            std::vector<std::string> entry = this->split_line(fileLine);

            int src_vertex = atoi(entry[1].c_str()) - 1;
            int dest_vertex = atoi(entry[2].c_str()) - 1;

            edges.insert(std::make_tuple(src_vertex, dest_vertex));
        }

        return Graph(edges, num_vertex);
    }
};

#endif