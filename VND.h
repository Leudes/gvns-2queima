#ifndef VND_GVNS_H
#define VND_GVNS_H

#include "Solucao.h"
#include "Vizinhancas.h"

#include <chrono>
#include <random>

struct ResultadoVND {
    unsigned avaliacoes = 0;
    unsigned melhorias = 0;
};

class VND {
private:
    const Vizinhancas& vizinhancas;

public:
    explicit VND(const Vizinhancas& vizinhancas_recebidas);

    ResultadoVND melhorar(
        Solucao& solucao,
        std::mt19937& gerador,
        unsigned limite_por_vizinhanca,
        unsigned limite_total,
        const std::chrono::steady_clock::time_point& limite_de_tempo) const;
};

#endif
