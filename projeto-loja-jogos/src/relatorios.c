#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ARQUIVO_REGISTRO "registro_estoque.txt"
#define PREFIXO_ENTRADA 'E'
#define PREFIXO_SAIDA 'S'
#define FORMATO_LINHA_REGISTRO "%c %d %d\n"


// Função que lê o arquivo, filtra por produto e gera relatório
void GerarRelatorioPorProduto(int idProduto) {
    FILE *arquivo = fopen(ARQUIVO_REGISTRO, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", ARQUIVO_REGISTRO);
        return;
    }

    int totalEntradas = 0, totalSaidas = 0;

    printf("Relatório para o Produto %d:\n", idProduto);

    char linha[100];
    Transacao transacao;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        sscanf(linha, FORMATO_LINHA_REGISTRO, &transacao.tipo, &transacao.idProduto, &transacao.quantidade);

        if (transacao.idProduto == idProduto) {
            printf("Tipo: %c, Quantidade: %d\n", transacao.tipo, transacao.quantidade);

            if (transacao.tipo == PREFIXO_ENTRADA) {
                totalEntradas += transacao.quantidade;
            } else if (transacao.tipo == PREFIXO_SAIDA) {
                totalSaidas += transacao.quantidade;
            }
        }
    }

    printf("Total de Entradas: %d\n", totalEntradas);
    printf("Total de Saídas: %d\n", totalSaidas);

    fclose(arquivo);
}

// Função para ler o arquivo de registros, gerar o relatório por categoria e salvar em um arquivo
void GerarRelatorioPorCategoria() {
    FILE *arquivo = fopen(ARQUIVO_REGISTRO, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", ARQUIVO_REGISTRO);
        return;
    }

    // Inicializar estrutura para armazenar o relatório por categoria
    RelatorioCategoria relatorio[100]; // Supondo um máximo de 100 categorias
    int numCategorias = 0;

    // Inicializar totais de categorias
    for (int i = 0; i < 100; i++) {
        relatorio[i].idCategoria = -1; // Marcar como inválido
        relatorio[i].totalEntradas = 0;
        relatorio[i].totalSaidas = 0;
    }

    char linha[100];
    Transacao transacao;

    // Ler cada linha do arquivo
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        sscanf(linha, FORMATO_LINHA_REGISTRO, &transacao.tipo, &transacao.idProduto, &transacao.quantidade);

        // Exemplo simples de cálculo da categoria
        int idCategoria = transacao.idProduto % 100; // Exemplo de mapeamento simples

        // Procurar se a categoria já está no relatório
        bool categoriaEncontrada = false;
        for (int i = 0; i < numCategorias; i++) {
            if (relatorio[i].idCategoria == idCategoria) {
                categoriaEncontrada = true;

                if (transacao.tipo == PREFIXO_ENTRADA) {
                    relatorio[i].totalEntradas += transacao.quantidade;
                } else if (transacao.tipo == PREFIXO_SAIDA) {
                    relatorio[i].totalSaidas += transacao.quantidade;
                }

                break;
            }
        }
         // Fechar o arquivo de entrada
    fclose(arquivo);

    // Abrir arquivo para escrever o relatório
    FILE *arquivoRelatorio = fopen("relatorio_por_categoria.txt", "w");
    if (arquivoRelatorio == NULL) {
        printf("Erro ao abrir o arquivo de relatório\n");
        return;
    }

    // Escrever o relatório no arquivo
    fprintf(arquivoRelatorio, "Relatório por Categoria de Produto:\n");
    for (int i = 0; i < numCategorias; i++) {
        fprintf(arquivoRelatorio, "Categoria %d:\n", relatorio[i].idCategoria);
        fprintf(arquivoRelatorio, "Total de Entradas: %d\n", relatorio[i].totalEntradas);
        fprintf(arquivoRelatorio, "Total de Saídas: %d\n", relatorio[i].totalSaidas);
        fprintf(arquivoRelatorio, "\n");
    }

    // Fechar o arquivo de relatório
    fclose(arquivoRelatorio);

    printf("Relatório gerado com sucesso\n");
}