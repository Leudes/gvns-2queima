#ifndef VIZINHANCAS_GVNS_H
#define VIZINHANCAS_GVNS_H

#include "Avaliador.h"
#include "Graph.h"
#include "Solucao.h"

#include <chrono>
#include <random>
#include <vector>

enum class TipoVizinhanca {
    remocao,
    substituicao,
    realocacao,
    troca,
    adicao
};

class Vizinhancas {
private:
    const Graph& grafo;
    const Avaliador& avaliador;

    bool avaliar_e_aceitar(
        Solucao& atual,
        const std::vector<int>& novas_fontes,
        unsigned& avaliacoes) const;

    bool aplicar_movimento_aleatorio(
        std::vector<int>& fontes,
        std::mt19937& gerador) const;

public:
    Vizinhancas(
        const Graph& grafo_recebido,
        const Avaliador& avaliador_recebido);

    bool primeira_melhoria(
        TipoVizinhanca tipo,
        Solucao& atual,
        std::mt19937& gerador,
        unsigned limite_de_avaliacoes,
        const std::chrono::steady_clock::time_point& limite_de_tempo,
        unsigned& avaliacoes) const;

    Solucao perturbar(
        const Solucao& atual,
        unsigned intensidade,
        std::mt19937& gerador,
        unsigned& avaliacoes,
        const std::chrono::steady_clock::time_point& limite_de_tempo) const;
};

#endif
