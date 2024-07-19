#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARQUIVO_REGISTRO "estoque.txt"
#define PREFIXO_ENTRADA 'E'
#define PREFIXO_SAIDA 'S'
#define FORMATO_LINHA_REGISTRO "%c %d %d %lf %ld\n"
#define NUM_LINHAS 1000
#define NUM_PRODUTOS 20
#define QUANTIDADE_MAX 100
#define VALOR_UNIT_MAX 100

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
        int id_produto = rand() % NUM_PRODUTOS + 1;
        int quantidade = rand() % QUANTIDADE_MAX + 1;
        double valor_unitario = rand() % VALOR_UNIT_MAX + 1;
        
        // Obter o timestamp atual
        time_t agora = time(NULL);

        // Definir o intervalo de tempo em segundos (5 anos)
        time_t intervalo = 5 * 365 * 24 * 60 * 60; // 5 anos em segundos

        // Gerar um número aleatório dentro do intervalo de 5 anos
        time_t timestamp = agora - (rand() % intervalo);

        fprintf(fd, FORMATO_LINHA_REGISTRO, tipo_registro, id_produto, quantidade, valor_unitario, timestamp);
    }

    fclose(fd);
    printf("Arquivo 'registro_estoque.txt' gerado com sucesso.\n");
}

int main()
{
    gerar_registro_estoque();
    return 0;
}
