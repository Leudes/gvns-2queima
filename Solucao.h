#ifndef SOLUCAO_GVNS_H
#define SOLUCAO_GVNS_H

#include <vector>

struct AvaliacaoSolucao {
    bool completa = false;
    int rodadas = 0;
    int fontes_usadas = 0;
    int vertices_queimados = 0;
    long long soma_das_rodadas_de_queima = 0;
};

struct Solucao {
    std::vector<int> fontes;
    AvaliacaoSolucao avaliacao;
};

#endif
