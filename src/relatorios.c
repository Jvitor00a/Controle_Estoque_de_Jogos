#include "relatorios.h"
#include "consulta.h"
#include "datas.h"
#include "registro.h"
#include <raygui.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static struct tm data_inicio = {0}; // Supondo que esta variavel ja esteja inicializada em outro lugar
static struct tm data_fim = {9999, 11, 31, 23, 59, 59, 0, 0, 0}; // Data muito distante

// Função auxiliar para verificar se a transação está dentro do intervalo de datas
bool TransacaoDentroDoIntervalo(struct tm *tempo_transacao)
{
    time_t tempo_transacao_t = mktime(tempo_transacao);
    return difftime(tempo_transacao_t, mktime(&data_inicio)) >= 0 &&
           difftime(tempo_transacao_t, mktime(&data_fim)) <= 0;
}

// Assume que Janeiro = 0
int DiasNoMes(int mes, int ano)
{
    struct tm time_in = {0, 0, 0, 1, mes, ano - 1900};
    struct tm time_out;

    // Passa para o próximo mes
    time_in.tm_mon += 1;

    // time_in agora se refere ao primeiro dia do próximo mes
    mktime(&time_in);

    // Pegamos agora o dia anterior, ou seja, o último do mes especificado
    time_in.tm_mday = 0;

    // Normalizar novamente a estrutura
    mktime(&time_in);

    return time_in.tm_mday;
}

int ObterAnoAtual()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_year + 1900;
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
               &transacao.preco_unitario, &transacao.timestamp);

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
                relatorio->custo_total += transacao.quantidade * transacao.preco_unitario;
            }
            else if (transacao.tipo == IDENTIFICADOR_SAIDA)
            {
                relatorio->total_saidas += transacao.quantidade;
                relatorio->valor_total_vendas += transacao.quantidade * transacao.preco_unitario;
            }
        }
    }

    fclose(arquivo);
    return relatorio;
}

// Função para ler o arquivo de registros, gerar o relatório por categoria
RelatorioCategoria *GerarRelatorioPorCategoria(const char *nome_categoria)
{
    FILE *arquivo = fopen(ARQUIVO_REGISTRO, "r");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", ARQUIVO_REGISTRO);
        return NULL;
    }

    RelatorioCategoria *relatorio = (RelatorioCategoria *)malloc(sizeof(RelatorioCategoria));
    if (relatorio == NULL)
    {
        printf("Erro ao alocar memória\n");
        fclose(arquivo);
        return NULL;
    }

    // Inicializar o relatório para a categoria fornecida
    strcpy(relatorio->nome_categoria, nome_categoria);
    relatorio->total_entradas = 0;
    relatorio->total_saidas = 0;
    relatorio->custo_total = 0;
    relatorio->valor_total_vendas = 0;

    char linha[100];
    Transacao transacao;

    // Ler cada linha do arquivo
    while (fgets(linha, sizeof(linha), arquivo) != NULL)
    {
        sscanf(linha, FORMATO_LINHA_REGISTRO, &transacao.tipo, &transacao.id_produto, &transacao.quantidade,
               &transacao.preco_unitario, &transacao.timestamp);

        Produto produto = BuscarProdutoPorID(transacao.id_produto);
        if (produto.id == 0)
        {
            TraceLog(LOG_ERROR, "Não foi possível encontrar produto com id especificado");
            continue;
        }

        // Verificar se a transação está dentro do intervalo de datas
        struct tm *tempo_transacao = localtime(&transacao.timestamp);

        if (tempo_transacao == NULL)
        {
            printf("Erro ao converter timestamp da transacao\n");
            continue;
        }

        // Ignorar transações fora do intervalo de datas
        if (!TransacaoDentroDoIntervalo(tempo_transacao))
        {
            continue;
        }

        // Atualizar os totais da categoria se a categoria corresponder
        if (strcmp(produto.categoria, nome_categoria) == 0)
        {
            if (transacao.tipo == IDENTIFICADOR_ENTRADA)
            {
                relatorio->total_entradas += transacao.quantidade;
                relatorio->custo_total += transacao.quantidade * transacao.preco_unitario;
            }
            else if (transacao.tipo == IDENTIFICADOR_SAIDA)
            {
                relatorio->total_saidas += transacao.quantidade;
                relatorio->valor_total_vendas += transacao.quantidade * transacao.preco_unitario;
            }
        }
    }

    fclose(arquivo);
    return relatorio;
}

