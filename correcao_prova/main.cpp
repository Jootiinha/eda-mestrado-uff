#include <iostream>

#include "libs/numero_utils.hpp"

int main() {
    // Exemplo do enunciado: (((2)-(1))*(4))
    char expressao[] = {'(', '(', '(', '2', ')', '-', '(', '1', ')', ')', '*', '(', '4', ')', ')'};
    int tamanho = sizeof(expressao) / sizeof(expressao[0]);

    std::cout << "Expressao original: (((2)-(1))*(4))" << std::endl;

    // Executa a filtragem
    removerParentesesExcedentes(expressao, tamanho);

    return 0;
}
