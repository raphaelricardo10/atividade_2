#include <sstream>
#include <iostream>
#include <tuple>
#include <ilcplex/ilocplex.h>

#include "graph_file.hpp"
#include "graph.hpp"

ILOSTLBEGIN
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Correct call: ./txt filename" << endl;
        exit(1);
    }

    std::string filename(argv[1]);

    ColFile file(filename);
    Graph graph = file.read_file();

    IloEnv env;          // declara Variável de ambiente do CPLEX
    IloModel model(env); // declara Variavel do modelo do CPLEX (que existe dentro do ambiente criado)

    IloBoolVarArray w(env, graph.num_vertex); // declara Vetor de variáveis numéricas (que existe dentro do ambiente criado)
                                              // Vetor inicialmente vazio

    IloArray<IloBoolVarArray> x(env, graph.num_vertex);
    std::cout << "OK!:" << graph.num_vertex;

    // Variável w
    for (int j = 0; j < graph.num_vertex; j++)
    {
        std::ostringstream oss;
        oss << "w_" << j + 1;

        w[j] = IloBoolVar(env, oss.str().c_str());
    }

    // Variável X
    for (int i = 0; i < graph.num_vertex; i++)
    {
        x[i] = IloBoolVarArray(env, graph.num_vertex);

        for (int j = 0; j < graph.num_vertex; j++)
        {
            std::ostringstream oss;
            oss << "x_" << i + 1 << "_" << j + 1;

            // Zera as variáveis que não são arestas
            int max = graph.has_edge(i, j) ? 1 : 0;

            x[i][j] = IloBoolVar(env, 0, max, oss.str().c_str());
            model.add(x[i][j]);
        }
    }

    // Todo vértice deve ter uma cor
    IloConstraintArray single_color_constraints(env);
    for (int i = 0; i < graph.num_vertex; i++)
    {
        IloExpr constraint(env);

        for (int j = 0; j < graph.num_vertex; j++)
        {
            constraint += x[i][j];
        }

        single_color_constraints.add(constraint == 1);
        constraint.end();
    }

    // Cada vértice de uma aresta deve ter uma cor diferente
    IloConstraintArray edge_color_constraints(env);
    for (int j = 0; j < graph.num_vertex; j++)
    {
        for (Edge edge : graph.edges)
        {
            int i = edge.first;
            int k = edge.second;

            edge_color_constraints.add(x[i][j] + x[k][j] <= w[j]);
        }
    }

    // Quebra de simetria
    IloConstraintArray symmetric_break_constraints(env);
    for (int j = 0; j < graph.num_vertex - 1; j++)
    {
        edge_color_constraints.add(w[j] >= w[j + 1]);
    }

    // Restrição de fortalecimento
    IloConstraintArray strong_constraints(env);
    for (int j = 0; j < graph.num_vertex; j++)
    {
        IloExpr constraint(env);
        for (int i = 0; i < graph.num_vertex; i++)
        {
            constraint += x[i][j];
        }

        strong_constraints.add(w[j] <= constraint);
        constraint.end();
    }

    model.add(single_color_constraints);
    model.add(edge_color_constraints);
    model.add(strong_constraints);
    model.add(symmetric_break_constraints);

    // Função Objetivo
    IloExpr fo(env);
    for (int j = 0; j < graph.num_vertex; j++)
    {
        fo += w[j];
    }

    model.add(IloMinimize(env, fo, "FO"));

    try
    {
        IloCplex solver(model); // declara variável "solver" sobre o modelo a ser solucionado
        solver.exportModel("model.lp");
        solver.solve();                                    // chama o "solver"
        cout << "Max=" << solver.getObjValue() << endl;    //  imprime solução do problema
        cout << "LB=" << solver.getBestObjValue() << endl; //  imprime solução do problema
        env.end();
    }
    catch (IloException &e)
    {
        std::cout << e;
    }

    std::cout << "OK!";
}
