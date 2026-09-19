#include "VND.h"

#include <algorithm>
#include <array>

VND::VND(const Vizinhancas& vizinhancas_recebidas)
    : vizinhancas(vizinhancas_recebidas) {
}

ResultadoVND VND::melhorar(
    Solucao& solucao,
    std::mt19937& gerador,
    unsigned limite_por_vizinhanca,
    unsigned limite_total,
    const std::chrono::steady_clock::time_point& limite_de_tempo) const {

    ResultadoVND resultado;

    const std::array<TipoVizinhanca, 5> ordem = {
        TipoVizinhanca::remocao,
        TipoVizinhanca::substituicao,
        TipoVizinhanca::realocacao,
        TipoVizinhanca::troca,
        TipoVizinhanca::adicao
    };

    std::size_t vizinhanca_atual = 0;

    while (vizinhanca_atual < ordem.size() &&
           resultado.avaliacoes < limite_total &&
           std::chrono::steady_clock::now() < limite_de_tempo) {

        unsigned avaliacoes_restantes =
            limite_total - resultado.avaliacoes;
        unsigned limite_desta_busca = std::min(
            limite_por_vizinhanca,
            avaliacoes_restantes);

        bool melhorou = vizinhancas.primeira_melhoria(
            ordem[vizinhanca_atual],
            solucao,
            gerador,
            limite_desta_busca,
            limite_de_tempo,
            resultado.avaliacoes);

        if (melhorou) {
            resultado.melhorias++;
            vizinhanca_atual = 0;
        } else {
            vizinhanca_atual++;
        }
    }

    return resultado;
}