// Cria uma string que contém o nome de todos os produtos separados por ';'
static char *CriarOpcoesParaListaProdutos()
{
    ListaProduto lista = ListarProdutos();
    size_t tamanho_total = 0;
    char *separador = ";";

    for (ListaProdutoNo no = ListaProdutoInicio(lista); no != NULL; no = ListaProdutoNoProximo(no))
    {
        Produto *prod = ListaProdutoNoObter(no);

        if (prod == NULL)
        {
            TraceLog(LOG_ERROR, "Erro ao contar estoque");
            abort();
        }

        tamanho_total += strlen(prod->nome);

        if (ListaProdutoNoProximo(no) != NULL)
            tamanho_total += strlen(separador);
    }

    char *out = malloc(tamanho_total + 1); // +1 para o \0

    if (out == NULL)
    {
        TraceLog(LOG_ERROR, "Erro ao alocar memória para a lista de jogos\n");
        abort();
    }

    *out = '\0';

    for (ListaProdutoNo no = ListaProdutoInicio(lista); no != NULL; no = ListaProdutoNoProximo(no))
    {
        Produto *prod = ListaProdutoNoObter(no);

        if (prod == NULL)
        {
            TraceLog(LOG_ERROR, "Erro ao contar estoque");
            abort();
        }

        strcat(out, prod->nome);

        if (ListaProdutoNoProximo(no) != NULL)
            strcat(out, separador);
    }

    ListaProdutoDescartar(&lista);

    return out;
}

// Cria uma string que contém o nome de todas as categorias existentes
static char *CriarOpcoesParaListaCategorias()
{
    ListaProduto lista = ListarProdutos();
    size_t tamanho_total = 0;
    char *separador = ";", *out = malloc(0);
    out[0] = '\0';

    for (ListaProdutoNo no = ListaProdutoInicio(lista); no != NULL; no = ListaProdutoNoProximo(no))
    {
        Produto *prod = ListaProdutoNoObter(no);

        if (prod == NULL)
        {
            TraceLog(LOG_ERROR, "Erro ao contar estoque");
            abort();
        }

        // Categoria já adicionada
        if (strstr(out, prod->categoria))
            continue;

        // Nova categoria
        out = realloc(out, strlen(out) + strlen(separador) + strlen(prod->categoria) + 1);
        strcat(out, prod->categoria);
        strcat(out, separador);
    }

    // Remove o último separador
    out[strlen(out) - strlen(separador)] = 0;

    ListaProdutoDescartar(&lista);

    return out;
}

void BotaoGerarRelatorioPressionado();

// Limite inferior da data das transações que entrarão no relatório
static bool dia_inicio_filtro_editando = false;
static int dia_inicio_filtro = 0;
static bool mes_inicio_filtro_editando = false;
static int mes_inicio_filtro = 0;
static bool ano_inicio_filtro_editando = false;
static int ano_inicio_filtro = -1;

// Limite superior da data das transações que entrarão no relatório
static bool dia_fim_filtro_editando = false;
static int dia_fim_filtro = 31;
static bool mes_fim_filtro_editando = false;
static int mes_fim_filtro = 11; // Dezembro
static bool ano_fim_filtro_editando = false;
static int ano_fim_filtro = -1;

// Tipo de relatório (por produto ou por categoria)
static bool seletor_tipo_relatorio_editando = false;
static int seletor_tipo_relatorio_idx_selecionado = 0;

// A categoria ou o produto selecionado
static char conteudo_botao_especificar_produto[128] = "Escolha um produto";
static char *seletor_dados_relatorio_opcoes = "";
static int id_produto_selecionado;
static char categoria_selecionada[128] = "";

