#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "relatorios.h"
#include "registro.h"

// Função que lê o arquivo, filtra por produto e gera relatório
RelatorioProduto *GerarRelatorioPorProduto(int idProduto)
{
    FILE *arquivo = fopen(ARQUIVO_REGISTRO, "r");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", ARQUIVO_REGISTRO);
        return NULL;
    }

    RelatorioProduto *relatorio = (RelatorioProduto *)malloc(sizeof(RelatorioProduto));
    if (relatorio == NULL)
    {
        printf("Erro ao alocar memória\n");
        fclose(arquivo);
        return NULL;
    }
    relatorio->totalEntradas = 0;
    relatorio->totalSaidas = 0;
    relatorio->custoTotal = 0.0;
    relatorio->valorTotalVendas = 0.0;

    char linha[100];
    Transacao transacao;

    while (fgets(linha, sizeof(linha), arquivo) != NULL)
    {
        sscanf(linha, FORMATO_LINHA_REGISTRO, &transacao.tipo, &transacao.idProduto, &transacao.quantidade, &transacao.precoUnitario);

        if (transacao.idProduto == idProduto)
        {
            if (transacao.tipo == IDENTIFICADOR_ENTRADA)
            {
                relatorio->totalEntradas += transacao.quantidade;
                relatorio->custoTotal += transacao.quantidade * transacao.precoUnitario;
            }
            else if (transacao.tipo == IDENTIFICADOR_SAIDA)
            {
                relatorio->totalSaidas += transacao.quantidade;
                relatorio->valorTotalVendas += transacao.quantidade * transacao.precoUnitario;
            }
        }
    }

    fclose(arquivo);
    return relatorio;
}

// Função para ler o arquivo de registros, gerar o relatório por categoria
RelatorioCategoria *GerarRelatorioPorCategoria(int *numCategorias)
{
    FILE *arquivo = fopen(ARQUIVO_REGISTRO, "r");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", ARQUIVO_REGISTRO);
        return NULL;
    }

    RelatorioCategoria *relatorio = (RelatorioCategoria *)malloc(100 * sizeof(RelatorioCategoria)); // Supondo um máximo de 100 categorias
    if (relatorio == NULL)
    {
        printf("Erro ao alocar memória\n");
        fclose(arquivo);
        return NULL;
    }

    *numCategorias = 0;

    // Inicializar totais de categorias
    for (int i = 0; i < 100; i++)
    {
        relatorio[i].idCategoria = -1; // Marcar como inválido
        relatorio[i].totalEntradas = 0;
        relatorio[i].totalSaidas = 0;
        relatorio[i].custoTotal = 0.0;
        relatorio[i].valorTotalVendas = 0.0;
    }

    char linha[100];
    Transacao transacao;

    // Ler cada linha do arquivo
    while (fgets(linha, sizeof(linha), arquivo) != NULL)
    {
        sscanf(linha, FORMATO_LINHA_REGISTRO, &transacao.tipo, &transacao.idProduto, &transacao.quantidade, &transacao.precoUnitario);

        // Exemplo simples de cálculo da categoria
        int idCategoria = transacao.idProduto % 100; // Exemplo de mapeamento simples

        // Procurar se a categoria já está no relatório
        bool categoriaEncontrada = false;
        for (int i = 0; i < *numCategorias; i++)
        {
            if (relatorio[i].idCategoria == idCategoria)
            {
                categoriaEncontrada = true;

                if (transacao.tipo == IDENTIFICADOR_ENTRADA)
                {
                    relatorio[i].totalEntradas += transacao.quantidade;
                    relatorio[i].custoTotal += transacao.quantidade * transacao.precoUnitario;
                }
                else if (transacao.tipo == IDENTIFICADOR_SAIDA)
                {
                    relatorio[i].totalSaidas += transacao.quantidade;
                    relatorio[i].valorTotalVendas += transacao.quantidade * transacao.precoUnitario;
                }

                break;
            }
        }

        if (!categoriaEncontrada)
        {
            relatorio[*numCategorias].idCategoria = idCategoria;
            if (transacao.tipo == IDENTIFICADOR_ENTRADA)
            {
                relatorio[*numCategorias].totalEntradas = transacao.quantidade;
                relatorio[*numCategorias].custoTotal = transacao.quantidade * transacao.precoUnitario;
            }
            else if (transacao.tipo == IDENTIFICADOR_SAIDA)
            {
                relatorio[*numCategorias].totalSaidas = transacao.quantidade;
                relatorio[*numCategorias].valorTotalVendas = transacao.quantidade * transacao.precoUnitario;
            }
            (*numCategorias)++;
        }
    }

    fclose(arquivo);
    return relatorio;
}
