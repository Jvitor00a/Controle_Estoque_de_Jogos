#ifndef CONSULTA_H
#define CONSULTA_H

typedef struct {
    int id;
    char nome[100];
    char categoria[30];
    float preco;
    int quantidade;
} Produto;

Produto buscarProdutoPorId(int id);

Produto buscarProdutoPorNome(char* nome);

void exibirProduto(Produto produto);

#endif

