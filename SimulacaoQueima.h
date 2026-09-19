#ifndef SIMULACAO_QUEIMA_H
#define SIMULACAO_QUEIMA_H

#include "Graph.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

struct EstadoQueima {
    std::vector<uint8_t> queimado;
    std::vector<int> quantidade_vizinhos_queimados;
    std::vector<int> fila_propagacao_atual;
    std::vector<int> fila_propagacao_seguinte;
    int total_queimados = 0;

    void reiniciar(size_t numero_vertices) {
        if (queimado.size() != numero_vertices) {
            queimado.resize(numero_vertices);
            quantidade_vizinhos_queimados.resize(numero_vertices);
            fila_propagacao_atual.reserve(numero_vertices);
            fila_propagacao_seguinte.reserve(numero_vertices);
        }

        std::fill(queimado.begin(), queimado.end(), 0);
        std::fill(
            quantidade_vizinhos_queimados.begin(),
            quantidade_vizinhos_queimados.end(),
            0);
        fila_propagacao_atual.clear();
        fila_propagacao_seguinte.clear();
        total_queimados = 0;
    }

    bool todos_queimados() const {
        return total_queimados == static_cast<int>(queimado.size());
    }
};

inline bool queimar_vertice(
    const Graph& grafo,
    int vertice,
    EstadoQueima& estado,
    std::vector<int>* vertices_com_um_queimado = nullptr) {

    if (estado.queimado[vertice]) {
        return false;
    }

    estado.queimado[vertice] = 1;
    estado.total_queimados++;

    for (size_t vizinho : grafo.getNeighbors(vertice)) {
        if (estado.queimado[vizinho]) {
            continue;
        }

        estado.quantidade_vizinhos_queimados[vizinho]++;
        int quantidade = estado.quantidade_vizinhos_queimados[vizinho];

        if (vertices_com_um_queimado != nullptr && quantidade == 1) {
            vertices_com_um_queimado->push_back(
                static_cast<int>(vizinho));
        }

        if (quantidade == 2) {
            estado.fila_propagacao_seguinte.push_back(
                static_cast<int>(vizinho));
        }
    }

    return true;
}

inline bool propagar_fogo(
    const Graph& grafo,
    EstadoQueima& estado,
    std::vector<int>* vertices_com_um_queimado = nullptr) {

    bool algum_vertice_queimou = false;

    for (int vertice : estado.fila_propagacao_atual) {
        bool queimou = queimar_vertice(
            grafo,
            vertice,
            estado,
            vertices_com_um_queimado);

        if (queimou) {
            algum_vertice_queimou = true;
        }
    }

    estado.fila_propagacao_atual.clear();
    return algum_vertice_queimou;
}

inline void terminar_rodada(EstadoQueima& estado) {
    estado.fila_propagacao_atual.swap(
        estado.fila_propagacao_seguinte);
}

#endif
