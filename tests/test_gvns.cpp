#include "Avaliador.h"
#include "GVNS.h"
#include "Graph.h"
#include "VND.h"
#include "Vizinhancas.h"
#include "heuristica_inicial/HeuristicaInicial.h"

#include <cassert>
#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

Graph criar_ciclo(int numero_vertices) {
    Graph grafo;
    for (int vertice = 0; vertice < numero_vertices; vertice++) {
        grafo.addEdge(
            static_cast<std::size_t>(vertice),
            static_cast<std::size_t>((vertice + 1) % numero_vertices));
    }
    return grafo;
}

void testar_avaliacao_e_normalizacao() {
    Graph ciclo = criar_ciclo(10);
    Avaliador avaliador(ciclo);

    Solucao completa = avaliador.avaliar({0, 2, 4, 6, 8});
    assert(completa.avaliacao.completa);
    assert(completa.avaliacao.rodadas == 6);
    assert(completa.avaliacao.fontes_usadas == 5);

    Solucao incompleta = avaliador.avaliar({0});
    assert(!incompleta.avaliacao.completa);

    Solucao com_fonte_desnecessaria =
        avaliador.avaliar({0, 2, 4, 6, 8, 1});
    assert(com_fonte_desnecessaria.avaliacao.completa);
    assert(com_fonte_desnecessaria.fontes == completa.fontes);

    Graph um_vertice;
    um_vertice.addVertex(0);
    Solucao direta = Avaliador(um_vertice).avaliar({0});
    assert(direta.avaliacao.completa);
    assert(direta.avaliacao.rodadas == 1);
}

void testar_solucoes_iniciais() {
    Graph ciclo = criar_ciclo(10);
    Avaliador avaliador(ciclo);

    SolucaoInicial por_grau = construir_por_grau_relativo(ciclo);
    SolucaoInicial combinada =
        construir_por_grau_relativo_e_gatilho(ciclo);

    Solucao primeira = avaliador.avaliar(por_grau.fontes_de_queima);
    Solucao segunda = avaliador.avaliar(combinada.fontes_de_queima);

    assert(primeira.avaliacao.completa);
    assert(segunda.avaliacao.completa);
}

void testar_vizinhancas_e_vnd() {
    Graph ciclo = criar_ciclo(12);
    Avaliador avaliador(ciclo);
    Vizinhancas vizinhancas(ciclo, avaliador);
    VND vnd(vizinhancas);
    std::mt19937 gerador(17);

    SolucaoInicial inicial = construir_por_grau_relativo(ciclo);
    Solucao solucao = avaliador.avaliar(inicial.fontes_de_queima);
    Solucao antes = solucao;

    auto limite = std::chrono::steady_clock::now() +
        std::chrono::seconds(1);

    const std::array<TipoVizinhanca, 5> tipos = {
        TipoVizinhanca::remocao,
        TipoVizinhanca::substituicao,
        TipoVizinhanca::realocacao,
        TipoVizinhanca::troca,
        TipoVizinhanca::adicao
    };

    for (TipoVizinhanca tipo : tipos) {
        Solucao candidata = solucao;
        unsigned avaliacoes = 0;
        vizinhancas.primeira_melhoria(
            tipo,
            candidata,
            gerador,
            50,
            limite,
            avaliacoes);
        assert(candidata.avaliacao.completa);
        assert(!avaliador.melhor_que(solucao, candidata));
        assert(avaliacoes <= 50);
    }

    ResultadoVND resultado = vnd.melhorar(
        solucao,
        gerador,
        50,
        500,
        limite);

    assert(solucao.avaliacao.completa);
    assert(!avaliador.melhor_que(antes, solucao));
    assert(resultado.avaliacoes <= 500);

    for (unsigned intensidade = 1; intensidade <= 5; intensidade++) {
        unsigned avaliacoes = 0;
        Solucao perturbada = vizinhancas.perturbar(
            solucao,
            intensidade,
            gerador,
            avaliacoes,
            limite);
        assert(perturbada.avaliacao.completa);
        assert(avaliacoes <= 20);
    }
}

void testar_rejeicao_sem_reparo() {
    Graph ciclo = criar_ciclo(10);
    Avaliador avaliador(ciclo);
    Vizinhancas vizinhancas(ciclo, avaliador);
    std::mt19937 gerador(31);

    Solucao solucao = avaliador.avaliar({0, 2, 4, 6, 8});
    std::vector<int> fontes_antes = solucao.fontes;
    unsigned avaliacoes = 0;
    auto limite = std::chrono::steady_clock::now() +
        std::chrono::seconds(1);

    bool aceitou = vizinhancas.primeira_melhoria(
        TipoVizinhanca::remocao,
        solucao,
        gerador,
        20,
        limite,
        avaliacoes);

    assert(!aceitou);
    assert(solucao.fontes == fontes_antes);
    assert(solucao.avaliacao.completa);
}

void testar_gvns_completo() {
    Graph ciclo = criar_ciclo(20);
    Avaliador avaliador(ciclo);

    SolucaoInicial inicial = construir_por_grau_relativo(ciclo);
    Solucao referencia = avaliador.avaliar(inicial.fontes_de_queima);

    ParametrosGVNS parametros;
    parametros.tempo_limite_segundos = 0.05;
    parametros.maior_intensidade = 3;
    parametros.limite_sem_melhoria = 10;
    parametros.avaliacoes_por_vizinhanca = 30;
    parametros.avaliacoes_por_vnd = 200;

    BuscaGVNS gvns(ciclo);
    ResultadoGVNS resultado = gvns.executar(parametros, 123);

    assert(resultado.melhor_solucao.avaliacao.completa);
    assert(!avaliador.melhor_que(
        referencia,
        resultado.melhor_solucao));
    assert(resultado.avaliacoes >= 2);
}

int main() {
    testar_avaliacao_e_normalizacao();
    testar_solucoes_iniciais();
    testar_vizinhancas_e_vnd();
    testar_rejeicao_sem_reparo();
    testar_gvns_completo();

    std::cout << "Todos os testes do GVNS passaram.\n";
    return 0;
}
