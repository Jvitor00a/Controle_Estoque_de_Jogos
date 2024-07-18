#include "relatorios.h"
#include "registro.h"
#include <raygui.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static struct tm data_inicio = {0}; // Supondo que esta variável já esteja inicializada em outro lugar
static struct tm data_fim = {9999, 11, 31, 23, 59, 59, 0, 0, 0}; // Data muito distante

// Função auxiliar para verificar se a transação está dentro do intervalo de datas
bool TransacaoDentroDoIntervalo(struct tm *tempo_transacao)
{
    time_t tempo_transacao_t = mktime(tempo_transacao);
    return difftime(tempo_transacao_t, mktime(&data_inicio)) >= 0 &&
           difftime(tempo_transacao_t, mktime(&data_fim)) <= 0;
}

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

    *relatorio = (RelatorioProduto){0};

    char linha[100];
    Transacao transacao;

    while (fgets(linha, sizeof(linha), arquivo) != NULL)
    {
        sscanf(linha, FORMATO_LINHA_REGISTRO, &transacao.tipo, &transacao.id_produto, &transacao.quantidade,
               &transacao.preço_unitario, &transacao.timestamp);

        struct tm *tempo_transacao = localtime(&transacao.timestamp);

        if (tempo_transacao == NULL)
        {
            printf("Erro ao converter timestamp da transacao\n");
            abort();
        }

        // Verificar se a transação está dentro do intervalo de datas e é do produto desejado
        if (transacao.id_produto == id_produto && TransacaoDentroDoIntervalo(tempo_transacao))
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
               &transacao.preço_unitario, &transacao.timestamp);

        // Exemplo simples de cálculo da categoria
        int id_categoria = transacao.id_produto % 100; // Exemplo de mapeamento simples

        // Verificar se a transação está dentro do intervalo de datas
        struct tm *tempo_transacao = localtime(&transacao.timestamp);

        if (tempo_transacao == NULL)
        {
            printf("Erro ao converter timestamp da transacao\n");
            abort();
        }

        // Procurar se a categoria já está no relatório
        bool categoria_encontrada = false;
        for (int i = 0; i < *num_categorias; i++)
        {
            if (relatorio[i].id_categoria == id_categoria)
            {
                categoria_encontrada = true;
                break;
            }
        }

        // Se a categoria não foi encontrada, adicioná-la ao relatório
        if (!categoria_encontrada)
        {
            relatorio[*num_categorias].id_categoria = id_categoria;
            (*num_categorias)++;
        }

        // Atualizar os totais da categoria
        for (int i = 0; i < *num_categorias; i++)
        {
            if (relatorio[i].id_categoria == id_categoria && TransacaoDentroDoIntervalo(tempo_transacao))
            {
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
    }

    fclose(arquivo);
    return relatorio;
}

bool WindowBox000Active = true;
bool DropdownBox002EditMode = false;
int DropdownBox002Active = 0;
bool DropdownBox004EditMode = false;
int DropdownBox004Active = 0;
bool DropdownBox005EditMode = false;
int DropdownBox005Active = 0;
bool DropdownBox007EditMode = false;
int DropdownBox007Active = 0;
bool DropdownBox008EditMode = false;
int DropdownBox008Active = 0;
bool DropdownBox009EditMode = false;
int DropdownBox009Active = 0;
bool TextBox010EditMode = false;
char TextBox010Text[128] = "999";
bool TextBox011EditMode = false;
char TextBox011Text[128] = "999";
bool TextBox017EditMode = false;
char TextBox017Text[128] = "999";
bool TextBox019EditMode = false;
char TextBox019Text[128] = "99,99";
bool TextBox023EditMode = false;
char TextBox023Text[128] = "SAMPLE TEXT";
bool TextBox025EditMode = false;
char TextBox025Text[128] = "9999,99";
bool DropdownBox028EditMode = false;
int DropdownBox028Active = 0;
bool DropdownBox030EditMode = false;
int DropdownBox030Active = 0;
bool Button031Pressed = false;

