#include "registro.h"
#include "consulta.h"
#include "lista_contagem.h"
#include "lista_produto.h"
#include "raygui.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

ResultadoTransacao RegistrarEntradaProduto(int id_produto, int quantidade, double valor_unitario)
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "a");

    if (fd == NULL)
    {
        return (ResultadoTransacao){.tipo = RESULTADO_FALHA, .msg_erro = "Erro ao abrir o arquivo de estoque\n"};
    }

    time_t current_time = time(NULL);
    fprintf(fd, FORMATO_LINHA_REGISTRO, IDENTIFICADOR_ENTRADA, id_produto, quantidade, valor_unitario,
            (long)current_time);

    fclose(fd);

    printf("Entrada de %d unidade(s) do produto %d registrada com sucesso\n", quantidade, id_produto);

    return (ResultadoTransacao){.tipo = RESULTADO_SUCESSO};
}

ResultadoTransacao RegistrarSaidaProduto(int id_produto, int quantidade, double valor_unitario)
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "a");

    if (fd == NULL)
    {
        return (ResultadoTransacao){.tipo = RESULTADO_FALHA, .msg_erro = "Erro ao abrir o arquivo de estoque"};
    }

    time_t current_time = time(NULL);
    fprintf(fd, FORMATO_LINHA_REGISTRO, IDENTIFICADOR_SAIDA, id_produto, quantidade, valor_unitario,
            (long)current_time);

    fclose(fd);

    printf("Saída de %d unidade(s) do produto %d registrada com sucesso\n", quantidade, id_produto);

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
        long _timestamp;

        sscanf(buffer_leitura, FORMATO_LINHA_REGISTRO, &tipo_registro, &id, &qtd, &valor_unitario, &_timestamp);

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
            }
            else
            {
                printf("Nao é permitido retirar %d produtos de estoque com apenas %d disponíveis\n", qtd, emEstoque);
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
        return -1;
    }

    char buffer_leitura[100];
    int quantidade_total = 0;

    while (fgets(buffer_leitura, sizeof(buffer_leitura), fd) != NULL)
    {
        char tipo_registro;
        int id_produto, qtd;
        double valor_unitario;
        long _timestamp;

        sscanf(buffer_leitura, FORMATO_LINHA_REGISTRO, &tipo_registro, &id_produto, &qtd, &valor_unitario, &_timestamp);

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
        long timestamp;

        sscanf(buffer_leitura, FORMATO_LINHA_REGISTRO, &tipo_registro, &id_produto, &qtd, &valor_unitario, &timestamp);

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
        fprintf(temp_fd, FORMATO_LINHA_REGISTRO, tipo_registro, id_produto, qtd, valor_unitario, timestamp);
    }

    fclose(fd);
    fclose(temp_fd);
    free(estoque);

    // Substituir o arquivo original pelo arquivo temporário
    remove(ARQUIVO_REGISTRO);
    rename("temp.txt", ARQUIVO_REGISTRO);
}

// Variáveis para armazenar o estado da interface gráfica
int lista_jogos_indice_rolagem = 0, indice_jogo_selecionado = 0, indice_jogo_selecionado_anterior = -1,
    quantidade_jogo_selecionado = 0;
double valor_unitario_jogo_selecionado = 0;
bool editando_quantidade = false;
char *texto_lista_jogos = NULL;
ListaProduto lista_produtos;

static char *CriarListaProdutos()
{
    size_t tamanho_total = 0;
    char *separador = "; ";

    for (ListaProdutoNó nó = ListaProdutoInicio(lista_produtos); nó != NULL; nó = ListaProdutoNóProximo(nó))
    {
        Produto *prod = ListaProdutoNóObter(nó);

        if (prod == NULL)
        {
            TraceLog(LOG_ERROR, "Erro ao contar estoque");
            abort();
        }

        tamanho_total += strlen(prod->nome);

        if (ListaProdutoNóProximo(nó) != NULL)
            tamanho_total += strlen(separador); // +2 para "; "
    }

    char *out = malloc(tamanho_total + 1); // +1 para o \0

    if (out == NULL)
    {
        TraceLog(LOG_ERROR, "Erro ao alocar memória para a lista de jogos\n");
        abort();
    }

    *out = '\0';

    for (ListaProdutoNó nó = ListaProdutoInicio(lista_produtos); nó != NULL; nó = ListaProdutoNóProximo(nó))
    {
        Produto *prod = ListaProdutoNóObter(nó);

        if (prod == NULL)
        {
            TraceLog(LOG_ERROR, "Erro ao contar estoque");
            abort();
        }

        strcat(out, prod->nome);

        if (ListaProdutoNóProximo(nó) != NULL)
            strcat(out, separador);
    }

    return out;
}

