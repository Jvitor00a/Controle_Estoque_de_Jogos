#include "interface.h"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "consulta.h"
#include "raygui.h"
#include "registro.h"
#include "relatorios.h"
#include "rota.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define NOME_PROGRAMA "Estoque de jogos"

typedef enum InterfaceRotas
{
    INTERFACE_ROTA_ESTOQUE,
} InterfaceRotas;

static void PopularListaRotas(char *);

Rota lista_rotas[] = {
    {.nome = "Estoque", .fnRenderizarRota = RenderizarRotaEstoque, .fnInicializarRota = InicializarRotaEstoque},
    {.nome = "Pesquisar", .fnRenderizarRota = RenderizarRotaDePesquisa},
    {.nome = "Relatorios", .fnRenderizarRota = RenderizarRotaRelatorios}};

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

    if (rota_selecionada.fnRenderizarRota != NULL)
        rota_selecionada.fnRenderizarRota();
}

void RodarInterface()
{
    int screenWidth = 600;
    int screenHeight = 380;

    InitWindow(screenWidth, screenHeight, "Games Warehouse");
    SetTargetFPS(60);

    int SelecaoRota = 0;
    char ListaRotasTexto[1000] = "";

    const int num_rotas = sizeof(lista_rotas) / sizeof(Rota);

    printf("A aplicacao possui %d rota(s)\n", num_rotas);

    PopularListaRotas(ListaRotasTexto);

    InicializarRotas();

    while (!WindowShouldClose()) // Detectar botao de fechar ou ESC clicados
    {
        BeginDrawing();

        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        // Cabeçalho da aplicação que mostra as rotas disponíveis
        //----------------------------------------------------------------------------------
        GuiToggleGroup((Rectangle){8, 8, (screenWidth - 16 - 2 * (num_rotas - 1)) / (float)num_rotas, 32},
                       ListaRotasTexto, &SelecaoRota);
        //----------------------------------------------------------------------------------

        RenderizarRotaSelecionada(SelecaoRota);

        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context
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
