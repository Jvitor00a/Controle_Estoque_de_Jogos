#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cadastro.h"

// Check if item with same name exists. If not, new item is added with a new id
bool incluirProduto(char *nome, char *categoria, float preço, int quantidade)
{
    printf("Checando se produto ja existe\n")
        FILE *file = fopen("data/produtos.txt", "w");
    Produto produto;

    id = 1 while (fscanf(file, "%d %s %s %f %d", &produto.id, produto.nome, produto.categoria, &produto.preco, &produto.quantidade) != EOF)
    {
        if (strcmp(produto.nome, nome) == 0)
        {
            fclose(file);
            printf("Produto com mesmo nome encontrado. Cadastro Cancelado\n");
            return false;
        }
        id++
    }

    produto.id = id;
    strcpy(produto.nome, nome)
        strcpy(produto.categoria, categoria)
            produto.preco = preço
                                produto.quantidade = quantidade
        fprintf(file, "%d %s %s %.2f %d\n", produto.id, produto.nome, produto.categoria, produto.preco, produto.quantidade)
            fclose(file);
    print("Produto criado no estoque\n");
    return true;
}
