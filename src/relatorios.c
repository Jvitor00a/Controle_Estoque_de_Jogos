#include "relatorios.h"
#include "registro.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Função que lê o arquivo, filtra por produto e gera relatório
RelatorioProduto *GerarRelatorioPorProduto(int id_produto)
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
    relatorio->total_entradas = 0;
    relatorio->total_saidas = 0;
    relatorio->custo_total = 0.0;
    relatorio->valor_total_vendas = 0.0;

    char linha[100];
    Transacao transacao;

    while (fgets(linha, sizeof(linha), arquivo) != NULL)
    {
        sscanf(linha, FORMATO_LINHA_REGISTRO, &transacao.tipo, &transacao.id_produto, &transacao.quantidade,
               &transacao.preço_unitario);

        if (transacao.id_produto == id_produto)
        {
            if (transacao.tipo == IDENTIFICADOR_ENTRADA)
            {
                relatorio->total_entradas += transacao.quantidade;
                relatorio->custo_total += transacao.quantidade * transacao.preço_unitario;
            }
            else if (transacao.tipo == IDENTIFICADOR_SAIDA)
            {
                relatorio->total_saidas += transacao.quantidade;
                relatorio->valor_total_vendas += transacao.quantidade * transacao.preço_unitario;
            }
        }
    }

    fclose(arquivo);
    return relatorio;
}

// Função para ler o arquivo de registros, gerar o relatório por categoria
RelatorioCategoria *GerarRelatorioPorCategoria(int *num_categorias)
{
    FILE *arquivo = fopen(ARQUIVO_REGISTRO, "r");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", ARQUIVO_REGISTRO);
        return NULL;
    }

    RelatorioCategoria *relatorio =
        (RelatorioCategoria *)malloc(100 * sizeof(RelatorioCategoria)); // Supondo um máximo de 100 categorias
    if (relatorio == NULL)
    {
        printf("Erro ao alocar memória\n");
        fclose(arquivo);
        return NULL;
    }

    *num_categorias = 0;

    // Inicializar totais de categorias
    for (int i = 0; i < 100; i++)
    {
        relatorio[i].id_categoria = -1; // Marcar como inválido
        relatorio[i].total_entradas = 0;
        relatorio[i].total_saidas = 0;
        relatorio[i].custo_total = 0.0;
        relatorio[i].valor_total_vendas = 0.0;
    }

    char linha[100];
    Transacao transacao;

    // Ler cada linha do arquivo
    while (fgets(linha, sizeof(linha), arquivo) != NULL)
    {
        sscanf(linha, FORMATO_LINHA_REGISTRO, &transacao.tipo, &transacao.id_produto, &transacao.quantidade,
               &transacao.preço_unitario);

        // Exemplo simples de cálculo da categoria
        int id_categoria = transacao.id_produto % 100; // Exemplo de mapeamento simples

        // Procurar se a categoria já está no relatório
        bool categoria_encontrada = false;
        for (int i = 0; i < *num_categorias; i++)
        {
            if (relatorio[i].id_categoria == id_categoria)
            {
                categoria_encontrada = true;

                if (transacao.tipo == IDENTIFICADOR_ENTRADA)
                {
                    relatorio[i].total_entradas += transacao.quantidade;
                    relatorio[i].custo_total += transacao.quantidade * transacao.preço_unitario;
                }
                else if (transacao.tipo == IDENTIFICADOR_SAIDA)
                {
                    relatorio[i].total_saidas += transacao.quantidade;
                    relatorio[i].valor_total_vendas += transacao.quantidade * transacao.preço_unitario;
                }

                break;
            }
        }

        if (!categoria_encontrada)
        {
            relatorio[*num_categorias].id_categoria = id_categoria;
            if (transacao.tipo == IDENTIFICADOR_ENTRADA)
            {
                relatorio[*num_categorias].total_entradas = transacao.quantidade;
                relatorio[*num_categorias].custo_total = transacao.quantidade * transacao.preço_unitario;
            }
            else if (transacao.tipo == IDENTIFICADOR_SAIDA)
            {
                relatorio[*num_categorias].total_saidas = transacao.quantidade;
                relatorio[*num_categorias].valor_total_vendas = transacao.quantidade * transacao.preço_unitario;
            }
            (*num_categorias)++;
        }
    }

    fclose(arquivo);
    return relatorio;
}
