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
static const char *etiqueta_nome = "Nome";
static const char *etiqueta_categoria = "Categoria";
static const char *etiqueta_preço_novo_jogo = "Preço (centavos) ";
static const char *texto_botão_salvar = "Salvar";
static const char *texto_novo_jogo = "Novo jogo";
static const char *texto_adicionar_novo_jogo = "+ Adicionar novo jogo";

static int lista_jogos_indice_rolagem = 0, indice_jogo_selecionado = 0, indice_jogo_selecionado_anterior = -1,
           quantidade_jogo_selecionado = 0;
static bool editando_quantidade = false;
static char *texto_lista_jogos = NULL;
static ListaProduto lista_produtos;

static bool mostrar_janela_novo_jogo = false;
static char nome_novo_jogo[128] = "";
static bool editando_nome_novo_jogo = false;
static char categoria_novo_jogo[128] = "";
static bool editando_categoria_novo_jogo = false;
static int valor_novo_jogo_centavos = 0;
static bool editando_valor_novo_jogo = false;

static const Vector2 anchor03 = {208, 120};

void InicializarRotaEstoque()
{
    LimparEstoque();
}

static char *CriarListaProdutos();
static void PreencherDadosProduto();
static void BotaoCancelarClicado(); // Logica do botao de cancelar
static void BotaoSalvarClicado();   // Logica do botao de salvar alterações no estoque
static void BotãoSalvarNovoJogoClicado();

void RenderizarRotaEstoque()
{
    lista_produtos = ListarProdutos();

    if (indice_jogo_selecionado < 0)
        indice_jogo_selecionado = 0;

    if (indice_jogo_selecionado != indice_jogo_selecionado_anterior) // Verifica se a posição foi alterada
    {
        PreencherDadosProduto();
    }

    texto_lista_jogos = CriarListaProdutos();

    if (mostrar_janela_novo_jogo)
        GuiDisable();

    if (GuiButton((Rectangle){8, 48, 384, 32}, texto_adicionar_novo_jogo))
        mostrar_janela_novo_jogo = true;

    GuiListView((Rectangle){8, 88, 384, 280}, texto_lista_jogos, &lista_jogos_indice_rolagem, &indice_jogo_selecionado);
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

    if (mostrar_janela_novo_jogo)
    {
        GuiEnable();

        mostrar_janela_novo_jogo =
            !GuiWindowBox((Rectangle){anchor03.x - 16, anchor03.y + 0, 232, 160}, texto_novo_jogo);
        if (GuiTextBox((Rectangle){anchor03.x + 88, anchor03.y + 32, 120, 24}, nome_novo_jogo, 128,
                       editando_nome_novo_jogo))
            editando_nome_novo_jogo = !editando_nome_novo_jogo;
        if (GuiTextBox((Rectangle){anchor03.x + 88, anchor03.y + 64, 120, 24}, categoria_novo_jogo, 128,
                       editando_categoria_novo_jogo))
            editando_categoria_novo_jogo = !editando_categoria_novo_jogo;
        if (GuiValueBox((Rectangle){anchor03.x + 88, anchor03.y + 96, 120, 24}, etiqueta_preço_novo_jogo,
                        &valor_novo_jogo_centavos, 0, 99999, editando_valor_novo_jogo))
            editando_valor_novo_jogo = !editando_valor_novo_jogo;
        GuiLabel((Rectangle){anchor03.x + 56, anchor03.y + 32, 32, 24}, etiqueta_nome);
        GuiLabel((Rectangle){anchor03.x + 32, anchor03.y + 64, 56, 24}, etiqueta_categoria);
        if (GuiButton((Rectangle){anchor03.x + 8, anchor03.y + 128, 200, 24}, texto_botão_salvar))
            BotãoSalvarNovoJogoClicado();
    }

    ListaProdutoDescartar(&lista_produtos);
    GuiEnable();
}

char *CriarListaProdutos()

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

void PreencherDadosProduto()
{
    ListaProdutoNó nó_selecionado = ListaProdutoObterEmPosicao(lista_produtos, indice_jogo_selecionado);

    // lista de produtos vazia
    if (nó_selecionado == NULL)
    {
        return;
    }

    Produto *produto_selecionado = ListaProdutoNóObter(nó_selecionado);

    if (produto_selecionado == NULL)
    {
        printf("Encontrado produto inválido na lista de jogos ao renderizar estoque\n");
        return;
    }

    quantidade_jogo_selecionado = ContarProduto(produto_selecionado->id);
    indice_jogo_selecionado_anterior = indice_jogo_selecionado;
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

static void BotãoSalvarNovoJogoClicado()
{
    if (strlen(nome_novo_jogo) == 0 || strlen(categoria_novo_jogo) == 0 || valor_novo_jogo_centavos <= 0)
    {
        TraceLog(LOG_WARNING, "Preencha todos os campos corretamente antes de salvar.");
        return;
    }

    // Converter centavos para valor unitário
    float valor_unitario = valor_novo_jogo_centavos / 100.0f;

    CadastrarProduto(nome_novo_jogo, categoria_novo_jogo, valor_unitario);

    mostrar_janela_novo_jogo = false;
}
