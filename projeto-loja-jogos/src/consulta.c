#include <stdio.h>
#include <string.h>
#include "consulta.h"


Produto buscarProdutoPorID(int id) {
    FILE *file = fopen("data/produtos.txt", "r"); 
    Produto produto;
    Produto produtoNaoEncontrado = {0, "", "", 0.0, 0}; 

    if (file == NULL) { 
        printf("Erro ao abrir o arquivo de produtos.\n");
        return produtoNaoEncontrado;
    }

    
    while (fscanf(file, "%d %s %s %lf %d", &produto.id, produto.nome, produto.categoria, &produto.preco, &produto.quantidade) != EOF) {
        if (produto.id == id) {
            fclose(file); 
            return produto;
        }
    }

    fclose(file); 
    return produtoNaoEncontrado;
}


Produto buscarProdutoPorNome(char* nome) {
    FILE *file = fopen("data/produtos.txt", "r"); 
    Produto produto;
    Produto produtoNaoEncontrado = {0, "", "", 0.0, 0}; 

    if (file == NULL) { 
        printf("Erro ao abrir o arquivo de produtos.\n");
        return produtoNaoEncontrado;
    }


    while (fscanf(file, "%d %s %s %lf %d", &produto.id, produto.nome, produto.categoria, &produto.preco, &produto.quantidade) != EOF) {
        if (strcmp(produto.nome, nome) == 0) {
            fclose(file);
            return produto;
        }
    }

    fclose(file);
    return produtoNaoEncontrado;
}


void exibirProduto(Produto produto) {
    if (produto.id == 0) { 
        printf("Produto não encontrado.\n");
    } else {
        printf("ID: %d\n", produto.id);
        printf("Nome: %s\n", produto.nome);
        printf("Categoria: %s\n", produto.categoria);
        printf("Preço: %.2f\n", produto.preco);
        printf("Quantidade: %d\n", produto.quantidade);
    }
}