// Placeholders para os dados do relatório
static char conteudo_caixa_texto_unidades_compradas[16] = "";
static char conteudo_caixa_texto_unidades_vendidas[16] = "";
static char conteudo_caixa_texto_balanco[16] = "";
static char conteudo_caixa_texto_custo_unitario[16] = "";
static char conteudo_caixa_texto_lucro_unitario[16] = "";
static char conteudo_caixa_texto_lucro_total[16] = "";

static bool janela_especificar_dados_ativa = false;
static int janela_especificar_dados_idx_rolagem = 0;
static int janela_especificar_dados_idx_selecionado = 0;
static char *titulos_janela_especificar_dados[] = {"Selecionar produto", "Selecionar categoria"};
static int ano_atual;

void RenderizarRotaRelatorios()
{
    switch (seletor_tipo_relatorio_idx_selecionado)
    {
    // Relatório por produto
    case 0:
        seletor_dados_relatorio_opcoes = CriarOpcoesParaListaProdutos();
        if (id_produto_selecionado == 0)
        {
            strcpy(conteudo_botao_especificar_produto, "Escolha um produto");
        }
        else
        {
            strcpy(conteudo_botao_especificar_produto, BuscarProdutoPorID(id_produto_selecionado).nome);
        }
        break;

    // Relatório por categoria
    case 1:
        seletor_dados_relatorio_opcoes = CriarOpcoesParaListaCategorias();

        if (strlen(categoria_selecionada) == 0)
        {
            strcpy(conteudo_botao_especificar_produto, "Escolha uma categoria");
        }
        else
        {
            strcpy(conteudo_botao_especificar_produto, categoria_selecionada);
        }
        break;
    }

    if (mes_inicio_filtro_editando || mes_fim_filtro_editando || seletor_tipo_relatorio_editando)
        GuiLock();

    if (janela_especificar_dados_ativa)
        GuiDisable();

    /**
     * As chamadas de função para renderizar os dados do relatório devem aparecer primeiro pois
     * os dropdowns têm prioridade de renderização (devem aparecer por cima).
     */

    GuiLabel((Rectangle){144, 136, 120, 24}, "Unidades compradas:");
    GuiTextBox((Rectangle){144, 152, 120, 24}, conteudo_caixa_texto_unidades_compradas, 128, false);
    GuiLabel((Rectangle){264, 152, 40, 24}, " un(s).");

    GuiLabel((Rectangle){352, 136, 104, 24}, "Unidades vendidas:");
    GuiTextBox((Rectangle){352, 152, 120, 24}, conteudo_caixa_texto_unidades_vendidas, 128, false);
    GuiLabel((Rectangle){472, 152, 40, 24}, " un(s).");

    GuiLabel((Rectangle){144, 192, 120, 24}, "Custo unitário médio:");
    GuiLabel((Rectangle){128, 208, 16, 24}, "R$");
    GuiTextBox((Rectangle){144, 208, 120, 24}, conteudo_caixa_texto_custo_unitario, 128, false);

    GuiLabel((Rectangle){352, 192, 120, 24}, "Lucro unitário médio");
    GuiLabel((Rectangle){336, 208, 16, 24}, "R$");
    GuiTextBox((Rectangle){352, 208, 120, 24}, conteudo_caixa_texto_lucro_unitario, 128, false);

    GuiLabel((Rectangle){144, 240, 120, 24}, "Balanço:");
    GuiTextBox((Rectangle){144, 256, 120, 24}, conteudo_caixa_texto_balanco, 128, false);
    GuiLabel((Rectangle){264, 256, 40, 24}, " un(s).");

    GuiLabel((Rectangle){352, 240, 120, 24}, "Lucro total:");
    GuiLabel((Rectangle){336, 256, 16, 24}, "R$");
    GuiTextBox((Rectangle){352, 256, 120, 24}, conteudo_caixa_texto_lucro_total, 128, false);

    GuiGroupBox((Rectangle){8, 48, 584, 80}, "Filtros");

    GuiLabel((Rectangle){40, 96, 112, 24}, "Gerar relatório por");
    GuiLabel((Rectangle){16, 56, 32, 24}, "Início");
    GuiLabel((Rectangle){336, 56, 24, 24}, "Fim");

    if (GuiButton((Rectangle){464, 96, 120, 24}, "Gerar"))
    {
        BotaoGerarRelatorioPressionado();
    }

    // Escolha do tipo de relatorio
    if (GuiDropdownBox((Rectangle){152, 96, 120, 24}, "Produto; Categoria", &seletor_tipo_relatorio_idx_selecionado,
                       seletor_tipo_relatorio_editando))
        seletor_tipo_relatorio_editando = !seletor_tipo_relatorio_editando;

    if (GuiButton((Rectangle){280, 96, 128, 24}, conteudo_botao_especificar_produto))
        janela_especificar_dados_ativa = !janela_especificar_dados_ativa;

    // Escolha data de inicio do relatorio
    ano_atual = ObterAnoAtual();
    if (ano_fim_filtro < 0)
        ano_fim_filtro = ano_atual;
    if (ano_inicio_filtro < 0)
        ano_inicio_filtro = ano_atual;

    if (GuiValueBox((Rectangle){48, 56, 48, 24}, "", &dia_inicio_filtro, 1,
                    DiasNoMes(mes_inicio_filtro, ano_inicio_filtro), dia_inicio_filtro_editando))
        dia_inicio_filtro_editando = !dia_inicio_filtro_editando;
    if (GuiDropdownBox((Rectangle){96, 56, 120, 24}, meses_do_ano, &mes_inicio_filtro, mes_inicio_filtro_editando))
        mes_inicio_filtro_editando = !mes_inicio_filtro_editando;
    if (GuiValueBox((Rectangle){216, 56, 56, 24}, "", &ano_inicio_filtro, 1900, ano_atual, ano_inicio_filtro_editando))
        ano_inicio_filtro_editando = !ano_inicio_filtro_editando;

    // Escolha data de fim do relatorio
    if (GuiValueBox((Rectangle){360, 56, 48, 24}, "", &dia_fim_filtro, 1, DiasNoMes(mes_fim_filtro, ano_fim_filtro),
                    dia_fim_filtro_editando))
        dia_fim_filtro_editando = !dia_fim_filtro_editando;
    if (GuiDropdownBox((Rectangle){408, 56, 120, 24}, meses_do_ano, &mes_fim_filtro, mes_fim_filtro_editando))
        mes_fim_filtro_editando = !mes_fim_filtro_editando;
    if (GuiValueBox((Rectangle){528, 56, 56, 24}, "", &ano_fim_filtro, 1900, ano_atual, ano_fim_filtro_editando))
        ano_fim_filtro_editando = !ano_fim_filtro_editando;

    if (janela_especificar_dados_ativa)
    {
        GuiEnable();

        janela_especificar_dados_ativa = !GuiWindowBox(
            (Rectangle){196, 122, 208, 168}, titulos_janela_especificar_dados[seletor_tipo_relatorio_idx_selecionado]);

        GuiListView((Rectangle){204, 154, 192, 96}, seletor_dados_relatorio_opcoes,
                    &janela_especificar_dados_idx_rolagem, &janela_especificar_dados_idx_selecionado);

        if (GuiButton((Rectangle){236, 258, 120, 24}, "Salvar"))
        {
            janela_especificar_dados_ativa = !janela_especificar_dados_ativa;
            SalvarEscolhaDadosRelatorio();
        }
    }

    GuiEnable();
    GuiUnlock();
}

