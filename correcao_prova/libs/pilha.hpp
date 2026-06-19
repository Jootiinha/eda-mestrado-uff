#ifndef PILHA_HPP
#define PILHA_HPP

#include "item.hpp"

// Nó interno para encadeamento da Pilha (Baseado na classe Element dos Slides 33 e 34)
class ElementoPilha {
public:
    Item* item;
    ElementoPilha* proximo;
    ElementoPilha(Item* i, ElementoPilha* prox = nullptr) {
        item = i;
        proximo = prox;
    }
    ~ElementoPilha() {
        delete item;
    }
};

// Interface e Implementação da Pilha (Conforme especificações dos Slides 18 e 57)
class Pilha {
private:
    ElementoPilha* topo;
    int contadorTamanho;

public:
    Pilha() {
        topo = nullptr;
        contadorTamanho = 0;
    }

    ~Pilha() {
        while (!isEmpty()) {
            Item* aux = pop();
            delete aux;
        }
    }

    // Adiciona um elemento ao topo - Slide 18
    void push(Item* item) {
        ElementoPilha* novo = new ElementoPilha(item, topo);
        topo = novo;
        contadorTamanho++;
    }

    // Remove e retorna o elemento do topo - Slide 18
    Item* pop() {
        if (isEmpty()) return nullptr;
        ElementoPilha* aux = topo;
        Item* itemRetorno = topo->item;

        topo = topo->proximo;
        aux->item = nullptr;
        delete aux;
        contadorTamanho--;
        return itemRetorno;
    }

    // Apenas retorna o elemento do topo - Slide 18
    Item* top() {
        if (isEmpty()) return nullptr;
        return topo->item;
    }

    // Retorna o tamanho atual - Slide 18
    int size() {
        return contadorTamanho;
    }

    // Verifica se está vazia - Slide 18
    bool isEmpty() {
        return topo == nullptr;
    }
};

#endif
