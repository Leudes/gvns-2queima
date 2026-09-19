#include "Vizinhancas.h"

#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>

namespace {

std::vector<int> criar_ordem_aleatoria(
    int quantidade,
    std::mt19937& gerador) {

    std::vector<int> ordem(static_cast<std::size_t>(quantidade));
    std::iota(ordem.begin(), ordem.end(), 0);
    std::shuffle(ordem.begin(), ordem.end(), gerador);
    return ordem;
}

std::vector<int> vertices_fora_da_solucao(
    std::size_t numero_vertices,
    const std::vector<int>& fontes,
    std::mt19937& gerador) {

    std::vector<unsigned char> esta_na_solucao(numero_vertices, 0);
    for (int vertice : fontes) {
        esta_na_solucao[vertice] = 1;
    }

    std::vector<int> vertices;
    for (std::size_t vertice = 0; vertice < numero_vertices; vertice++) {
        if (!esta_na_solucao[vertice]) {
            vertices.push_back(static_cast<int>(vertice));
        }
    }

    std::shuffle(vertices.begin(), vertices.end(), gerador);
    return vertices;
}

bool tempo_esgotado(
    const std::chrono::steady_clock::time_point& limite) {
    return std::chrono::steady_clock::now() >= limite;
}

}  // namespace

Vizinhancas::Vizinhancas(
    const Graph& grafo_recebido,
    const Avaliador& avaliador_recebido)
    : grafo(grafo_recebido),
      avaliador(avaliador_recebido) {
}

bool Vizinhancas::avaliar_e_aceitar(
    Solucao& atual,
    const std::vector<int>& novas_fontes,
    unsigned& avaliacoes) const {

    Solucao candidata = avaliador.avaliar(novas_fontes);
    avaliacoes++;

    // Nao ha reparo: uma sequencia incompleta e simplesmente rejeitada.
    if (!candidata.avaliacao.completa ||
        !avaliador.melhor_que(candidata, atual)) {
        return false;
    }

    atual = std::move(candidata);
    return true;
}

bool Vizinhancas::primeira_melhoria(
    TipoVizinhanca tipo,
    Solucao& atual,
    std::mt19937& gerador,
    unsigned limite_de_avaliacoes,
    const std::chrono::steady_clock::time_point& limite_de_tempo,
    unsigned& avaliacoes) const {

    if (limite_de_avaliacoes == 0 || tempo_esgotado(limite_de_tempo)) {
        return false;
    }

    const unsigned inicio_das_avaliacoes = avaliacoes;
    const int quantidade_fontes = static_cast<int>(atual.fontes.size());
    std::vector<int> posicoes =
        criar_ordem_aleatoria(quantidade_fontes, gerador);

    auto pode_avaliar = [&]() {
        return avaliacoes - inicio_das_avaliacoes < limite_de_avaliacoes &&
            !tempo_esgotado(limite_de_tempo);
    };

    if (tipo == TipoVizinhanca::remocao) {
        for (int posicao : posicoes) {
            if (!pode_avaliar()) {
                break;
            }

            std::vector<int> candidata = atual.fontes;
            candidata.erase(candidata.begin() + posicao);

            if (avaliar_e_aceitar(atual, candidata, avaliacoes)) {
                return true;
            }
        }
        return false;
    }

    std::vector<int> vertices_disponiveis = vertices_fora_da_solucao(
        grafo.getOrder(), atual.fontes, gerador);

    if (tipo == TipoVizinhanca::substituicao) {
        for (int posicao : posicoes) {
            for (int vertice : vertices_disponiveis) {
                if (!pode_avaliar()) {
                    return false;
                }

                std::vector<int> candidata = atual.fontes;
                candidata[posicao] = vertice;

                if (avaliar_e_aceitar(atual, candidata, avaliacoes)) {
                    return true;
                }
            }
        }
        return false;
    }

    if (tipo == TipoVizinhanca::realocacao) {
        std::vector<int> destinos =
            criar_ordem_aleatoria(quantidade_fontes, gerador);

        for (int origem : posicoes) {
            for (int destino : destinos) {
                if (origem == destino) {
                    continue;
                }
                if (!pode_avaliar()) {
                    return false;
                }

                std::vector<int> candidata = atual.fontes;
                int vertice = candidata[origem];
                candidata.erase(candidata.begin() + origem);
                candidata.insert(candidata.begin() + destino, vertice);

                if (avaliar_e_aceitar(atual, candidata, avaliacoes)) {
                    return true;
                }
            }
        }
        return false;
    }

    if (tipo == TipoVizinhanca::troca) {
        for (int primeira : posicoes) {
            for (int segunda : posicoes) {
                if (primeira >= segunda) {
                    continue;
                }
                if (!pode_avaliar()) {
                    return false;
                }

                std::vector<int> candidata = atual.fontes;
                std::swap(candidata[primeira], candidata[segunda]);

                if (avaliar_e_aceitar(atual, candidata, avaliacoes)) {
                    return true;
                }
            }
        }
        return false;
    }

    std::vector<int> destinos =
        criar_ordem_aleatoria(quantidade_fontes + 1, gerador);

    for (int vertice : vertices_disponiveis) {
        for (int destino : destinos) {
            if (!pode_avaliar()) {
                return false;
            }

            std::vector<int> candidata = atual.fontes;
            candidata.insert(candidata.begin() + destino, vertice);

            if (avaliar_e_aceitar(atual, candidata, avaliacoes)) {
                return true;
            }
        }
    }

    return false;
}

