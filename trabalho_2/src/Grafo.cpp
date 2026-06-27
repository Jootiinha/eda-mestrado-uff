#include "Grafo.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <queue>
#include <stdexcept>

void Grafo::carregarVertices(const std::string& caminhoArquivo) {
    std::ifstream arquivo(caminhoArquivo);
    if (!arquivo) {
        throw std::runtime_error("Nao foi possivel abrir o arquivo de vertices: " + caminhoArquivo);
    }

    int quantidade = 0;
    arquivo >> quantidade;
    if (!arquivo || quantidade <= 0) {
        throw std::runtime_error("Quantidade de vertices invalida no arquivo: " + caminhoArquivo);
    }

    bairros_.clear();
    bairros_.reserve(quantidade);
    adjacencias_.assign(quantidade, {});

    for (int i = 0; i < quantidade; ++i) {
        Bairro bairro{};
        arquivo >> bairro.id >> bairro.custoInstalacao;
        for (int mes = 0; mes < 12; ++mes) {
            arquivo >> bairro.demandasMensais[mes];
        }

        if (!arquivo) {
            throw std::runtime_error("Linha de vertice incompleta no arquivo: " + caminhoArquivo);
        }

        bairros_.push_back(bairro);
    }
}

void Grafo::carregarArestas(const std::string& caminhoArquivo) {
    if (bairros_.empty()) {
        throw std::runtime_error("Carregue os vertices antes das arestas.");
    }

    std::ifstream arquivo(caminhoArquivo);
    if (!arquivo) {
        throw std::runtime_error("Nao foi possivel abrir o arquivo de arestas: " + caminhoArquivo);
    }

    int quantidade = 0;
    arquivo >> quantidade;
    if (!arquivo || quantidade < 0) {
        throw std::runtime_error("Quantidade de arestas invalida no arquivo: " + caminhoArquivo);
    }

    arestas_.clear();
    for (auto& lista : adjacencias_) {
        lista.clear();
    }

    for (int i = 0; i < quantidade; ++i) {
        int idOrigem = 0;
        int idDestino = 0;
        double custo = 0.0;
        arquivo >> idOrigem >> idDestino >> custo;
        if (!arquivo) {
            throw std::runtime_error("Linha de aresta incompleta no arquivo: " + caminhoArquivo);
        }

        const int origem = indicePorId(idOrigem);
        const int destino = indicePorId(idDestino);
        if (origem < 0 || destino < 0) {
            throw std::runtime_error("Aresta referencia vertice inexistente.");
        }

        arestas_.push_back({origem, destino, custo});

        // O problema trata bairros vizinhos com distancia entre eles; aqui o grafo
        // foi modelado como nao direcionado para refletir essa vizinhanca.
        adjacencias_[origem].push_back({destino, custo});
        adjacencias_[destino].push_back({origem, custo});
    }
}

ResultadoAlgoritmo Grafo::dijkstra(int indiceOrigem) const {
    validarIndice(indiceOrigem);

    const auto inicio = std::chrono::high_resolution_clock::now();

    std::vector<double> distancias(quantidadeVertices(), INFINITO);
    distancias[indiceOrigem] = 0.0;

    using NoFila = std::pair<double, int>;
    std::priority_queue<NoFila, std::vector<NoFila>, std::greater<NoFila>> fila;
    fila.push({0.0, indiceOrigem});

    while (!fila.empty()) {
        const auto [distanciaAtual, atual] = fila.top();
        fila.pop();

        if (distanciaAtual > distancias[atual]) {
            continue;
        }

        for (const auto& [vizinho, custo] : adjacencias_[atual]) {
            const double candidata = distancias[atual] + custo;
            if (candidata < distancias[vizinho]) {
                distancias[vizinho] = candidata;
                fila.push({candidata, vizinho});
            }
        }
    }

    const auto fim = std::chrono::high_resolution_clock::now();
    const auto tempo = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio).count();
    return {distancias, tempo};
}

