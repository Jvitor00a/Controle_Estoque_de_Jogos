#include <stdio.h>
#include <stdlib.h>
#include "registro.h"
#include "raylib.h"
#include "raygui.h"
#include "lista_contagem.h"

ResultadoTransacao RegistrarEntradaProduto(int idProduto, int quantidade)
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "a");

    if (fd == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
    }

    printf("Arquivo aberto com sucesso\n");

    fprintf(fd, FORMATO_LINHA_REGISTRO, IDENTIFICADOR_ENTRADA, idProduto, quantidade);

    fclose(fd);

    return (ResultadoTransacao){.tipo = RESULTADO_SUCESSO};
}

ResultadoTransacao RegistrarSaidaProduto(int idProduto, int quantidade)
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "a");

    if (fd == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
    }

    fprintf(fd, FORMATO_LINHA_REGISTRO, IDENTIFICADOR_SAIDA, idProduto, quantidade);

    fclose(fd);

    return (ResultadoTransacao){.tipo = RESULTADO_SUCESSO};
}

int idItemProcurado = 0;
bool FiltrarPorId(ItemLista *item)
{
    Contagem *cont = (Contagem *)item->dados;

    if (cont->idProduto == idItemProcurado)
        return true;

    return false;
}

Lista *ContarEstoque()
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "r");

    if (fd == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        abort();
    }

    Lista *resultados = CriarLista();

    list_Contagem result = list_Contagem_new(NULL, NULL);

    char buffer_leitura[100];

    while (1)
    {
        fgets(buffer_leitura, 100, fd);

        if (feof(fd))
            break;

        char tipo_registro;
        int id, qtd;
        double valor_unitario;

        sscanf(buffer_leitura, FORMATO_LINHA_REGISTRO, &tipo_registro, &id, &qtd, &valor_unitario);

        if (tipo_registro == IDENTIFICADOR_ENTRADA)
            qtd = qtd;
        else
            qtd = -qtd;

        idItemProcurado = id;
        ItemLista *item = ListaEncontrar(resultados, FiltrarPorId);

        list_Contagem_node node = list_Contagem_begin(result);
        Contagem *c;

        while ((c = list_Contagem_node_get(node)) != NULL)
        {
            if (c->idProduto == id)
                break;
            node = list_Contagem_node_next(node);
        }

        if (item == NULL)
        {
            if (qtd < 0)
            {
                printf("Tentativa de realizar saída de produto sem estoque, ignorando\n");
                continue;
            }

            Contagem *cont = (Contagem *)malloc(sizeof(Contagem));
            *cont = (Contagem){.idProduto = id, .quantidade = qtd};
            ListaAcrescentar(resultados, cont);
            list_Contagem_push_back(result, cont);
        }
        else
        {
            Contagem *jogo = ((Contagem *)item->dados);
            int emEstoque = jogo->quantidade;

            // A checagem a seguir é necessária pois "qtd" pode ser negativo (em caso de saída de produto)
            if ((qtd + emEstoque) >= 0)
            {
                jogo->quantidade += qtd;
                printf("Adicionadas %d unidades do produto #%d, %dun(s). ao total\n", qtd, jogo->idProduto, jogo->quantidade);
            }
            else
            {
                printf("Nao e permitido retirar %d produtos de estoque com apenas %d disponiveis\n", qtd, emEstoque);
                continue;
            }
        }
    }

    return resultados;
}

int ListaJogosScrollIndex = 0, PosicaoJogoSelecionado = 0, UltimaPosicaoSelecionada = -1, SpinnerQtdValor = 0;
bool SpinnerQtdEditando = false;
char *ListaJogosTexto = NULL;
Lista *resultadoUltimaContagem;

char string_jogo_id[] = "JOGO#XXXXX";
const int tamanho_string_jogo_id = sizeof(string_jogo_id) / sizeof(char);
const int max_jogos_lista = 100000;

