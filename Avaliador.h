#ifndef AVALIADOR_GVNS_H
#define AVALIADOR_GVNS_H

#include "Graph.h"
#include "Solucao.h"

#include <vector>

class Avaliador {
private:
    const Graph& grafo;

public:
    explicit Avaliador(const Graph& grafo_recebido);

    Solucao avaliar(const std::vector<int>& fontes) const;

    bool melhor_que(
        const Solucao& candidata,
        const Solucao& referencia) const;
};

#endif
