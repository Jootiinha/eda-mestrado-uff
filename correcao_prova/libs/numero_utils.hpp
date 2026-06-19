#ifndef NUMERO_UTILS_HPP
#define NUMERO_UTILS_HPP

#include <cctype>
#include <iostream>

#include "item.hpp"
#include "pilha.hpp"

bool ehNumero(char x) {
    return std::isdigit(x);
}

bool ehNumero(Item* item) {
    if (item == nullptr) return false;
    ItemChar* ic = (ItemChar*) item; // Downcast (Semelhante ao Slide 30)
    return ehNumero(ic->data);
}

void removerParenteses(char e[], int n) {
    Pilha p;

    // Varredura da expressão caractere por caractere (Lógica do Slide 20)
    for (int i = 0; i < n; i++) {
        if (e[i] == ')' && !p.isEmpty() && ehNumero(p.top())) {
            Item* aux = p.pop();

            if (!p.isEmpty() && ((ItemChar*)p.top())->data == '(') {
                Item* abreParentese = p.pop(); 
                delete abreParentese;
                p.push(aux); 
            } else {
                p.push(aux);
                p.push(new ItemChar(e[i]));
            }
        } else {
            p.push(new ItemChar(e[i]));
        }
    }

    // Transferência para a segunda pilha auxiliar para não inverter a saída (Slide 21 e 22)
    Pilha q;
    while (!p.isEmpty()) {
        q.push(p.pop());
    }

    // Exibição e limpeza final dos dados (Slide 22)
    std::cout << "Resultado: ";
    while (!q.isEmpty()) {
        ItemChar* ic = (ItemChar*) q.pop();
        std::cout << ic->data;
        delete ic;
    }
    std::cout << std::endl;
}

#endif
