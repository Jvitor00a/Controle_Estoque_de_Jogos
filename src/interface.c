#include "interface.h"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "consulta.h"
#include "raygui.h"
#include "registro.h"
#include "rota.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define NOME_PROGRAMA "Estoque de jogos"

typedef enum InterfaceRotas
{
    INTERFACE_ROTA_ESTOQUE,
} InterfaceRotas;

typedef struct ContextoRotaEstoque
{
    int lista_jogos_indice_rolagem;
    int ListaJogosSelecionado;
    bool editando_quantidade;
    int quantidade_jogo_selecionado;
    char texto_lista_jogos[1000];
} ContextoRotaEstoque;

static void PopularListaJogos(char *);
static void PopularListaRotas(char *);

Rota lista_rotas[] = {
    {.nome = "Vender"},
    {.nome = "Produtos"},
    {.nome = "Estoque", .fnRenderizarRota = RenderizarRotaEstoque, .fnInicializarRota = InicializarRotaEstoque},
    {.nome = "Pesquisar", .fnRenderizarRota = RenderizarRotaDePesquisa},
    {.nome = "Relatorios"}};

const int num_rotas = sizeof(lista_rotas) / sizeof(Rota);

void InicializarRotas()
{
    for (int i = 0; i < num_rotas; i++)
    {
        if (lista_rotas[i].fnInicializarRota != NULL)
            lista_rotas[i].fnInicializarRota();
    }
}

void RenderizarRotaSelecionada(int selecionada)
{
    if (selecionada < 0 || selecionada >= num_rotas)
    {
        TraceLog(LOG_ERROR,
                 "Tentativa de renderizar rota #%d porem a aplicacao so possui "
                 "%d rotas registradas",
                 selecionada, num_rotas);
        return;
    }

    Rota rota_selecionada = lista_rotas[selecionada];

    // printf("Renderizando rota #%d: %s\n", selecionada, rota_selecionada.nome);

    if (rota_selecionada.fnRenderizarRota != NULL)
        rota_selecionada.fnRenderizarRota();
}

void RodarInterface()
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 600;
    int screenHeight = 380;

    InitWindow(screenWidth, screenHeight, "Games Warehouse");
    SetTargetFPS(60);

    // tela principal: controls initialization
    //----------------------------------------------------------------------------------
    int SelecaoRota = 0;
    char ListaRotasTexto[1000] = "";

    const int num_rotas = sizeof(lista_rotas) / sizeof(Rota);

    printf("A aplicacao possui %d rota(s)\n", num_rotas);

    PopularListaRotas(ListaRotasTexto);

    InicializarRotas();
    //----------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        // raygui: controls drawing
        //----------------------------------------------------------------------------------

        GuiToggleGroup((Rectangle){8, 8, (screenWidth - 16 - 2 * (num_rotas - 1)) / (float)num_rotas, 32},
                       ListaRotasTexto, &SelecaoRota);
        // GuiLabel((Rectangle){8, 8, 160, 32}, "Ola, {{Nome}}}");
        RenderizarRotaSelecionada(SelecaoRota);

        //----------------------------------------------------------------------------------

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
                   //--------------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------------
// Controls Functions Definitions (local)
//------------------------------------------------------------------------------------

void PopularListaJogos(char *out)
{
    Lista *lista = ContarEstoque();

    if (lista == NULL)
    {
        TraceLog(LOG_ERROR, "Erro ao contar estoque");
        abort();
    }

    char string_jogo_id[] = "JOGO#XXXXX";
    for (ItemLista *i = lista->primeiro; i != NULL; i = i->proximo)
    {
        Contagem *cont = (Contagem *)(i->dados);

        if (cont == NULL)
        {
            TraceLog(LOG_ERROR, "Erro ao contar estoque");
            abort();
        }

        sprintf(&string_jogo_id[5], "%.5d", cont->id_produto % 100000);

        int chars_escritos;

        if (i->proximo != NULL)
            chars_escritos = sprintf(out, "%s; ", string_jogo_id);
        else
            chars_escritos = sprintf(out, "%s", string_jogo_id);

        out += chars_escritos;
    }

    *out = 0;
}

void PopularListaRotas(char *out)
{
    const int num_rotas = sizeof(lista_rotas) / sizeof(Rota);

    for (int i = 0; i < num_rotas; i++)
    {
        if (i < num_rotas - 1)
            out += sprintf(out, "%s; ", lista_rotas[i].nome);
        else
            out += sprintf(out, "%s", lista_rotas[i].nome);
    }

    *out = 0;
}
