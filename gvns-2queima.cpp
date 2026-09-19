#include "GVNS.h"
#include "Graph.h"

#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

struct ParametrosDoPrograma {
    std::string caminho_do_grafo;
    std::string arquivo_de_saida = "resultados_gvns.csv";
    ParametrosGVNS gvns;
    unsigned tentativas = 1;
    unsigned long semente = 0;
    bool modo_irace = false;
};

struct ResultadoDoPrograma {
    std::string nome_do_grafo;
    std::size_t numero_de_vertices = 0;
    std::size_t numero_de_arestas = 0;
    float densidade = 0.0f;
    unsigned long semente = 0;
    ResultadoGVNS gvns;
};

ParametrosDoPrograma ler_argumentos(int argc, char* argv[]);
void criar_cabecalho_se_necessario(const std::string& nome_do_arquivo);
void salvar_resultado(
    const std::string& nome_do_arquivo,
    const ResultadoDoPrograma& resultado);
void mostrar_ajuda(const char* nome_do_programa);

int main(int argc, char* argv[]) {
    try {
        ParametrosDoPrograma parametros = ler_argumentos(argc, argv);

        if (!parametros.modo_irace) {
            criar_cabecalho_se_necessario(parametros.arquivo_de_saida);
        }

        Graph grafo(parametros.caminho_do_grafo);
        BuscaGVNS gvns(grafo);

        for (unsigned tentativa = 0;
             tentativa < parametros.tentativas;
             tentativa++) {

            unsigned long semente = parametros.semente + tentativa;
            ResultadoGVNS resultado_gvns = gvns.executar(
                parametros.gvns,
                semente);

            if (parametros.modo_irace) {
                std::cout
                    << resultado_gvns.melhor_solucao.avaliacao.rodadas
                    << '\n';
                continue;
            }

            ResultadoDoPrograma resultado;
            resultado.nome_do_grafo = std::filesystem::path(
                parametros.caminho_do_grafo).filename().string();
            resultado.numero_de_vertices = grafo.getOrder();
            resultado.numero_de_arestas = grafo.getSize();
            resultado.densidade = grafo.getDensity();
            resultado.semente = semente;
            resultado.gvns = std::move(resultado_gvns);

            salvar_resultado(parametros.arquivo_de_saida, resultado);
        }
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << '\n';
        return 1;
    }

    return 0;
}

ParametrosDoPrograma ler_argumentos(int argc, char* argv[]) {
    if (argc < 2) {
        mostrar_ajuda(argv[0]);
        throw std::invalid_argument("informe o arquivo do grafo");
    }

    ParametrosDoPrograma parametros;
    parametros.caminho_do_grafo = argv[1];

    for (int posicao = 2; posicao < argc; posicao++) {
        std::string argumento = argv[posicao];

        auto proximo_valor = [&]() -> std::string {
            if (posicao + 1 >= argc) {
                throw std::invalid_argument(
                    "falta o valor de " + argumento);
            }
            posicao++;
            return argv[posicao];
        };

        if (argumento == "--tempo") {
            parametros.gvns.tempo_limite_segundos =
                std::stod(proximo_valor());
        } else if (argumento == "--kmax") {
            parametros.gvns.maior_intensidade =
                std::stoul(proximo_valor());
        } else if (argumento == "--sem-melhoria") {
            parametros.gvns.limite_sem_melhoria =
                std::stoul(proximo_valor());
        } else if (argumento == "--avaliacoes-vizinhanca") {
            parametros.gvns.avaliacoes_por_vizinhanca =
                std::stoul(proximo_valor());
        } else if (argumento == "--avaliacoes-vnd") {
            parametros.gvns.avaliacoes_por_vnd =
                std::stoul(proximo_valor());
        } else if (argumento == "--trials") {
            parametros.tentativas = std::stoul(proximo_valor());
        } else if (argumento == "--seed") {
            parametros.semente = std::stoul(proximo_valor());
        } else if (argumento == "--output") {
            parametros.arquivo_de_saida = proximo_valor();
        } else if (argumento == "--irace") {
            parametros.modo_irace = true;
        } else if (argumento == "--help" || argumento == "-h") {
            mostrar_ajuda(argv[0]);
            std::exit(0);
        } else {
            throw std::invalid_argument(
                "argumento desconhecido: " + argumento);
        }
    }

    if (parametros.tentativas == 0) {
        throw std::invalid_argument("--trials deve ser maior que zero");
    }

    if (parametros.modo_irace && parametros.tentativas != 1) {
        throw std::invalid_argument(
            "o modo --irace exige --trials 1");
    }

    return parametros;
}

void mostrar_ajuda(const char* nome_do_programa) {
    std::cerr
        << "Uso: " << nome_do_programa << " <grafo> [opcoes]\n"
        << "Opcoes:\n"
        << "  --tempo SEGUNDOS\n"
        << "  --kmax VALOR\n"
        << "  --sem-melhoria VALOR\n"
        << "  --avaliacoes-vizinhanca VALOR\n"
        << "  --avaliacoes-vnd VALOR\n"
        << "  --trials VALOR\n"
        << "  --seed VALOR\n"
        << "  --output ARQUIVO\n"
        << "  --irace\n";
}

void criar_cabecalho_se_necessario(const std::string& nome_do_arquivo) {
    if (std::filesystem::exists(nome_do_arquivo)) {
        return;
    }

    std::ofstream arquivo(nome_do_arquivo);
    if (!arquivo) {
        throw std::runtime_error(
            "nao foi possivel criar " + nome_do_arquivo);
    }

    arquivo
        << "graph_name,graph_order,graph_size,density,fitness_value,"
        << "elapsed_time(microsecond),fontes_usadas,iteracoes,avaliacoes,"
        << "melhorias_gvns,melhorias_busca_local,heuristica_inicial,seed\n";
}

void salvar_resultado(
    const std::string& nome_do_arquivo,
    const ResultadoDoPrograma& resultado) {

    std::ofstream arquivo(nome_do_arquivo, std::ios::app);
    if (!arquivo) {
        throw std::runtime_error(
            "nao foi possivel abrir " + nome_do_arquivo);
    }

    const ResultadoGVNS& gvns = resultado.gvns;
    const Solucao& melhor = gvns.melhor_solucao;
    long long microssegundos = static_cast<long long>(
        gvns.tempo_segundos * 1000000.0);

    arquivo
        << resultado.nome_do_grafo << ','
        << resultado.numero_de_vertices << ','
        << resultado.numero_de_arestas << ','
        << resultado.densidade << ','
        << melhor.avaliacao.rodadas << ','
        << microssegundos << ','
        << melhor.avaliacao.fontes_usadas << ','
        << gvns.iteracoes << ','
        << gvns.avaliacoes << ','
        << gvns.melhorias_do_gvns << ','
        << gvns.melhorias_da_busca_local << ','
        << gvns.heuristica_inicial << ','
        << resultado.semente << '\n';

    // Mantem o mesmo formato ja usado pelas versoes do BRKGA.
    arquivo << "sequence,[ ";
    for (std::size_t posicao = 0;
         posicao < melhor.fontes.size();
         posicao++) {
        arquivo << melhor.fontes[posicao];
        if (posicao + 1 < melhor.fontes.size()) {
            arquivo << ' ';
        }
    }
    arquivo << " ]\n";
}
