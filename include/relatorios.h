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
    long timestamp;
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
    char nome_categoria[128];
    int total_entradas;
    int total_saidas;
    double custo_total;
    double valor_total_vendas;
} RelatorioCategoria;

RelatorioProduto *GerarRelatorioPorProduto(int id_produto);
RelatorioCategoria *GerarRelatorioPorCategoria(const char *);

void RenderizarRotaRelatorios();

void SalvarEscolhaDadosRelatorio();

#endif // RELATORIO_ESTOQUE_H
