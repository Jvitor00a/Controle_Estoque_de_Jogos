#ifndef PRODUTO_H
#define PRODUTO_H

#define TAMANHO_MAXIMO_NOME_PRODUTO 100
#define TAMANHO_MAXIMO_CATEGORIA_PRODUTO 30

typedef struct Produto
{
    int id;
    char nome[TAMANHO_MAXIMO_NOME_PRODUTO];
    char categoria[TAMANHO_MAXIMO_CATEGORIA_PRODUTO];
    double valor_unitario;
} Produto;

#endif