void SalvarEscolhaDadosRelatorio()
{
    char *copia_opcoes = malloc(strlen(seletor_dados_relatorio_opcoes) + 1);
    strcpy(copia_opcoes, seletor_dados_relatorio_opcoes);

    // Pode conter um nome de produto ou uma categoria
    char *item = strtok(copia_opcoes, ";");
    int i = 0;
    while (item != NULL)
    {
        if (i == janela_especificar_dados_idx_selecionado)
        {
            switch (seletor_tipo_relatorio_idx_selecionado)
            {
            case 0: // Produto
                id_produto_selecionado = BuscarProdutoPorNome(item).id;
                break;
            case 1: // Categoria
                strcpy(categoria_selecionada, item);
                break;
            }

            break;
        }

        i++;
        item = strtok(NULL, ";");
    }
}

void BotaoGerarRelatorioPressionado()
{
    // Configurar intervalo de datas
    struct tm data_inicio_temp = {0, 0, 0, dia_inicio_filtro, mes_inicio_filtro, ano_inicio_filtro - 1900};
    struct tm data_fim_temp = {59, 59, 23, dia_fim_filtro, mes_fim_filtro, ano_fim_filtro - 1900};

    data_inicio = data_inicio_temp;
    data_fim = data_fim_temp;

    if (seletor_tipo_relatorio_idx_selecionado == 0) // Relatório por produto
    {
        if (id_produto_selecionado == 0)
        {
            TraceLog(LOG_ERROR, "Nenhum produto selecionado para o relatório.");
            return;
        }

        RelatorioProduto *relatorio = GerarRelatorioPorProduto(id_produto_selecionado);
        if (relatorio == NULL)
        {
            TraceLog(LOG_ERROR, "Erro ao gerar relatório de produto.");
            return;
        }

        snprintf(conteudo_caixa_texto_unidades_compradas, sizeof(conteudo_caixa_texto_unidades_compradas), "%d",
                 relatorio->total_entradas);
        snprintf(conteudo_caixa_texto_unidades_vendidas, sizeof(conteudo_caixa_texto_unidades_vendidas), "%d",
                 relatorio->total_saidas);
        snprintf(conteudo_caixa_texto_balanco, sizeof(conteudo_caixa_texto_balanco), "%d",
                 relatorio->total_entradas - relatorio->total_saidas);
        snprintf(conteudo_caixa_texto_custo_unitario, sizeof(conteudo_caixa_texto_custo_unitario), "%.2f",
                 relatorio->total_entradas ? relatorio->custo_total / relatorio->total_entradas : 0);
        snprintf(conteudo_caixa_texto_lucro_unitario, sizeof(conteudo_caixa_texto_lucro_unitario), "%.2f",
                 relatorio->total_saidas ? relatorio->valor_total_vendas / relatorio->total_saidas : 0);
        snprintf(conteudo_caixa_texto_lucro_total, sizeof(conteudo_caixa_texto_lucro_total), "%.2f",
                 relatorio->valor_total_vendas - relatorio->custo_total);

        free(relatorio);
    }
    else if (seletor_tipo_relatorio_idx_selecionado == 1) // Relatório por categoria
    {
        if (strlen(categoria_selecionada) == 0)
        {
            TraceLog(LOG_ERROR, "Nenhuma categoria selecionada para o relatório.");
            return;
        }

        RelatorioCategoria *relatorio = GerarRelatorioPorCategoria(categoria_selecionada);
        if (relatorio == NULL)
        {
            TraceLog(LOG_ERROR, "Erro ao gerar relatório de categoria.");
            return;
        }

        snprintf(conteudo_caixa_texto_unidades_compradas, sizeof(conteudo_caixa_texto_unidades_compradas), "%d",
                 relatorio->total_entradas);
        snprintf(conteudo_caixa_texto_unidades_vendidas, sizeof(conteudo_caixa_texto_unidades_vendidas), "%d",
                 relatorio->total_saidas);
        snprintf(conteudo_caixa_texto_balanco, sizeof(conteudo_caixa_texto_balanco), "%d",
                 relatorio->total_entradas - relatorio->total_saidas);
        snprintf(conteudo_caixa_texto_custo_unitario, sizeof(conteudo_caixa_texto_custo_unitario), "%.2f",
                 relatorio->total_entradas ? relatorio->custo_total / relatorio->total_entradas : 0);
        snprintf(conteudo_caixa_texto_lucro_unitario, sizeof(conteudo_caixa_texto_lucro_unitario), "%.2f",
                 relatorio->total_saidas ? relatorio->valor_total_vendas / relatorio->total_saidas : 0);
        snprintf(conteudo_caixa_texto_lucro_total, sizeof(conteudo_caixa_texto_lucro_total), "%.2f",
                 relatorio->valor_total_vendas - relatorio->custo_total);

        free(relatorio);
    }
}