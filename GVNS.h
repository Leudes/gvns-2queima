#ifndef BUSCA_GVNS_H
#define BUSCA_GVNS_H

#include "Avaliador.h"
#include "Solucao.h"
#include "VND.h"
#include "Vizinhancas.h"

#include <string>

struct ParametrosGVNS {
    double tempo_limite_segundos = 10.0;
    unsigned maior_intensidade = 5;
    unsigned limite_sem_melhoria = 100;
    unsigned avaliacoes_por_vizinhanca = 100;
    unsigned avaliacoes_por_vnd = 2000;
};

struct ResultadoGVNS {
    Solucao melhor_solucao;
    std::string heuristica_inicial;
    unsigned iteracoes = 0;
    unsigned avaliacoes = 0;
    unsigned melhorias_do_gvns = 0;
    unsigned melhorias_da_busca_local = 0;
    double tempo_segundos = 0.0;
};

class BuscaGVNS {
private:
    const Graph& grafo;
    Avaliador avaliador;
    Vizinhancas vizinhancas;
    VND busca_local;

public:
    explicit BuscaGVNS(const Graph& grafo_recebido);

    ResultadoGVNS executar(
        const ParametrosGVNS& parametros,
        unsigned long semente) const;
};

#endif
