#include "registro.h"
#include <stdio.h>
#include <stdlib.h>

#define ARQUIVO_REGISTRO "registro_estoque.txt"
#define PREFIXO_ENTRADA 'E'
#define PREFIXO_SAIDA 'S'
#define FORMATO_LINHA_REGISTRO "%c %d %d\n"

ResultadoTransacao registrar_entrada(int idProduto, int quantidade)
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "a");

    if (fd == NULL)
    {
        printf("Erro ao abrir o arquivo");
    }

    printf("Arquivo aberto com sucesso");

    fprintf(fd, FORMATO_LINHA_REGISTRO, PREFIXO_ENTRADA, idProduto, quantidade);

    fclose(fd);

    return (ResultadoTransacao){.tipo = RESULTADO_SUCESSO};
}

ResultadoTransacao registrar_saida(int idProduto, int quantidade)
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "a");

    if (fd == NULL)
    {
        printf("Erro ao abrir o arquivo");
    }

    fprintf(fd, FORMATO_LINHA_REGISTRO, PREFIXO_SAIDA, idProduto, quantidade);

    fclose(fd);

    return (ResultadoTransacao){.tipo = RESULTADO_SUCESSO};
}

int encontrar_contagem_id_produto(int idProduto, ResultadoContagem *contagem)
{
    for (int i = 0; i < contagem->num_produtos; i++)
    {
        if (contagem->produtos[i].idProduto == idProduto)
            return i;
    }

    return -1;
}

ResultadoContagem *adicionar_contagem(int idProduto, int quantidade, ResultadoContagem *contagem)
{
    contagem->num_produtos++;
    contagem->produtos = realloc(contagem->produtos, sizeof(Contagem) * contagem->num_produtos);
    contagem->produtos[contagem->num_produtos - 1] = (Contagem){.idProduto = idProduto, .quantidade = quantidade};
}

ResultadoContagem *contar_estoque()
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "r");

    if (fd == NULL)
    {
        printf("Erro ao abrir o arquivo");
    }

    ResultadoContagem *resultado = (ResultadoContagem *)malloc(sizeof(ResultadoContagem));
    int idx_prod;
    char buffer_leitura[100];

    while (1)
    {
        fgets(buffer_leitura, 100, fd);

        if (feof(fd))
            break;

        char tipo_registro;
        int id, qtd;

        sscanf(buffer_leitura, FORMATO_LINHA_REGISTRO, &tipo_registro, &id, &qtd);

        if (tipo_registro == PREFIXO_ENTRADA)
            qtd = qtd;
        else
            qtd = -qtd;

        idx_prod = encontrar_contagem_id_produto(id, resultado);

        if (idx_prod < 0)
        {
            adicionar_contagem(id, qtd, resultado);
        }
        else
        {
            resultado->produtos[idx_prod].quantidade += qtd;
        }
    }

    return resultado;
}
