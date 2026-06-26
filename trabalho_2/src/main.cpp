#include "Grafo.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void executarAnalisePadrao(const Grafo& grafo) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Quantidade de bairros: " << grafo.quantidadeVertices() << "\n";
    std::cout << "Quantidade de arestas: " << grafo.quantidadeArestas() << "\n\n";

    const ResultadoEscolhaGalpao resultado = grafo.escolherMelhorGalpao();
    for (const auto& avaliacao : resultado.avaliacoes) {
        std::cout << "Bairro " << avaliacao.idBairro << "\n";
        std::cout << "  Custo de instalacao: " << avaliacao.custoInstalacao << "\n";
        for (int mes = 0; mes < 12; ++mes) {
            std::cout << "  Mes " << (mes + 1) << ": custo operacional = " << avaliacao.custosMensais[mes] << "\n";
        }

        std::cout << "  Custo operacional anual: " << avaliacao.custoOperacaoAnual << "\n";
        std::cout << "  Custo galpao (instalacao - operacao anual): " << avaliacao.custoGalpao << "\n";
        std::cout << "  Tempo Dijkstra (us): " << avaliacao.resultadoDijkstra.tempoMicrossegundos << "\n";
        std::cout << "  Tempo Bellman-Ford (us): " << avaliacao.resultadoBellmanFord.tempoMicrossegundos << "\n\n";
    }

    const auto& melhorAvaliacao = resultado.avaliacoes[resultado.indiceMelhorAvaliacao];
    std::cout << "Melhor bairro para instalar o galpao: " << melhorAvaliacao.idBairro << "\n";
    std::cout << "Menor custoGalpao encontrado: " << melhorAvaliacao.custoGalpao << "\n";
}

void executarBenchmark(const Grafo& grafo, const std::string& caminhoCsv, int repeticoes) {
    std::ofstream arquivoSaida(caminhoCsv);
    if (!arquivoSaida) {
        throw std::runtime_error("Nao foi possivel criar o arquivo CSV: " + caminhoCsv);
    }

    arquivoSaida << "bairro_id,repeticoes,media_dijkstra_us,media_bellman_ford_us\n";
    arquivoSaida << std::fixed << std::setprecision(4);

    const auto resultados = grafo.executarBenchmark(repeticoes);
    for (const auto& resultado : resultados) {
        arquivoSaida
            << resultado.idBairro << ","
            << resultado.repeticoes << ","
            << resultado.mediaDijkstraMicros << ","
            << resultado.mediaBellmanFordMicros << "\n";
    }

    std::cout << "Arquivo CSV gerado em: " << caminhoCsv << "\n";
    std::cout << "Benchmark executado com " << repeticoes << " repeticoes por bairro.\n";
}
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 3 && argc != 6) {
            std::cerr << "Uso: " << argv[0] << " <arquivo_vertices> <arquivo_arestas>\n";
            std::cerr << "Ou:  " << argv[0]
                      << " --benchmark <arquivo_vertices> <arquivo_arestas> <arquivo_csv> <repeticoes>\n";
            return 1;
        }

        Grafo grafo;
        if (argc == 3) {
            grafo.carregarVertices(argv[1]);
            grafo.carregarArestas(argv[2]);
            executarAnalisePadrao(grafo);
            return 0;
        }

        const std::string modo = argv[1];
        if (modo != "--benchmark") {
            throw std::runtime_error("Modo invalido. Use --benchmark para gerar o CSV de tempos.");
        }

        grafo.carregarVertices(argv[2]);
        grafo.carregarArestas(argv[3]);
        executarBenchmark(grafo, argv[4], std::stoi(argv[5]));
        return 0;
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << "\n";
        return 1;
    }
}
