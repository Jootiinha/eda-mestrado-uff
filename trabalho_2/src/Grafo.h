#ifndef GRAFO_H
#define GRAFO_H

#include <array>
#include <limits>
#include <string>
#include <vector>

struct Bairro {
    int id;
    double custoInstalacao;
    std::array<int, 12> demandasMensais;
};

struct Aresta {
    int origem;
    int destino;
    double custo;
};

struct ResultadoAlgoritmo {
    std::vector<double> distancias;
    long long tempoMicrossegundos;
};

struct ResultadoAvaliacaoBairro {
    int indiceBairro;
    int idBairro;
    double custoInstalacao;
    std::array<double, 12> custosMensais;
    double custoOperacaoAnual;
    double custoGalpao;
    ResultadoAlgoritmo resultadoDijkstra;
    ResultadoAlgoritmo resultadoBellmanFord;
};

struct ResultadoEscolhaGalpao {
    std::vector<ResultadoAvaliacaoBairro> avaliacoes;
    int indiceMelhorAvaliacao;
};

struct ResultadoBenchmarkBairro {
    int idBairro;
    int repeticoes;
    double mediaDijkstraMicros;
    double mediaBellmanFordMicros;
};

class Grafo {
public:
    void carregarVertices(const std::string& caminhoArquivo);
    void carregarArestas(const std::string& caminhoArquivo);

    ResultadoAlgoritmo dijkstra(int indiceOrigem) const;
    ResultadoAlgoritmo bellmanFord(int indiceOrigem) const;

    double calcularCustoOperacaoMensal(int indiceDeposito, int mes, const std::vector<double>& distancias) const;
    double calcularCustoOperacaoAnual(int indiceDeposito, const std::vector<double>& distancias) const;
    ResultadoAvaliacaoBairro avaliarBairroCandidato(int indiceDeposito) const;
    ResultadoEscolhaGalpao escolherMelhorGalpao() const;
    std::vector<ResultadoBenchmarkBairro> executarBenchmark(int repeticoes) const;

    int quantidadeVertices() const;
    int quantidadeArestas() const;
    const Bairro& obterBairro(int indice) const;
    int indicePorId(int id) const;

private:
    static constexpr double INFINITO = std::numeric_limits<double>::infinity();

    std::vector<Bairro> bairros_;
    std::vector<Aresta> arestas_;
    std::vector<std::vector<std::pair<int, double>>> adjacencias_;

    static bool distanciasSaoIguais(
        const std::vector<double>& a,
        const std::vector<double>& b,
        double tolerancia = 1e-9
    );
    void validarIndice(int indice) const;
};

#endif