void PopularListaJogos(char **out)
{
    size_t tamanho_total = 0;
    for (ItemLista *i = resultadoUltimaContagem->primeiro; i != NULL; i = i->proximo)
    {
        Contagem *cont = (Contagem *)(i->dados);

        if (cont == NULL)
        {
            TraceLog(LOG_ERROR, "Erro ao contar estoque");
            abort();
        }

        sprintf(&string_jogo_id[5], "%.5d", cont->idProduto % max_jogos_lista);

        int tamanho_antigo = tamanho_total;
        tamanho_total += tamanho_string_jogo_id;
        if (i->proximo != NULL)
            tamanho_total += 2; // "; "

        *out = realloc(*out, tamanho_total + 1); // +1 para o \0

        if (*out == NULL)
        {
            TraceLog(LOG_ERROR, "Erro ao realocar memória");
            abort();
        }

        if (tamanho_antigo == 0)
            strcpy(*out, string_jogo_id);
        else
            strcat(*out, string_jogo_id);

        if (i->proximo != NULL)
            strcat(*out, "; ");
    }

    if (*out != NULL)
        (*out)[tamanho_total] = '\0';
}

void InicializarRotaEstoque()
{
    resultadoUltimaContagem = ContarEstoque();
    ListaJogosTexto = NULL; // Initialize to NULL
    PopularListaJogos(&ListaJogosTexto);
}
static void BotaoCancelarClicado(); // Logica do botao de cancelar
static void BotaoSalvarClicado();   // Logica do botao de salvar alterações no estoque

void RenderizarRotaEstoque()
{
    if (PosicaoJogoSelecionado < 0)
        PosicaoJogoSelecionado = 0;

    if (PosicaoJogoSelecionado != UltimaPosicaoSelecionada) // Verifica se a posição foi alterada
    {
        UltimaPosicaoSelecionada = PosicaoJogoSelecionado;
        ItemLista *itemSelecionado = ListaPosicao(resultadoUltimaContagem, PosicaoJogoSelecionado);
        if (itemSelecionado != NULL)
        {
            Contagem *c = (Contagem *)itemSelecionado->dados;
            SpinnerQtdValor = c->quantidade;
            printf("Produto da posicao %d encontrado com sucesso\n", PosicaoJogoSelecionado);
        }
        else
        {
            printf("Nao foi possivel encontrar o produto na posicao %d\n", PosicaoJogoSelecionado);
        }
    }

    GuiLabel((Rectangle){8, 36, 120, 24}, "Jogos");
    GuiListView((Rectangle){8, 56, 384, 312}, ListaJogosTexto, &ListaJogosScrollIndex, &PosicaoJogoSelecionado);
    GuiGroupBox((Rectangle){400, 48, 192, 320}, "Detalhes");
    if (GuiButton((Rectangle){408, 336, 88, 24}, "Cancelar"))
        BotaoCancelarClicado();
    if (GuiButton((Rectangle){504, 336, 80, 24}, "Salvar"))
        BotaoSalvarClicado();
    if (GuiSpinner((Rectangle){472, 64, 112, 24}, "Quantidade ", &SpinnerQtdValor, 0, 1000000, SpinnerQtdEditando))
        SpinnerQtdEditando = !SpinnerQtdEditando;
}

static void BotaoCancelarClicado()
{
    // Reset spinner value to the original value
    if (UltimaPosicaoSelecionada >= 0)
    {
        ItemLista *itemSelecionado = ListaPosicao(resultadoUltimaContagem, UltimaPosicaoSelecionada);
        if (itemSelecionado != NULL)
        {
            Contagem *c = (Contagem *)itemSelecionado->dados;
            SpinnerQtdValor = c->quantidade;
            printf("Alterações canceladas para o produto #%d\n", c->idProduto);
        }
    }
}

static void BotaoSalvarClicado()
{
    // Save the changes to the inventory
    if (UltimaPosicaoSelecionada >= 0)
    {
        ItemLista *itemSelecionado = ListaPosicao(resultadoUltimaContagem, UltimaPosicaoSelecionada);
        if (itemSelecionado != NULL)
        {
            Contagem *c = (Contagem *)itemSelecionado->dados;
            int quantidadeAnterior = c->quantidade;

            if (SpinnerQtdValor != quantidadeAnterior)
            {
                int diferenca = SpinnerQtdValor - quantidadeAnterior;
                if (diferenca > 0)
                {
                    RegistrarEntradaProduto(c->idProduto, diferenca);
                }
                else
                {
                    RegistrarSaidaProduto(c->idProduto, -diferenca);
                }
                c->quantidade = SpinnerQtdValor;
                printf("Estoque atualizado para o produto #%d: %d unidades\n", c->idProduto, c->quantidade);
            }
        }
    }
}