void InicializarRotaEstoque()
{
    LimparEstoque();
}

static void BotaoCancelarClicado(); // Logica do botao de cancelar
static void BotaoSalvarClicado();   // Logica do botao de salvar alterações no estoque

void RenderizarRotaEstoque()
{
    lista_produtos = ListarProdutos();

    if (indice_jogo_selecionado < 0)
        indice_jogo_selecionado = 0;

    if (indice_jogo_selecionado != indice_jogo_selecionado_anterior) // Verifica se a posição foi alterada
    {
        ListaProdutoNó nó_selecionado = ListaProdutoObterEmPosicao(lista_produtos, indice_jogo_selecionado);

        if (nó_selecionado == NULL)
        {
            printf("Encontrado nó inválido na lista de jogos ao renderizar estoque\n");
            return;
        }

        Produto *produto_selecionado = ListaProdutoNóObter(nó_selecionado);

        if (produto_selecionado == NULL)
        {
            printf("Encontrado produto inválido na lista de jogos ao renderizar estoque\n");
            return;
        }

        valor_unitario_jogo_selecionado = produto_selecionado->valor_unitario;
        quantidade_jogo_selecionado = ContarProduto(produto_selecionado->id);
        indice_jogo_selecionado_anterior = indice_jogo_selecionado;
    }

    texto_lista_jogos = CriarListaProdutos();

    GuiLabel((Rectangle){8, 36, 120, 24}, "Jogos");
    GuiListView((Rectangle){8, 56, 384, 312}, texto_lista_jogos, &lista_jogos_indice_rolagem, &indice_jogo_selecionado);
    GuiGroupBox((Rectangle){400, 48, 192, 320}, "Detalhes");
    if (GuiSpinner((Rectangle){472, 64, 112, 24}, "Quantidade ", &quantidade_jogo_selecionado, 0, 1000000,
                   editando_quantidade))
    {
        editando_quantidade = !editando_quantidade;
    }
    if (GuiButton((Rectangle){408, 336, 88, 24}, "Cancelar"))
        BotaoCancelarClicado();
    if (GuiButton((Rectangle){504, 336, 80, 24}, "Salvar"))
        BotaoSalvarClicado();

    ListaProdutoDescartar(&lista_produtos);
}

static void BotaoCancelarClicado()
{
    // Resetar o controlador da quantidade para o valor original
    if (indice_jogo_selecionado >= 0)
    {
        ListaProdutoNó nó_item_selecionado = ListaProdutoObterEmPosicao(lista_produtos, indice_jogo_selecionado);

        if (nó_item_selecionado != NULL)
        {
            Produto *produto_selecionado = ListaProdutoNóObter(nó_item_selecionado);

            if (produto_selecionado == NULL)
            {
                printf("Encontrado nó inválido na posição %d\n", indice_jogo_selecionado);
                return;
            }

            quantidade_jogo_selecionado = ContarProduto(produto_selecionado->id);
            printf("Alterações canceladas para o produto #%s\n", produto_selecionado->nome);
        }
    }
}

static void BotaoSalvarClicado()
{
    // Salvar as mudanças no estoque
    if (indice_jogo_selecionado >= 0)
    {
        ListaProdutoNó nó_item_selecionado = ListaProdutoObterEmPosicao(lista_produtos, indice_jogo_selecionado);

        if (nó_item_selecionado == NULL)
        {
            printf("Não foi possível obter o produto na posição %d\n", indice_jogo_selecionado);
            return;
        }

        Produto *produto_selecionado = ListaProdutoNóObter(nó_item_selecionado);

        if (produto_selecionado == NULL)
        {
            printf("Encontrado nó inválido na posição %d\n", indice_jogo_selecionado);
            return;
        }

        int quantidade_anterior = ContarProduto(produto_selecionado->id);

        if (quantidade_jogo_selecionado == quantidade_anterior)
        {
            printf("Nenhuma alteração necessária\n");
            return;
        }

        int diferenca = quantidade_jogo_selecionado - quantidade_anterior;
        ResultadoTransacao resultado;
        if (diferenca > 0)
        {
            resultado =
                RegistrarEntradaProduto(produto_selecionado->id, diferenca, produto_selecionado->valor_unitario);
        }
        else
        {
            resultado = RegistrarSaidaProduto(produto_selecionado->id, -diferenca, produto_selecionado->valor_unitario);
        }
    }
}
