#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARQUIVO_REGISTRO "registro_estoque.txt"
#define PREFIXO_ENTRADA 'E'
#define PREFIXO_SAIDA 'S'
#define FORMATO_LINHA_REGISTRO "%c %d %d\n"
#define NUM_LINHAS 1000
#define NUM_PRODUTOS 10
#define QUANTIDADE_MAX 100

void gerar_registro_estoque()
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "w");

    if (fd == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        exit(1);
    }

    srand(time(NULL));

    for (int i = 0; i < NUM_LINHAS; i++)
    {
        char tipo_registro = (rand() % 2 == 0) ? PREFIXO_ENTRADA : PREFIXO_SAIDA;
        int idProduto = rand() % NUM_PRODUTOS + 1;
        int quantidade = rand() % QUANTIDADE_MAX + 1;

        fprintf(fd, FORMATO_LINHA_REGISTRO, tipo_registro, idProduto, quantidade);
    }

    fclose(fd);
    printf("Arquivo 'registro_estoque.txt' gerado com sucesso.\n");
}

int main()
{
    gerar_registro_estoque();
    return 0;
}
