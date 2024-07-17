#include "registro.h"
#include "lista_contagem.h"
#include "lista_produto.h"
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

    fprintf(fd, FORMATO_LINHA_REGISTRO, IDENTIFICADOR_ENTRADA, id_produto, quantidade, valor_unitario);

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

    fprintf(fd, FORMATO_LINHA_REGISTRO, IDENTIFICADOR_SAIDA, id_produto, quantidade, valor_unitario);

    fclose(fd);

    return (ResultadoTransacao){.tipo = RESULTADO_SUCESSO};
}

int id_item_procurado = 0;
bool FiltrarPorId(ListaContagemNó nó)
{
    Contagem *cont = ListaContagemNóObter(nó);

    if (cont == NULL)
        return false;

    if (cont->id_produto == id_item_procurado)
        return true;

    return false;
}

ListaContagem ContarEstoque()
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "r");

    if (fd == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        abort();
    }

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
        ListaContagemNó nó = ListaContagemInicio(resultado);
        Contagem *c;

        while ((c = ListaContagemNóObter(nó)) != NULL)
        {
            if (c->id_produto == id)
                break;
            nó = ListaContagemNóProximo(nó);
        }

        if (c == NULL)
        {
            if (qtd < 0)
            {
                printf("Tentativa de realizar saída de produto sem estoque, ignorando\n");
                continue;
            }

            Contagem *cont = (Contagem *)malloc(sizeof(Contagem));
            *cont = (Contagem){.id_produto = id, .quantidade = qtd};
            ListaContagemEmpurrarTras(resultado, cont);
        }
        else
        {
            int emEstoque = c->quantidade;

            if ((qtd + emEstoque) >= 0)
            {
                c->quantidade += qtd;
                printf("Adicionadas %d unidades do produto #%d, %dun(s). ao total\n", qtd, c->id_produto,
                       c->quantidade);
            }
            else
            {
                printf("Nao e permitido retirar %d produtos de estoque com apenas %d disponiveis\n", qtd, emEstoque);
                continue;
            }
        }
    }

    fclose(fd);
    return resultado;
}

int ContarProduto(int id)
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "r");

    if (fd == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", ARQUIVO_REGISTRO);
        return -1; // Indicative of an error opening the file
    }

    char buffer_leitura[100];
    int quantidade_total = 0;

    while (fgets(buffer_leitura, sizeof(buffer_leitura), fd) != NULL)
    {
        char tipo_registro;
        int id_produto, qtd;
        double valor_unitario;

        sscanf(buffer_leitura, FORMATO_LINHA_REGISTRO, &tipo_registro, &id_produto, &qtd, &valor_unitario);

        if (id_produto == id)
        {
            if (tipo_registro == IDENTIFICADOR_ENTRADA)
            {
                quantidade_total += qtd;
            }
            else if (tipo_registro == IDENTIFICADOR_SAIDA)
            {
                quantidade_total -= qtd;
            }
        }
    }

    fclose(fd);

    return quantidade_total;
}

void LimparEstoque()
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "r");
    if (fd == NULL)
    {
        printf("Erro ao abrir o arquivo %s para leitura\n", ARQUIVO_REGISTRO);
        return;
    }

    FILE *temp_fd = fopen("temp.txt", "w");
    if (temp_fd == NULL)
    {
        printf("Erro ao abrir o arquivo temporário para escrita\n");
        fclose(fd);
        return;
    }

    char buffer_leitura[100];
    ListaProduto produtos = ListarProdutos();
    int tamanho_lista_produtos = ListaProdutoTamanho(produtos);
    int *estoque = (int *)calloc(tamanho_lista_produtos, sizeof(int));

    if (estoque == NULL)
    {
        printf("Erro ao alocar memória para o estoque\n");
        fclose(fd);
        fclose(temp_fd);
        return;
    }

    // Primeiro, contar o estoque atual
    while (fgets(buffer_leitura, sizeof(buffer_leitura), fd) != NULL)
    {
        char tipo_registro;
        int id_produto, qtd;
        double valor_unitario;

        sscanf(buffer_leitura, FORMATO_LINHA_REGISTRO, &tipo_registro, &id_produto, &qtd, &valor_unitario);

        if (id_produto > tamanho_lista_produtos)
        {
            continue; // Produto inválido
        }

        if (tipo_registro == IDENTIFICADOR_ENTRADA)
        {
            estoque[id_produto] += qtd;
        }
        else if (tipo_registro == IDENTIFICADOR_SAIDA)
        {
            if (estoque[id_produto] >= qtd)
            {
                estoque[id_produto] -= qtd;
            }
            else
            {
                continue; // Saída inválida
            }
        }

        // Escrever linha válida no arquivo temporário
        fprintf(temp_fd, FORMATO_LINHA_REGISTRO, tipo_registro, id_produto, qtd, valor_unitario);
    }

    fclose(fd);
    fclose(temp_fd);
    free(estoque);

    // Substituir o arquivo original pelo arquivo temporário
    remove(ARQUIVO_REGISTRO);
    rename("temp.txt", ARQUIVO_REGISTRO);
}