void RenderizarRotaRelatorios()
{
    if (DropdownBox002EditMode || DropdownBox004EditMode || DropdownBox005EditMode || DropdownBox007EditMode ||
        DropdownBox008EditMode || DropdownBox009EditMode || DropdownBox028EditMode || DropdownBox030EditMode)
        GuiLock();

    if (WindowBox000Active)
    {

        GuiGroupBox((Rectangle){8, 48, 584, 80}, "Filtros");
        // Escolha data de inicio do relatorio
        GuiLabel((Rectangle){16, 56, 32, 24}, "Início");
        if (GuiDropdownBox((Rectangle){48, 56, 48, 24}, "10", &DropdownBox002Active, DropdownBox002EditMode))
            DropdownBox002EditMode = !DropdownBox002EditMode;
        if (GuiDropdownBox((Rectangle){96, 56, 120, 24}, "Janeiro", &DropdownBox004Active, DropdownBox004EditMode))
            DropdownBox004EditMode = !DropdownBox004EditMode;
        if (GuiDropdownBox((Rectangle){216, 56, 56, 24}, "1900", &DropdownBox005Active, DropdownBox005EditMode))
            DropdownBox005EditMode = !DropdownBox005EditMode;

        // Escolha data de fim do relatorio
        GuiLabel((Rectangle){336, 56, 24, 24}, "Fim");
        if (GuiDropdownBox((Rectangle){360, 56, 48, 24}, "10", &DropdownBox007Active, DropdownBox007EditMode))
            DropdownBox007EditMode = !DropdownBox007EditMode;
        if (GuiDropdownBox((Rectangle){408, 56, 120, 24}, "Janeiro", &DropdownBox008Active, DropdownBox008EditMode))
            DropdownBox008EditMode = !DropdownBox008EditMode;
        if (GuiDropdownBox((Rectangle){528, 56, 56, 24}, "1900", &DropdownBox009Active, DropdownBox009EditMode))
            DropdownBox009EditMode = !DropdownBox009EditMode;

        // Escolha do tipo de relatorio
        GuiLabel((Rectangle){40, 96, 112, 24}, "Gerar relatório por");
        if (GuiDropdownBox((Rectangle){152, 96, 120, 24}, "Produto; Categoria", &DropdownBox028Active,
                           DropdownBox028EditMode))
            DropdownBox028EditMode = !DropdownBox028EditMode;
        if (GuiDropdownBox((Rectangle){280, 96, 120, 24}, "ProdutoC6", &DropdownBox030Active, DropdownBox030EditMode))
            DropdownBox030EditMode = !DropdownBox030EditMode;

        Button031Pressed = GuiButton((Rectangle){464, 96, 120, 24}, "Gerar");

        GuiTextBox((Rectangle){144, 152, 120, 24}, TextBox010Text, 128, false);
        GuiTextBox((Rectangle){352, 152, 120, 24}, TextBox011Text, 128, false);

        GuiLabel((Rectangle){144, 136, 120, 24}, "Unidades compradas:");
        GuiLabel((Rectangle){352, 136, 104, 24}, "Unidades vendidas:");
        GuiLabel((Rectangle){144, 240, 120, 24}, "Balanço:");
        GuiLabel((Rectangle){264, 152, 40, 24}, " un(s).");
        GuiLabel((Rectangle){472, 152, 40, 24}, " un(s).");
        GuiTextBox((Rectangle){144, 256, 120, 24}, TextBox017Text, 128, false);
        GuiLabel((Rectangle){264, 256, 40, 24}, " un(s).");
        GuiTextBox((Rectangle){144, 208, 120, 24}, TextBox019Text, 128, false);
        GuiLabel((Rectangle){144, 192, 120, 24}, "Custo unitário médio:");
        GuiLabel((Rectangle){128, 208, 16, 24}, "R$");
        GuiLabel((Rectangle){352, 192, 120, 24}, "Lucro unitário médio");
        GuiTextBox((Rectangle){352, 208, 120, 24}, TextBox023Text, 128, false);
        GuiLabel((Rectangle){336, 208, 16, 24}, "R$");
        GuiTextBox((Rectangle){352, 256, 120, 24}, TextBox025Text, 128, false);
        GuiLabel((Rectangle){352, 240, 120, 24}, "Lucro total:");
        GuiLabel((Rectangle){336, 256, 16, 24}, "R$");
    }

    GuiUnlock();
}
