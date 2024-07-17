#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Estrutura para representar um produto
typedef struct
{
    int id;
    char nome[50];
    char categoria[50];
    double valor_unitario;
} Produto;

// Função para gerar um nome de produto aleatório
void GerarNomeProduto(char *nome)
{
    const char *nomes[] = {"ProdutoA", "ProdutoB", "ProdutoC", "ProdutoD", "ProdutoE"};
    int indice = rand() % 5;
    sprintf(nome, "%s%d", nomes[indice], rand() % 100);
}

// Função para gerar uma categoria de produto aleatória
void GerarCategoriaProduto(char *categoria)
{
    const char *categorias[] = {"Categoria1", "Categoria2", "Categoria3", "Categoria4", "Categoria5"};
    int indice = rand() % 5;
    sprintf(categoria, "%s", categorias[indice]);
}

// Função principal para gerar a lista de produtos
int main()
{
    FILE *file = fopen("data/produtos.txt", "w");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return 1;
    }

    srand(time(NULL)); // Inicializa o gerador de números aleatórios

    for (int i = 1; i <= 20; i++)
    {
        Produto produto;
        produto.id = i;
        GerarNomeProduto(produto.nome);
        GerarCategoriaProduto(produto.categoria);
        produto.valor_unitario = (rand() % 10000) / 100.0; // Gera um preço aleatório entre 0.00 e 99.99

        fprintf(file, "%d %s %s %.2lf\n", produto.id, produto.nome, produto.categoria, produto.valor_unitario);
    }

    fclose(file);
    printf("Lista de produtos gerada com sucesso!\n");
    return 0;
}
