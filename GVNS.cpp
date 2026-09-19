#include "GVNS.h"
#include "heuristica_inicial/HeuristicaInicial.h"

#include <chrono>
#include <random>
#include <stdexcept>
#include <utility>

BuscaGVNS::BuscaGVNS(const Graph& grafo_recebido)
    : grafo(grafo_recebido),
      avaliador(grafo_recebido),
      vizinhancas(grafo_recebido, avaliador),
      busca_local(vizinhancas) {
}

ResultadoGVNS BuscaGVNS::executar(
    const ParametrosGVNS& parametros,
    unsigned long semente) const {

    if (parametros.tempo_limite_segundos <= 0.0 ||
        parametros.maior_intensidade == 0 ||
        parametros.limite_sem_melhoria == 0 ||
        parametros.avaliacoes_por_vizinhanca == 0 ||
        parametros.avaliacoes_por_vnd == 0) {
        throw std::invalid_argument(
            "Todos os parametros do GVNS devem ser maiores que zero");
    }

    ResultadoGVNS resultado;
    std::mt19937 gerador(semente);

    auto inicio = std::chrono::steady_clock::now();
    auto duracao = std::chrono::duration<double>(
        parametros.tempo_limite_segundos);
    auto limite_de_tempo = inicio +
        std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            duracao);

    SolucaoInicial inicial_grau = construir_por_grau_relativo(grafo);
    SolucaoInicial inicial_combinada =
        construir_por_grau_relativo_e_gatilho(grafo);

    Solucao por_grau = avaliador.avaliar(
        inicial_grau.fontes_de_queima);
    Solucao combinada = avaliador.avaliar(
        inicial_combinada.fontes_de_queima);
    resultado.avaliacoes += 2;

    Solucao atual;
    if (avaliador.melhor_que(combinada, por_grau)) {
        atual = std::move(combinada);
        resultado.heuristica_inicial = "grau_relativo_e_gatilho";
    } else {
        atual = std::move(por_grau);
        resultado.heuristica_inicial = "grau_relativo";
    }

    ResultadoVND primeiro_vnd = busca_local.melhorar(
        atual,
        gerador,
        parametros.avaliacoes_por_vizinhanca,
        parametros.avaliacoes_por_vnd,
        limite_de_tempo);
    resultado.avaliacoes += primeiro_vnd.avaliacoes;
    resultado.melhorias_da_busca_local += primeiro_vnd.melhorias;

    Solucao melhor = atual;
    unsigned sem_melhoria = 0;

    while (std::chrono::steady_clock::now() < limite_de_tempo &&
           sem_melhoria < parametros.limite_sem_melhoria) {

        unsigned intensidade = 1;

        while (intensidade <= parametros.maior_intensidade &&
               sem_melhoria < parametros.limite_sem_melhoria &&
               std::chrono::steady_clock::now() < limite_de_tempo) {

            resultado.iteracoes++;

            unsigned avaliacoes_da_perturbacao = 0;
            Solucao candidata = vizinhancas.perturbar(
                atual,
                intensidade,
                gerador,
                avaliacoes_da_perturbacao,
                limite_de_tempo);
            resultado.avaliacoes += avaliacoes_da_perturbacao;

            if (candidata.fontes == atual.fontes) {
                sem_melhoria++;
                intensidade++;
                continue;
            }

            ResultadoVND resultado_vnd = busca_local.melhorar(
                candidata,
                gerador,
                parametros.avaliacoes_por_vizinhanca,
                parametros.avaliacoes_por_vnd,
                limite_de_tempo);
            resultado.avaliacoes += resultado_vnd.avaliacoes;
            resultado.melhorias_da_busca_local +=
                resultado_vnd.melhorias;

            if (avaliador.melhor_que(candidata, atual)) {
                atual = std::move(candidata);
                resultado.melhorias_do_gvns++;
                sem_melhoria = 0;
                intensidade = 1;

                if (avaliador.melhor_que(atual, melhor)) {
                    melhor = atual;
                }
            } else {
                sem_melhoria++;
                intensidade++;
            }
        }
    }

    auto fim = std::chrono::steady_clock::now();
    resultado.tempo_segundos =
        std::chrono::duration<double>(fim - inicio).count();
    resultado.melhor_solucao = std::move(melhor);
    return resultado;
}
