#include <stdio.h>
#include <stdlib.h>
#include "registro.h"

#define ARQUIVO_REGISTRO "registro_estoque.txt"
#define PREFIXO_ENTRADA 'E'
#define PREFIXO_SAIDA 'S'
#define FORMATO_LINHA_REGISTRO "%c %d %d\n"

ResultadoTransacao RegistrarEntradaProduto(int idProduto, int quantidade)
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "a");

    if (fd == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
    }

    printf("Arquivo aberto com sucesso\n");

    fprintf(fd, FORMATO_LINHA_REGISTRO, PREFIXO_ENTRADA, idProduto, quantidade);

    fclose(fd);

    return (ResultadoTransacao){.tipo = RESULTADO_SUCESSO};
}

ResultadoTransacao RegistrarSaidaProduto(int idProduto, int quantidade)
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "a");

    if (fd == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
    }

    fprintf(fd, FORMATO_LINHA_REGISTRO, PREFIXO_SAIDA, idProduto, quantidade);

    fclose(fd);

    return (ResultadoTransacao){.tipo = RESULTADO_SUCESSO};
}

int idItemProcurado = 0;
bool FiltrarPorId(ItemLista *item)
{
    Contagem *cont = (Contagem *)item->dados;

    if (cont->idProduto == idItemProcurado)
        return true;

    return false;
}

Lista *ContarEstoque()
{
    FILE *fd = fopen(ARQUIVO_REGISTRO, "r");

    if (fd == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        abort();
    }

    Lista *resultados = CriarLista();

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

        idItemProcurado = id;
        ItemLista *item = ListaEncontrar(resultados, FiltrarPorId);

        if (item == NULL)
        {
            Contagem *cont = (Contagem *)malloc(sizeof(Contagem));
            cont->idProduto = id;
            cont->quantidade = qtd;
            ListaAcrescentar(resultados, cont);
        }
        else
        {
            ((Contagem *)item->dados)->quantidade += qtd;
        }
    }

    return resultados;
}