ResultadoAlgoritmo Grafo::bellmanFord(int indiceOrigem) const {
    validarIndice(indiceOrigem);

    const auto inicio = std::chrono::high_resolution_clock::now();

    std::vector<double> distancias(quantidadeVertices(), INFINITO);
    distancias[indiceOrigem] = 0.0;

    for (int iteracao = 0; iteracao < quantidadeVertices() - 1; ++iteracao) {
        bool houveAtualizacao = false;

        for (const auto& aresta : arestas_) {
            if (distancias[aresta.origem] != INFINITO &&
                distancias[aresta.origem] + aresta.custo < distancias[aresta.destino]) {
                distancias[aresta.destino] = distancias[aresta.origem] + aresta.custo;
                houveAtualizacao = true;
            }

            if (distancias[aresta.destino] != INFINITO &&
                distancias[aresta.destino] + aresta.custo < distancias[aresta.origem]) {
                distancias[aresta.origem] = distancias[aresta.destino] + aresta.custo;
                houveAtualizacao = true;
            }
        }

        if (!houveAtualizacao) {
            break;
        }
    }

    const auto fim = std::chrono::high_resolution_clock::now();
    const auto tempo = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio).count();
    return {distancias, tempo};
}

double Grafo::calcularCustoOperacaoMensal(
    int indiceDeposito,
    int mes,
    const std::vector<double>& distancias
) const {
    validarIndice(indiceDeposito);
    if (mes < 0 || mes >= 12) {
        throw std::runtime_error("Mes invalido.");
    }
    if (static_cast<int>(distancias.size()) != quantidadeVertices()) {
        throw std::runtime_error("Vetor de distancias incompatível com o grafo.");
    }

    double custoMensal = 0.0;
    for (int i = 0; i < quantidadeVertices(); ++i) {
        custoMensal += distancias[i] * static_cast<double>(bairros_[i].demandasMensais[mes]);
    }
    return custoMensal;
}

double Grafo::calcularCustoOperacaoAnual(int indiceDeposito, const std::vector<double>& distancias) const {
    double acumulado = 0.0;
    for (int mes = 0; mes < 12; ++mes) {
        acumulado += calcularCustoOperacaoMensal(indiceDeposito, mes, distancias);
    }
    return acumulado;
}

ResultadoAvaliacaoBairro Grafo::avaliarBairroCandidato(int indiceDeposito) const {
    validarIndice(indiceDeposito);

    const auto resultadoDijkstra = dijkstra(indiceDeposito);
    const auto resultadoBellmanFord = bellmanFord(indiceDeposito);
    if (!distanciasSaoIguais(resultadoDijkstra.distancias, resultadoBellmanFord.distancias)) {
        throw std::runtime_error("Dijkstra e Bellman-Ford retornaram distancias diferentes.");
    }

    ResultadoAvaliacaoBairro avaliacao{};
    avaliacao.indiceBairro = indiceDeposito;
    avaliacao.idBairro = bairros_[indiceDeposito].id;
    avaliacao.custoInstalacao = bairros_[indiceDeposito].custoInstalacao;
    avaliacao.resultadoDijkstra = resultadoDijkstra;
    avaliacao.resultadoBellmanFord = resultadoBellmanFord;

    for (int mes = 0; mes < 12; ++mes) {
        avaliacao.custosMensais[mes] =
            calcularCustoOperacaoMensal(indiceDeposito, mes, resultadoDijkstra.distancias);
    }

    avaliacao.custoOperacaoAnual =
        calcularCustoOperacaoAnual(indiceDeposito, resultadoDijkstra.distancias);
    avaliacao.custoGalpao = avaliacao.custoInstalacao - avaliacao.custoOperacaoAnual;
    return avaliacao;
}

ResultadoEscolhaGalpao Grafo::escolherMelhorGalpao() const {
    if (bairros_.empty()) {
        throw std::runtime_error("Nao ha bairros carregados para analise.");
    }

    ResultadoEscolhaGalpao resultado{};
    resultado.indiceMelhorAvaliacao = -1;

    for (int indice = 0; indice < quantidadeVertices(); ++indice) {
        resultado.avaliacoes.push_back(avaliarBairroCandidato(indice));
        const auto& avaliacaoAtual = resultado.avaliacoes.back();

        if (resultado.indiceMelhorAvaliacao < 0 ||
            avaliacaoAtual.custoGalpao < resultado.avaliacoes[resultado.indiceMelhorAvaliacao].custoGalpao) {
            resultado.indiceMelhorAvaliacao = static_cast<int>(resultado.avaliacoes.size()) - 1;
        }
    }

    return resultado;
}

