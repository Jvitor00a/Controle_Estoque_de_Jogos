#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "raygui.h"
#include "raylib.h"
#include "consulta.h"

Produto buscarProdutoPorID(int id)
{
    printf("Pesquisando produto %d\n", id);

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

// Function to check if a string is a number
bool isNumber(const char *str)
{
    while (*str)
    {
        if (!isdigit(*str))
        {
            return false;
        }
        str++;
    }
    return true;
}

// Textos constantes parte da interface
const char *EtiquetaId = "Id";
const char *EtiquetaNome = "Nome";
const char *EtiquetaCategoria = "Categoria";
const char *EtiquetaValor = "Valor";
const char *EtiquetaQtd = "Quantidade";

// Variáveis da interface
char TextoLabelPesquisando[128] = "Digite o termo de busca na caixa de texto acima";
bool EditandoCaixaTextoPesquisa = false;
char TextoCaixaPesquisa[128] = "";
bool ProdutoEncontradoComSucesso = false;
// Variáveis do produto pesquisado
char IdProduto[128] = "001";
char NomeProduto[128] = "Nome do produto";
char CategoriaProduto[128] = "Categoria1";
char ValorProduto[128] = "100.00";
char QtdProduto[128] = "999";

void renderizarRotaDePesquisa()
{
    // raygui: controls drawing
    //----------------------------------------------------------------------------------

    if (GuiTextBox((Rectangle){8, 48, 584, 24}, TextoCaixaPesquisa, 128, EditandoCaixaTextoPesquisa))
    {
        EditandoCaixaTextoPesquisa = !EditandoCaixaTextoPesquisa;

        // Search logic
        Produto produtoEncontrado;
        if (isNumber(TextoCaixaPesquisa))
        {
            int id = atoi(TextoCaixaPesquisa);
            produtoEncontrado = buscarProdutoPorID(id);
        }
        else
        {
            produtoEncontrado = buscarProdutoPorNome(TextoCaixaPesquisa);
        }

        if (produtoEncontrado.id != 0)
        {
            snprintf(IdProduto, sizeof(IdProduto), "%d", produtoEncontrado.id);
            snprintf(NomeProduto, sizeof(NomeProduto), "%s", produtoEncontrado.nome);
            snprintf(CategoriaProduto, sizeof(CategoriaProduto), "%s", produtoEncontrado.categoria);
            snprintf(ValorProduto, sizeof(ValorProduto), "%.2f", produtoEncontrado.preco);
            snprintf(QtdProduto, sizeof(QtdProduto), "%d", produtoEncontrado.quantidade);
            strcpy(TextoLabelPesquisando, "Produto encontrado!");
            ProdutoEncontradoComSucesso = true;
        }
        else
        {
            strcpy(TextoLabelPesquisando, "Produto não encontrado.");
            ProdutoEncontradoComSucesso = false;
        }
    }
    // Ícone lupa
    GuiLabel((Rectangle){568, 48, 584, 24}, "#42#");

    const int screenWidth = 584;
    const int tamanhoLabelPesquisando = strlen(TextoLabelPesquisando) * 6;
    GuiLabel((Rectangle){(screenWidth - tamanhoLabelPesquisando) / 2, 80, tamanhoLabelPesquisando, 24}, TextoLabelPesquisando);

    if (ProdutoEncontradoComSucesso)
    {
        GuiLabel((Rectangle){240, 120, 16, 24}, EtiquetaId);
        GuiTextBox((Rectangle){256, 120, 120, 24}, IdProduto, 128, false);

        GuiLabel((Rectangle){224, 168, 32, 24}, EtiquetaNome);
        GuiTextBox((Rectangle){256, 168, 120, 24}, NomeProduto, 128, false);

        GuiLabel((Rectangle){200, 216, 56, 24}, EtiquetaCategoria);
        GuiTextBox((Rectangle){256, 216, 120, 24}, CategoriaProduto, 128, false);

        GuiLabel((Rectangle){224, 264, 32, 24}, EtiquetaValor);
        GuiTextBox((Rectangle){256, 264, 120, 24}, ValorProduto, 128, false);

        GuiLabel((Rectangle){192, 312, 64, 24}, EtiquetaQtd);
        GuiTextBox((Rectangle){256, 312, 120, 24}, QtdProduto, 128, false);
    }
    //----------------------------------------------------------------------------------
}
