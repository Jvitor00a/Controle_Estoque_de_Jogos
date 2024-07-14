#ifndef PRODUTO_H
#define PRODUTO_H

typedef struct {
    int id;
    char nome[100];
    char categoria[30];
    float preco;
    int quantidade;
} Produto;

#endif