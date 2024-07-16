#ifndef RELATORIO_H
#define RELATORIO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    char tipo;
    int idProduto;
    int quantidade;
    double precoUnitario;
} Transacao;

typedef struct
{
    int totalEntradas;
    int totalSaidas;
    double custoTotal;
    double valorTotalVendas;
} RelatorioProduto;

typedef struct
{
    int idCategoria;
    int totalEntradas;
    int totalSaidas;
    double custoTotal;
    double valorTotalVendas;
} RelatorioCategoria;

RelatorioProduto *GerarRelatorioPorProduto(int idProduto);
RelatorioCategoria *GerarRelatorioPorCategoria(int *numCategorias);

#endif // RELATORIO_ESTOQUE_H
