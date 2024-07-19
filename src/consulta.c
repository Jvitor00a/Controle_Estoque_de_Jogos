#include "consulta.h"
#include "raygui.h"
#include "raylib.h"
#include "registro.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Produto BuscarProdutoPorID(int id)
{
    FILE *file = fopen("data/produtos.txt", "r");
    Produto produto;
    Produto produto_nao_encontrado = {0, "", "", 0.0};

    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo de produtos.\n");
        return produto_nao_encontrado;
    }

    while (fscanf(file, "%d %s %s %lf\n", &produto.id, produto.nome, produto.categoria, &produto.valor_unitario) != EOF)
    {
        if (produto.id == id)
        {
            fclose(file);
            return produto;
        }
    }

    fclose(file);
    return produto_nao_encontrado;
}

Produto BuscarProdutoPorNome(char *nome)
{
    FILE *file = fopen("data/produtos.txt", "r");
    Produto produto;
    Produto produto_nao_encontrado = {0, "", "", 0.0};

    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo de produtos.\n");
        return produto_nao_encontrado;
    }

    while (fscanf(file, "%d %s %s %lf\n", &produto.id, produto.nome, produto.categoria, &produto.valor_unitario) != EOF)
    {
        if (strcmp(produto.nome, nome) == 0)
        {
            fclose(file);
            return produto;
        }
    }

    fclose(file);
    return produto_nao_encontrado;
}

void ExibirProduto(Produto produto)
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
        printf("Valor: %.2lf\n", produto.valor_unitario);
        printf("Quantidade: %d\n", ContarProduto(produto.id));
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
static const char *etiqueta_id = "Id";
static const char *etiqueta_nome = "Nome";
static const char *etiqueta_categoria = "Categoria";
static const char *etiqueta_valor = "Valor";
static const char *etiqueta_quantidade = "Quantidade";

// Variáveis da interface
static char texto_resultado_busca[128] = "Digite o termo de busca na caixa de texto acima";
static bool editando_caixa_texto_pesquisa = false;
static char termo_busca[128] = "";
static bool produto_encontrado_com_sucesso = false;
// Variáveis do produto pesquisado
static char id_produto[128] = "001";
static char nome_produto[128] = "Nome do produto";
static char categoria_produto[128] = "Categoria1";
static char valor_produto[128] = "100.00";
static char qtd_produto[128] = "999";

void EditarCaixaTextoPesquisa();

void RenderizarRotaDePesquisa()
{

    if (GuiTextBox((Rectangle){8, 48, 584, 24}, termo_busca, 128, editando_caixa_texto_pesquisa))
    {
        EditarCaixaTextoPesquisa();
    }

    // Ícone lupa
    GuiLabel((Rectangle){568, 48, 584, 24}, "#42#");

    const int largura_tela = 584;
    const int largura_caractere_em_pixels = 6;
    const int tamanho_texto_resultado_busca = strlen(texto_resultado_busca) * largura_caractere_em_pixels;
    GuiLabel((Rectangle){(largura_tela - tamanho_texto_resultado_busca) / 2, 80, tamanho_texto_resultado_busca, 24},
             texto_resultado_busca);

    if (produto_encontrado_com_sucesso)
    {
        GuiLabel((Rectangle){240, 120, 16, 24}, etiqueta_id);
        GuiTextBox((Rectangle){256, 120, 120, 24}, id_produto, 128, false);

        GuiLabel((Rectangle){224, 168, 32, 24}, etiqueta_nome);
        GuiTextBox((Rectangle){256, 168, 120, 24}, nome_produto, 128, false);

        GuiLabel((Rectangle){200, 216, 56, 24}, etiqueta_categoria);
        GuiTextBox((Rectangle){256, 216, 120, 24}, categoria_produto, 128, false);

        GuiLabel((Rectangle){224, 264, 32, 24}, etiqueta_valor);
        GuiTextBox((Rectangle){256, 264, 120, 24}, valor_produto, 128, false);

        GuiLabel((Rectangle){192, 312, 64, 24}, etiqueta_quantidade);
        GuiTextBox((Rectangle){256, 312, 120, 24}, qtd_produto, 128, false);
    }
    //----------------------------------------------------------------------------------
}

// Lógica de busca
void EditarCaixaTextoPesquisa()
{
    editando_caixa_texto_pesquisa = !editando_caixa_texto_pesquisa;

    // Caso o usuário ainda estiver digitando ou caso o termo de busca
    // esteja vazio, não é necessário fazer uma consulta
    if (editando_caixa_texto_pesquisa || strlen(termo_busca) == 0)
        return;

    Produto produto_encontrado;
    if (isNumber(termo_busca))
    {
        int id = atoi(termo_busca);
        produto_encontrado = BuscarProdutoPorID(id);
    }
    else
    {
        produto_encontrado = BuscarProdutoPorNome(termo_busca);
    }

    if (produto_encontrado.id != 0)
    {
        snprintf(id_produto, sizeof(id_produto), "%d", produto_encontrado.id);
        snprintf(nome_produto, sizeof(nome_produto), "%s", produto_encontrado.nome);
        snprintf(categoria_produto, sizeof(categoria_produto), "%s", produto_encontrado.categoria);
        snprintf(valor_produto, sizeof(valor_produto), "%.2lf", produto_encontrado.valor_unitario);
        snprintf(qtd_produto, sizeof(qtd_produto), "%d", ContarProduto(produto_encontrado.id));
        strcpy(texto_resultado_busca, "Produto encontrado!");
        produto_encontrado_com_sucesso = true;
    }
    else
    {
        strcpy(texto_resultado_busca, "Produto não encontrado.");
        produto_encontrado_com_sucesso = false;
    }
}
