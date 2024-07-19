#include "cadastro.h"
#include "lista_produto.h"
#include "raygui.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Static list to hold products
static ListaProduto lista_produtos = NULL;

char *CodificarString(char *str);
void DecodificarString(char *str);

// Function to load products from file
static bool CarregarProdutos()
{
    lista_produtos = ListaProdutoNova(NULL, NULL);

    FILE *file = fopen(ARQUIVO_PRODUTOS, "r");
    if (file == NULL)
    {
        printf("Falha ao tentar carregar produtos: erro ao abrir arquivo %99s\n", ARQUIVO_PRODUTOS);
        return false;
    }

    Produto *produto = malloc(sizeof(Produto));

    if (produto == NULL)
    {
        printf("Falha ao tentar carregar produtos: erro ao alocar memória\n");
        return false;
    }

    while (fscanf(file, "%d %99s %29s %lf\n", &produto->id, produto->nome, produto->categoria,
                  &produto->valor_unitario) != EOF)
    {
        DecodificarString(produto->nome);
        DecodificarString(produto->categoria);
        ListaProdutoEmpurrarTras(lista_produtos, produto);
    }

    fclose(file);

    return true;
}

static void SalvarProdutos()
{
    FILE *file = fopen(ARQUIVO_PRODUTOS, "w");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", ARQUIVO_PRODUTOS);
        return;
    }

    for (ListaProdutoNo no = ListaProdutoInicio(lista_produtos); no != NULL; no = ListaProdutoNoProximo(no))
    {
        Produto *produto = ListaProdutoNoObter(no);

        if (produto == NULL)
        {
            printf("Erro ao ler dados da lista de produtos\n");
            return;
        }

        fprintf(file, "%d %s %s %lf\n", produto->id, CodificarString(produto->nome),
                CodificarString(produto->categoria), produto->valor_unitario);
    }

    fclose(file);
}

ListaProduto ListarProdutos()
{
    if (lista_produtos == NULL)
    {
        CarregarProdutos();
    }

    return ListaProdutoCopia(lista_produtos);
}

ResultadoCadastro CadastrarProduto(char *nome, char *categoria, float valor_unitario)
{
    ResultadoCadastro resultado;
    resultado.msg_erro = NULL;
    resultado.tipo = RESULTADO_SUCESSO;

    Produto *novo_produto = (Produto *)malloc(sizeof(Produto));
    if (novo_produto == NULL)
    {
        resultado.tipo = RESULTADO_FALHA;
        resultado.msg_erro = "Erro de alocação de memória.";
        return resultado;
    }

    novo_produto->id = ListaProdutoTamanho(lista_produtos) + 1;
    strncpy(novo_produto->nome, nome, TAMANHO_MAXIMO_NOME_PRODUTO - 1);
    novo_produto->nome[TAMANHO_MAXIMO_NOME_PRODUTO - 1] = '\0';
    strncpy(novo_produto->categoria, categoria, TAMANHO_MAXIMO_CATEGORIA_PRODUTO - 1);
    novo_produto->categoria[TAMANHO_MAXIMO_CATEGORIA_PRODUTO - 1] = '\0';
    novo_produto->valor_unitario = valor_unitario;

    ListaProdutoEmpurrarTras(lista_produtos, novo_produto);

    SalvarProdutos();

    resultado.produto = novo_produto;
    return resultado;
}

char *CodificarString(char *str)
{
    size_t len = 0;
    const char *temp = str;

    // Calculate the length of the encoded string
    while (*temp)
    {
        if (*temp == ' ')
            len += 3; // "%20" is 3 characters
        else
            len++;
        temp++;
    }

    char *out = malloc(len + 1); // Allocate memory for the encoded string
    if (out == NULL)
    {
        printf("Erro de alocação de memória.\n");
        return NULL;
    }

    char *dest = out;

    // Perform the encoding
    while (*str)
    {
        if (*str == ' ')
        {
            strcat(dest, "%20");
            dest += 3;
        }
        else
        {
            *dest = *str;
            dest++;
        }
        str++;
    }

    *dest = '\0'; // Null-terminate the encoded string
    return out;
}

void DecodificarString(char *str)
{
    char *src = str;
    char *dest = str;

    while (*src)
    {
        if (*src == '%' && *(src + 1) == '2' && *(src + 2) == '0')
        {
            *dest = ' ';
            src += 3;
        }
        else
        {
            *dest = *src;
            src++;
        }
        dest++;
    }

    *dest = '\0';
}