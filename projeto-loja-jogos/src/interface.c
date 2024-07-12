#include "interface.h"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "registro.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>

#define NOME_PROGRAMA "Estoque de jogos"

typedef enum InterfaceRotas
{
    INTERFACE_ROTA_ESTOQUE,
} InterfaceRotas;

typedef struct ContextoRotaEstoque
{
    int ListaJogosScrollIndex;
    int ListaJogosAtiva;
    bool SpinnerQtdEditando;
    int SpinnerQtdValor;
    char ListaJogosTexto[1000];
} ContextoRotaEstoque;

void RenderizarRotaEstoque(void *);
void *InicializarContextoEstoque();

static void Button007(); // Button: Button007 logic
static void Button008(); // Button: Button008 logic
static void PopularListaJogos(char *);
static void PopularListaRotas(char *);

typedef struct Rota
{
    char *nome;
    void (*fnRenderizarRota)(void *);
    void *(*fnInicializarContexto)();
} Rota;

Rota lista_rotas[] = {
    {.nome = "Vender", .fnRenderizarRota = RenderizarRotaEstoque, .fnInicializarContexto = InicializarContextoEstoque}};

void RenderizarRotaEstoque(void *contexto)
{
    ContextoRotaEstoque *cx = (ContextoRotaEstoque *)contexto;

    GuiListView((Rectangle){8, 56, 384, 312}, cx->ListaJogosTexto, &cx->ListaJogosScrollIndex, &cx->ListaJogosAtiva);
    GuiGroupBox((Rectangle){400, 48, 192, 320}, "Detalhes");
    if (GuiButton((Rectangle){408, 336, 88, 24}, "Cancelar"))
        Button007();
    if (GuiButton((Rectangle){504, 336, 80, 24}, "Salvar"))
        Button008();
    if (GuiSpinner((Rectangle){472, 64, 112, 24}, "Quantidade ", &cx->SpinnerQtdValor, 0, 100, cx->SpinnerQtdEditando))
        cx->SpinnerQtdEditando = !cx->SpinnerQtdEditando;
    GuiLabel((Rectangle){8, 32, 120, 24}, "Jogos");
}

void *InicializarContextoEstoque()
{
    ContextoRotaEstoque *cx = malloc(sizeof(ContextoRotaEstoque));

    *cx = (ContextoRotaEstoque){
        .ListaJogosAtiva = false,
        .ListaJogosScrollIndex = 0,
        .ListaJogosTexto = "",
        .SpinnerQtdEditando = false,
        .SpinnerQtdValor = 0,
    };

    return cx;
}

void InicializarContextosRotas(void **cx_out)
{
    const int num_rotas = sizeof(lista_rotas) / sizeof(Rota);
    for (int i = 0; i < num_rotas; i++)
    {
        cx_out[i] = lista_rotas[i].fnInicializarContexto();
    }
}

char *rotas[] = {"Vender, Produtos, Estoque, Relatorios"};

void RenderizarRotaSelecionada(int rota_selecionada, void **contextos)
{
    lista_rotas[rota_selecionada].fnRenderizarRota(contextos[rota_selecionada]);
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

    InitWindow(screenWidth, screenHeight, "tela principal");
    SetTargetFPS(60);

    // tela principal: controls initialization
    //----------------------------------------------------------------------------------
    int RotaSelecionada = 0;
    char ListaRotasTexto[1000] = "";

    const int num_rotas = sizeof(lista_rotas) / sizeof(Rota);

    printf("A aplicacao possui %d rotas\n", num_rotas);

    PopularListaRotas(ListaRotasTexto);

    void **contextos = malloc(num_rotas * sizeof(void *));
    InicializarContextosRotas(contextos);
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

        GuiToggleGroup((Rectangle){168, 8, 104, 32}, ListaRotasTexto, &RotaSelecionada);
        GuiLabel((Rectangle){8, 8, 160, 32}, "Ola, {{Nome}}}");
        RenderizarRotaSelecionada(RotaSelecionada, contextos);

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
static void Button007()
{
    // TODO: Implement control logic
}
static void Button008()
{
    // TODO: Implement control logic
}

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

        sprintf(&string_jogo_id[5], "%.5d", cont->idProduto % 100000);

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