std::vector<ResultadoBenchmarkBairro> Grafo::executarBenchmark(int repeticoes) const {
    if (repeticoes <= 0) {
        throw std::runtime_error("O numero de repeticoes do benchmark deve ser positivo.");
    }

    std::vector<ResultadoBenchmarkBairro> resultados;
    resultados.reserve(quantidadeVertices());

    for (int deposito = 0; deposito < quantidadeVertices(); ++deposito) {
        double somaDijkstraTempoRealMicros = 0.0;
        double somaBellmanTempoRealMicros = 0.0;
        double somaDijkstraCpuMicros = 0.0;
        double somaBellmanCpuMicros = 0.0;
        std::vector<double> distanciasReferencia;

        for (int repeticao = 0; repeticao < repeticoes; ++repeticao) {
            const std::clock_t inicioCpuDijkstra = std::clock();
            const auto inicioDijkstra = std::chrono::high_resolution_clock::now();
            const auto resultadoDijkstra = dijkstra(deposito);
            const auto fimDijkstra = std::chrono::high_resolution_clock::now();
            const std::clock_t fimCpuDijkstra = std::clock();

            const std::clock_t inicioCpuBellman = std::clock();
            const auto inicioBellman = std::chrono::high_resolution_clock::now();
            const auto resultadoBellman = bellmanFord(deposito);
            const auto fimBellman = std::chrono::high_resolution_clock::now();
            const std::clock_t fimCpuBellman = std::clock();

            if (!distanciasSaoIguais(resultadoDijkstra.distancias, resultadoBellman.distancias)) {
                throw std::runtime_error("Dijkstra e Bellman-Ford retornaram distancias diferentes no benchmark.");
            }

            if (repeticao == 0) {
                distanciasReferencia = resultadoDijkstra.distancias;
            } else if (!distanciasSaoIguais(distanciasReferencia, resultadoDijkstra.distancias)) {
                throw std::runtime_error("Dijkstra retornou distancias inconsistentes entre repeticoes.");
            }

            somaDijkstraTempoRealMicros += std::chrono::duration<double, std::micro>(fimDijkstra - inicioDijkstra).count();
            somaBellmanTempoRealMicros += std::chrono::duration<double, std::micro>(fimBellman - inicioBellman).count();
            somaDijkstraCpuMicros +=
                static_cast<double>(fimCpuDijkstra - inicioCpuDijkstra) * 1000000.0 / static_cast<double>(CLOCKS_PER_SEC);
            somaBellmanCpuMicros +=
                static_cast<double>(fimCpuBellman - inicioCpuBellman) * 1000000.0 / static_cast<double>(CLOCKS_PER_SEC);
        }

        resultados.push_back({
            bairros_[deposito].id,
            repeticoes,
            somaDijkstraTempoRealMicros / static_cast<double>(repeticoes),
            somaBellmanTempoRealMicros / static_cast<double>(repeticoes),
            somaDijkstraCpuMicros / static_cast<double>(repeticoes),
            somaBellmanCpuMicros / static_cast<double>(repeticoes),
        });
    }

    return resultados;
}

int Grafo::quantidadeVertices() const {
    return static_cast<int>(bairros_.size());
}

int Grafo::quantidadeArestas() const {
    return static_cast<int>(arestas_.size());
}

const Bairro& Grafo::obterBairro(int indice) const {
    validarIndice(indice);
    return bairros_[indice];
}

int Grafo::indicePorId(int id) const {
    for (int i = 0; i < quantidadeVertices(); ++i) {
        if (bairros_[i].id == id) {
            return i;
        }
    }
    return -1;
}

bool Grafo::distanciasSaoIguais(
    const std::vector<double>& a,
    const std::vector<double>& b,
    double tolerancia
) {
    if (a.size() != b.size()) {
        return false;
    }

    for (std::size_t i = 0; i < a.size(); ++i) {
        if (std::abs(a[i] - b[i]) > tolerancia) {
            return false;
        }
    }
    return true;
}

void Grafo::validarIndice(int indice) const {
    if (indice < 0 || indice >= quantidadeVertices()) {
        throw std::runtime_error("Indice de vertice invalido.");
    }
}