bool Vizinhancas::aplicar_movimento_aleatorio(
    std::vector<int>& fontes,
    std::mt19937& gerador) const {

    std::vector<int> disponiveis = vertices_fora_da_solucao(
        grafo.getOrder(), fontes, gerador);

    for (int tentativa = 0; tentativa < 15; tentativa++) {
        std::uniform_int_distribution<int> escolher_tipo(0, 4);
        int tipo = escolher_tipo(gerador);

        if (tipo == 0 && fontes.size() > 1) {
            std::uniform_int_distribution<std::size_t> escolher(
                0, fontes.size() - 1);
            fontes.erase(fontes.begin() + escolher(gerador));
            return true;
        }

        if (tipo == 1 && !fontes.empty() && !disponiveis.empty()) {
            std::uniform_int_distribution<std::size_t> escolher_fonte(
                0, fontes.size() - 1);
            std::uniform_int_distribution<std::size_t> escolher_vertice(
                0, disponiveis.size() - 1);
            fontes[escolher_fonte(gerador)] =
                disponiveis[escolher_vertice(gerador)];
            return true;
        }

        if (tipo == 2 && fontes.size() > 1) {
            std::uniform_int_distribution<std::size_t> escolher(
                0, fontes.size() - 1);
            std::size_t origem = escolher(gerador);
            std::size_t destino = escolher(gerador);

            if (origem != destino) {
                int vertice = fontes[origem];
                fontes.erase(fontes.begin() + origem);
                fontes.insert(fontes.begin() + destino, vertice);
                return true;
            }
        }

        if (tipo == 3 && fontes.size() > 1) {
            std::uniform_int_distribution<std::size_t> escolher(
                0, fontes.size() - 1);
            std::size_t primeira = escolher(gerador);
            std::size_t segunda = escolher(gerador);

            if (primeira != segunda) {
                std::swap(fontes[primeira], fontes[segunda]);
                return true;
            }
        }

        if (tipo == 4 && !disponiveis.empty()) {
            std::uniform_int_distribution<std::size_t> escolher_vertice(
                0, disponiveis.size() - 1);
            std::uniform_int_distribution<std::size_t> escolher_posicao(
                0, fontes.size());
            fontes.insert(
                fontes.begin() + escolher_posicao(gerador),
                disponiveis[escolher_vertice(gerador)]);
            return true;
        }
    }

    return false;
}

Solucao Vizinhancas::perturbar(
    const Solucao& atual,
    unsigned intensidade,
    std::mt19937& gerador,
    unsigned& avaliacoes,
    const std::chrono::steady_clock::time_point& limite_de_tempo) const {

    for (int tentativa = 0;
         tentativa < 20 && !tempo_esgotado(limite_de_tempo);
         tentativa++) {
        std::vector<int> fontes = atual.fontes;
        bool modificou = false;

        for (unsigned movimento = 0; movimento < intensidade; movimento++) {
            if (tempo_esgotado(limite_de_tempo)) {
                return atual;
            }

            modificou = aplicar_movimento_aleatorio(fontes, gerador) ||
                modificou;
        }

        if (!modificou) {
            continue;
        }

        if (tempo_esgotado(limite_de_tempo)) {
            return atual;
        }

        Solucao perturbada = avaliador.avaliar(fontes);
        avaliacoes++;

        // A perturbacao tambem nao repara uma solucao. Ela tenta outro
        // movimento quando a sequencia gerada nao queima todo o grafo.
        if (perturbada.avaliacao.completa &&
            perturbada.fontes != atual.fontes) {
            return perturbada;
        }
    }

    return atual;
}
