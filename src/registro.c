#include "registro.h"
#include "lista_contagem.h"
#include "raygui.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

// TODO: Adicionar valor unitario às funções de registrar entradas e saídas
// TOOD: Adicionar funcionalidade do usuario especificar valor unitario em cada
// entrada/saida

ResultadoTransacao RegistrarEntradaProduto(int id_produto, int quantidade, double valor_unitario)
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "a");

    if (fd == NULL)
    {
        return (ResultadoTransacao){.tipo = RESULTADO_FALHA, .msg_erro = "Erro ao abrir o arquivo de estoque"};
    }

    printf("Arquivo aberto com sucesso\n");

    fprintf(fd, FORMATO_LINHA_REGISTRO, IDENTIFICADOR_ENTRADA, id_produto, quantidade);

    fclose(fd);

    return (ResultadoTransacao){.tipo = RESULTADO_SUCESSO};
}

ResultadoTransacao RegistrarSaidaProduto(int id_produto, int quantidade, double valor_unitario)
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "a");

    if (fd == NULL)
    {
        return (ResultadoTransacao){.tipo = RESULTADO_FALHA, .msg_erro = "Erro ao abrir o arquivo de estoque"};
    }

    fprintf(fd, FORMATO_LINHA_REGISTRO, IDENTIFICADOR_SAIDA, id_produto, quantidade);

    fclose(fd);

    return (ResultadoTransacao){.tipo = RESULTADO_SUCESSO};
}

int id_item_procurado = 0;
bool FiltrarPorId(ItemLista *item)
{
    Contagem *cont = (Contagem *)item->dados;

    if (cont->id_produto == id_item_procurado)
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

    ListaContagem resultado = ListaContagemNova(NULL, NULL);

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

        id_item_procurado = id;
        ItemLista *item = ListaEncontrar(resultados, FiltrarPorId);

        ListaContagemNó nó = ListaContagemInicio(resultado);
        Contagem *c;

        while ((c = ListaContagemNóObter(nó)) != NULL)
        {
            if (c->id_produto == id)
                break;
            nó = ListaContagemNóProximo(nó);
        }

        if (item == NULL)
        {
            if (qtd < 0)
            {
                printf("Tentativa de realizar saída de produto sem estoque, "
                       "ignorando\n");
                continue;
            }

            Contagem *cont = (Contagem *)malloc(sizeof(Contagem));
            *cont = (Contagem){.id_produto = id, .quantidade = qtd};
            ListaAcrescentar(resultados, cont);
            ListaContagemEmpurrarTras(resultado, cont);
        }
        else
        {
            Contagem *jogo = ((Contagem *)item->dados);
            int emEstoque = jogo->quantidade;

            // A checagem a seguir é necessária pois "qtd" pode ser negativo (em
            // caso de saída de produto)
            if ((qtd + emEstoque) >= 0)
            {
                jogo->quantidade += qtd;
                printf("Adicionadas %d unidades do produto #%d, %dun(s). ao "
                       "total\n",
                       qtd, jogo->id_produto, jogo->quantidade);
            }
            else
            {
                printf("Nao e permitido retirar %d produtos de estoque com "
                       "apenas %d disponiveis\n",
                       qtd, emEstoque);
                continue;
            }
        }
    }

    return resultados;
}

int lista_jogos_indice_rolagem = 0, indice_jogo_selecionado = 0, ultimo_indice_selecionado = -1,
    quantidade_jogo_selecionado = 0;
bool editando_quantidade = false;
char *texto_lista_jogos = NULL;
Lista *resultado_contagem;

char string_jogo_id[] = "JOGO#XXXXX";
const int tamanho_string_jogo_id = sizeof(string_jogo_id) / sizeof(char);
const int max_jogos_lista = 100000;

void PopularListaJogos(char **out)
{
    size_t tamanho_total = 0;
    for (ItemLista *i = resultado_contagem->primeiro; i != NULL; i = i->proximo)
    {
        Contagem *cont = (Contagem *)(i->dados);

        if (cont == NULL)
        {
            TraceLog(LOG_ERROR, "Erro ao contar estoque");
            abort();
        }

        sprintf(&string_jogo_id[5], "%.5d", cont->id_produto % max_jogos_lista);

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
    resultado_contagem = ContarEstoque();
    texto_lista_jogos = NULL; // Initialize to NULL
    PopularListaJogos(&texto_lista_jogos);
}
static void BotaoCancelarClicado(); // Logica do botao de cancelar
static void BotaoSalvarClicado();   // Logica do botao de salvar alterações no estoque

void RenderizarRotaEstoque()
{
    if (indice_jogo_selecionado < 0)
        indice_jogo_selecionado = 0;

    if (indice_jogo_selecionado != ultimo_indice_selecionado) // Verifica se a posição foi alterada
    {
        ultimo_indice_selecionado = indice_jogo_selecionado;
        ItemLista *item_selecionado = ListaPosicao(resultado_contagem, indice_jogo_selecionado);
        if (item_selecionado != NULL)
        {
            Contagem *c = (Contagem *)item_selecionado->dados;
            quantidade_jogo_selecionado = c->quantidade;
            printf("Produto da posicao %d encontrado com sucesso\n", indice_jogo_selecionado);
        }
        else
        {
            printf("Nao foi possivel encontrar o produto na posicao %d\n", indice_jogo_selecionado);
        }
    }

    GuiLabel((Rectangle){8, 36, 120, 24}, "Jogos");
    GuiListView((Rectangle){8, 56, 384, 312}, texto_lista_jogos, &lista_jogos_indice_rolagem, &indice_jogo_selecionado);
    GuiGroupBox((Rectangle){400, 48, 192, 320}, "Detalhes");
    if (GuiButton((Rectangle){408, 336, 88, 24}, "Cancelar"))
        BotaoCancelarClicado();
    if (GuiButton((Rectangle){504, 336, 80, 24}, "Salvar"))
        BotaoSalvarClicado();
    if (GuiSpinner((Rectangle){472, 64, 112, 24}, "Quantidade ", &quantidade_jogo_selecionado, 0, 1000000,
                   editando_quantidade))
        editando_quantidade = !editando_quantidade;
}

static void BotaoCancelarClicado()
{
    // Reset spinner value to the original value
    if (ultimo_indice_selecionado >= 0)
    {
        ItemLista *item_selecionado = ListaPosicao(resultado_contagem, ultimo_indice_selecionado);
        if (item_selecionado != NULL)
        {
            Contagem *c = (Contagem *)item_selecionado->dados;
            quantidade_jogo_selecionado = c->quantidade;
            printf("Alterações canceladas para o produto #%d\n", c->id_produto);
        }
    }
}

static void BotaoSalvarClicado()
{
    // Save the changes to the inventory
    if (ultimo_indice_selecionado >= 0)
    {
        ItemLista *item_selecionado = ListaPosicao(resultado_contagem, ultimo_indice_selecionado);
        if (item_selecionado != NULL)
        {
            Contagem *c = (Contagem *)item_selecionado->dados;
            int quantidade_anterior = c->quantidade;

            if (quantidade_jogo_selecionado != quantidade_anterior)
            {
                int diferenca = quantidade_jogo_selecionado - quantidade_anterior;
                if (diferenca > 0)
                {
                    // RegistrarEntradaProduto(c->id_produto, diferenca);
                }
                else
                {
                    // RegistrarSaidaProduto(c->id_produto, -diferenca);
                }
                c->quantidade = quantidade_jogo_selecionado;
                printf("Estoque atualizado para o produto #%d: %d unidades\n", c->id_produto, c->quantidade);
            }
        }
    }
}