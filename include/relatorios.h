#ifndef RELATORIO_H
#define RELATORIO_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    char tipo;
    int id_produto;
    int quantidade;
    double preço_unitario;
} Transacao;

typedef struct
{
    int total_entradas;
    int total_saidas;
    double custo_total;
    double valor_total_vendas;
} RelatorioProduto;

typedef struct
{
    int id_categoria;
    int total_entradas;
    int total_saidas;
    double custo_total;
    double valor_total_vendas;
} RelatorioCategoria;

RelatorioProduto *GerarRelatorioPorProduto(int id_produto);
RelatorioCategoria *GerarRelatorioPorCategoria(int *numCategorias);

#endif // RELATORIO_ESTOQUE_H
