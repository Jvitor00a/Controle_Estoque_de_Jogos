#ifndef RELATORIO_ESTOQUE_H
#define RELATORIO_ESTOQUE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ARQUIVO_REGISTRO "registro_estoque.txt"
#define PREFIXO_ENTRADA 'E'
#define PREFIXO_SAIDA 'S'
#define FORMATO_LINHA_REGISTRO "%c %d %d\n"

// Estrutura para armazenar cada transação
typedef struct {
    char tipo;
    int idProduto;
    int quantidade;
} Transacao;

// Estrutura para armazenar informações agregadas por categoria
typedef struct {
    int idCategoria;
    int totalEntradas;
    int totalSaidas;
} RelatorioCategoria;

// Protótipos das funções
void GerarRelatorioPorProduto(int idProduto);
void GerarRelatorioPorCategoria();

#endif // RELATORIO_ESTOQUE_H