int lista_jogos_indice_rolagem = 0, indice_jogo_selecionado = 0, ultimo_indice_selecionado = -1,
    quantidade_jogo_selecionado = 0;
double valor_unitario_jogo_selecionado = 0;
bool editando_quantidade = false;
char *texto_lista_jogos = NULL;
ListaContagem resultado_contagem;

void PopularListaJogos(char **out)
{
    ListaProduto lista_produtos = ListarProdutos();

    size_t tamanho_total = 0;
    for (ListaProdutoNó nó = ListaProdutoInicio(lista_produtos); nó != NULL; nó = ListaProdutoNóProximo(nó))
    {
        Produto *prod = ListaProdutoNóObter(nó);

        if (prod == NULL)
        {
            TraceLog(LOG_ERROR, "Erro ao contar estoque");
            abort();
        }

        int tamanho_antigo = tamanho_total;
        tamanho_total += strlen(prod->nome);

        if (ListaProdutoNóProximo(nó) != NULL)
            tamanho_total += 2; // "; "

        *out = realloc(*out, tamanho_total + 1); // +1 para o \0

        if (*out == NULL)
        {
            TraceLog(LOG_ERROR, "Erro ao realocar memória");
            abort();
        }

        if (tamanho_antigo == 0)
            strcpy(*out, prod->nome);
        else
            strcat(*out, prod->nome);

        if (ListaProdutoNóProximo(nó) != NULL)
            strcat(*out, "; ");
    }

    if (*out != NULL)
        (*out)[tamanho_total] = '\0';
}

void InicializarRotaEstoque()
{
    LimparEstoque();
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
        ListaProdutoNó nó_selecionado = ListaProdutoObterEmPosicao(ListarProdutos(), indice_jogo_selecionado);

        if (nó_selecionado == NULL)
        {
            printf("Encontrado nó inválido na lista de jogos ao renderizar estoque");
            return;
        }

        Produto *produto_selecionado = ListaProdutoNóObter(nó_selecionado);

        if (produto_selecionado == NULL)
        {
            printf("Encontrado produto inválido na lista de jogos ao renderizar estoque");
            return;
        }

        valor_unitario_jogo_selecionado = produto_selecionado->valor_unitario;
        quantidade_jogo_selecionado = ContarProduto(produto_selecionado->id);
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
        ListaContagemNó nó_item_selecionado =
            ListaContagemObterEmPosicao(resultado_contagem, ultimo_indice_selecionado);

        if (nó_item_selecionado != NULL)
        {
            Contagem *c = ListaContagemNóObter(nó_item_selecionado);
            if (c == NULL)
            {
                printf("Encontrado nó inválido na posição %d", ultimo_indice_selecionado);
            }
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
        ListaContagemNó nó_item_selecionado =
            ListaContagemObterEmPosicao(resultado_contagem, ultimo_indice_selecionado);

        if (nó_item_selecionado != NULL)
        {
            Contagem *c = ListaContagemNóObter(nó_item_selecionado);
            if (c == NULL)
            {
                printf("Encontrado nó inválido na posição %d", ultimo_indice_selecionado);
            }
            int quantidade_anterior = c->quantidade;

            if (quantidade_jogo_selecionado != quantidade_anterior)
            {
                int diferenca = quantidade_jogo_selecionado - quantidade_anterior;
                if (diferenca > 0)
                {
                    RegistrarEntradaProduto(c->id_produto, diferenca, 0.0); // Valor unitário padrão para este exemplo
                }
                else
                {
                    RegistrarSaidaProduto(c->id_produto, -diferenca, 0.0); // Valor unitário padrão para este exemplo
                }
                c->quantidade = quantidade_jogo_selecionado;
                printf("Estoque atualizado para o produto #%d: %d unidades\n", c->id_produto, c->quantidade);
            }
        }
    }
}
