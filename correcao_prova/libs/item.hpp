#ifndef ITEM_HPP
#define ITEM_HPP

// Classe Base Abstrata para os Elementos (Conforme Slides 24 e 64)
class Item {
public:
    Item() {}
    virtual ~Item() {}
};

// Especialização para Caracteres (Baseado no modelo ItemInt do Slide 28)
class ItemChar : public Item {
public:
    char data;
    ItemChar() {}
    ItemChar(char c) { data = c; }
    virtual ~ItemChar() {}
};

#endif
