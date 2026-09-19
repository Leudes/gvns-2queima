#include "Avaliador.h"
#include "SimulacaoQueima.h"

#include <cstddef>
#include <stdexcept>

Avaliador::Avaliador(const Graph& grafo_recebido)
    : grafo(grafo_recebido) {
}

Solucao Avaliador::avaliar(const std::vector<int>& fontes) const {
    Solucao solucao;
    EstadoQueima estado;
    estado.reiniciar(grafo.getOrder());

    std::size_t proxima_fonte = 0;
    bool processo_continua = !estado.todos_queimados();

    while (processo_continua) {
        solucao.avaliacao.rodadas++;
        bool algum_vertice_queimou = false;

        for (int vertice : estado.fila_propagacao_atual) {
            if (queimar_vertice(grafo, vertice, estado)) {
                algum_vertice_queimou = true;
                solucao.avaliacao.soma_das_rodadas_de_queima +=
                    solucao.avaliacao.rodadas;
            }
        }
        estado.fila_propagacao_atual.clear();

        if (estado.todos_queimados()) {
            break;
        }

        while (proxima_fonte < fontes.size()) {
            int vertice = fontes[proxima_fonte];
            proxima_fonte++;

            if (vertice < 0 ||
                vertice >= static_cast<int>(grafo.getOrder())) {
                throw std::invalid_argument(
                    "A sequencia possui um vertice invalido");
            }

            if (estado.queimado[vertice]) {
                continue;
            }

            queimar_vertice(grafo, vertice, estado);
            solucao.fontes.push_back(vertice);
            solucao.avaliacao.fontes_usadas++;
            solucao.avaliacao.soma_das_rodadas_de_queima +=
                solucao.avaliacao.rodadas;
            algum_vertice_queimou = true;
            break;
        }

        // Se a fonte escolhida acabou de queimar o ultimo vertice, a rodada
        // atual ja terminou o processo. Antes, o avaliador entrava em uma
        // rodada vazia adicional somente para perceber que todos queimaram.
        if (estado.todos_queimados()) {
            break;
        }

        terminar_rodada(estado);

        processo_continua = algum_vertice_queimou ||
            !estado.fila_propagacao_atual.empty();
    }

    solucao.avaliacao.completa = estado.todos_queimados();
    solucao.avaliacao.vertices_queimados = estado.total_queimados;
    return solucao;
}

bool Avaliador::melhor_que(
    const Solucao& candidata,
    const Solucao& referencia) const {

    if (candidata.avaliacao.completa != referencia.avaliacao.completa) {
        return candidata.avaliacao.completa;
    }

    if (!candidata.avaliacao.completa) {
        if (candidata.avaliacao.vertices_queimados !=
            referencia.avaliacao.vertices_queimados) {
            return candidata.avaliacao.vertices_queimados >
                referencia.avaliacao.vertices_queimados;
        }
        return false;
    }

    if (candidata.avaliacao.rodadas != referencia.avaliacao.rodadas) {
        return candidata.avaliacao.rodadas < referencia.avaliacao.rodadas;
    }

    if (candidata.avaliacao.soma_das_rodadas_de_queima !=
        referencia.avaliacao.soma_das_rodadas_de_queima) {
        return candidata.avaliacao.soma_das_rodadas_de_queima <
            referencia.avaliacao.soma_das_rodadas_de_queima;
    }

    if (candidata.avaliacao.fontes_usadas !=
        referencia.avaliacao.fontes_usadas) {
        return candidata.avaliacao.fontes_usadas <
            referencia.avaliacao.fontes_usadas;
    }

    return candidata.fontes.size() < referencia.fontes.size();
}
