#include <stdio.h>
#include <string.h>
#include "raygui.h"
#include "raylib.h"
#include "consulta.h"

Produto buscarProdutoPorID(int id)
{
    FILE *file = fopen("data/produtos.txt", "r");
    Produto produto;
    Produto produtoNaoEncontrado = {0, "", "", 0.0, 0};

    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo de produtos.\n");
        return produtoNaoEncontrado;
    }

    while (fscanf(file, "%d %s %s %f %d", &produto.id, produto.nome, produto.categoria, &produto.preco, &produto.quantidade) != EOF)
    {
        if (produto.id == id)
        {
            fclose(file);
            return produto;
        }
    }

    fclose(file);
    return produtoNaoEncontrado;
}

Produto buscarProdutoPorNome(char *nome)
{
    FILE *file = fopen("data/produtos.txt", "r");
    Produto produto;
    Produto produtoNaoEncontrado = {0, "", "", 0.0, 0};

    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo de produtos.\n");
        return produtoNaoEncontrado;
    }

    while (fscanf(file, "%d %s %s %f %d", &produto.id, produto.nome, produto.categoria, &produto.preco, &produto.quantidade) != EOF)
    {
        if (strcmp(produto.nome, nome) == 0)
        {
            fclose(file);
            return produto;
        }
    }

    fclose(file);
    return produtoNaoEncontrado;
}

void exibirProduto(Produto produto)
{
    if (produto.id == 0)
    {
        printf("Produto não encontrado.\n");
    }
    else
    {
        printf("ID: %d\n", produto.id);
        printf("Nome: %s\n", produto.nome);
        printf("Categoria: %s\n", produto.categoria);
        printf("Valor: %.2f\n", produto.preco);
        printf("Quantidade: %d\n", produto.quantidade);
    }
}

bool editandoCaixaPesquisa;
char textoCaixaPesquisa[1000];

const char *EtiquetaId = "Id";
const char *EtiquetaNome = "Nome";
const char *EtiquetaCategoria = "Categoria";
const char *EtiquetaValor = "Valor";
const char *EtiquetaQtd = "Quantidade";

char TextoLabelPesquisando[128] = "Pesquisando....";
bool WindowBox000Active = true;
bool TextBox001EditMode = false;
char TextoCaixaPesquisa[128] = "Digite para pesquisar";
char IdProduto[128] = "001";
char NomeProduto[128] = "Nome do produto";
char CategoriaProduto[128] = "Categoria1";
char ValorProduto[128] = "100.00";
char QtdProduto[128] = "999";

void renderizarRotaDePesquisa()
{
    // raygui: controls drawing
    //----------------------------------------------------------------------------------

    if (GuiTextBox((Rectangle){8, 32, 584, 24}, TextoCaixaPesquisa, 128, TextBox001EditMode))
        TextBox001EditMode = !TextBox001EditMode;
    GuiLabel((Rectangle){272, 56, 120, 24}, TextoLabelPesquisando);
    GuiLabel((Rectangle){240, 96, 16, 24}, EtiquetaId);
    GuiLabel((Rectangle){224, 144, 32, 24}, EtiquetaNome);
    GuiLabel((Rectangle){200, 192, 56, 24}, EtiquetaCategoria);
    GuiLabel((Rectangle){224, 240, 32, 24}, EtiquetaValor);
    GuiLabel((Rectangle){192, 288, 64, 24}, EtiquetaQtd);
    GuiTextBox((Rectangle){256, 96, 120, 24}, IdProduto, 128, false);
    GuiTextBox((Rectangle){256, 144, 120, 24}, NomeProduto, 128, false);
    GuiTextBox((Rectangle){256, 192, 120, 24}, CategoriaProduto, 128, false);
    GuiTextBox((Rectangle){256, 240, 120, 24}, ValorProduto, 128, false);
    GuiTextBox((Rectangle){256, 288, 120, 24}, QtdProduto, 128, false);

    //----------------------------------------------------------------------------------
}
