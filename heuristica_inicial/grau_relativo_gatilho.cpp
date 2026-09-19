#include "HeuristicaInicial.h"

#include <chrono>
#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_do_grafo>\n";
        return 1;
    }

    try {
        Graph grafo(argv[1]);

        auto inicio = std::chrono::steady_clock::now();
        SolucaoInicial solucao =
            construir_por_grau_relativo_e_gatilho(grafo);
        auto fim = std::chrono::steady_clock::now();

        long long tempo_em_microssegundos =
            std::chrono::duration_cast<std::chrono::microseconds>(
                fim - inicio).count();

        std::cout << "heuristica: grau relativo e gatilho\n";
        std::cout << "vertices: " << grafo.getOrder() << '\n';
        std::cout << "arestas: " << grafo.getSize() << '\n';
        std::cout << "rodadas: " << solucao.numero_de_rodadas << '\n';
        std::cout << "fontes diretas: "
                  << solucao.fontes_de_queima.size() << '\n';
        std::cout << "tempo (microssegundos): "
                  << tempo_em_microssegundos << '\n';
        std::cout << "sequencia:";

        for (int vertice : solucao.fontes_de_queima) {
            std::cout << ' ' << vertice;
        }

        std::cout << '\n';
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << '\n';
        return 1;
    }

    return 0;
}
