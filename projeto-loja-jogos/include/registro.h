#ifndef REGISTRO_H
#define REGISTRO_H

#include <string.h>

typedef enum TipoResultado
{
    RESULTADO_SUCESSO,
    RESULTADO_FALHA
} TipoResultado;

typedef struct ResultadoTransacao
{
    TipoResultado tipo;
    char *msg_erro;
} ResultadoTransacao;

ResultadoTransacao registrar_entrada(int idProduto, int quantidade);
ResultadoTransacao registrar_saida(int idProduto, int quantidade);

typedef struct Contagem {
    int idProduto, quantidade;
} Contagem;

typedef struct ResultadoContagem
{
    int num_produtos;
    Contagem *produtos;
} ResultadoContagem;

ResultadoContagem *contar_estoque();

#endif