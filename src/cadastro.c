#include "cadastro.h"
#include "lista_produto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Static list to hold products
static ListaProduto lista_produtos = NULL;

// Function to load products from file
static void CarregarProdutos()
{
    lista_produtos = ListaProdutoNova(NULL, NULL);

    FILE *file = fopen(ARQUIVO_PRODUTOS, "r");
    if (file == NULL)
    {
        printf("Falha ao tentar carregar produtos: erro ao abrir arquivo %99s\n", ARQUIVO_PRODUTOS);
        return;
    }

    Produto *produto = malloc(sizeof(Produto));

    if (produto == NULL)
    {
        printf("Falha ao tentar carregar produtos: erro ao alocar memória\n");
        return;
    }

    while (fscanf(file, "%d %99s %29s %lf\n", &produto->id, produto->nome, produto->categoria,
                  &produto->valor_unitario) != EOF)
    {
        ListaProdutoEmpurrarTras(lista_produtos, produto);
    }

    fclose(file);
}

// Function to save products to file
static void SalvarProdutos()
{
    FILE *file = fopen(ARQUIVO_PRODUTOS, "w");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", ARQUIVO_PRODUTOS);
        return;
    }

    for (ListaProdutoNó nó = ListaProdutoInicio(lista_produtos); nó != ListaProdutoFim(lista_produtos);
         nó = ListaProdutoNóProximo(nó))
    {
        Produto *produto = ListaProdutoNóObter(nó);

        if (produto == NULL)
        {
            printf("Erro ao ler dados da lista de produtos");
            return;
        }

        fprintf(file, "%d %s %s %lf\n", produto->id, produto->nome, produto->categoria, produto->valor_unitario);
    }

    fclose(file);
}

ListaProduto ListarProdutos()
{
    CarregarProdutos(); // Ensure products are loaded from file
    return lista_produtos;
}

ResultadoCadastro CadastrarProduto(char *nome, char *categoria, float valor_unitario)
{
    ResultadoCadastro resultado;
    resultado.msg_erro = NULL;
    resultado.tipo = RESULTADO_SUCESSO;

    // Allocate memory for new product
    Produto *novo_produto = (Produto *)malloc(sizeof(Produto));
    if (novo_produto == NULL)
    {
        resultado.tipo = RESULTADO_FALHA;
        resultado.msg_erro = "Erro de alocação de memória.";
        return resultado;
    }

    // Set product fields
    novo_produto->id = ListaProdutoTamanho(lista_produtos) + 1; // Assuming IDs are assigned sequentially
    strncpy(novo_produto->nome, nome, TAMANHO_MAXIMO_NOME_PRODUTO - 1);
    novo_produto->nome[TAMANHO_MAXIMO_NOME_PRODUTO - 1] = '\0';
    strncpy(novo_produto->categoria, categoria, TAMANHO_MAXIMO_CATEGORIA_PRODUTO - 1);
    novo_produto->categoria[TAMANHO_MAXIMO_CATEGORIA_PRODUTO - 1] = '\0';
    novo_produto->valor_unitario = valor_unitario;

    // Add to list
    ListaProdutoEmpurrarTras(lista_produtos, novo_produto);

    // Save to file
    SalvarProdutos();

    resultado.produto = novo_produto;
    return resultado;
}
