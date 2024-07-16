#ifndef REGISTRO_H
#define REGISTRO_H

#include "lista.h"
#include "rota.h"
#include <string.h>

#define ARQUIVO_REGISTRO "data/registro_estoque.txt"
#define IDENTIFICADOR_ENTRADA 'E'
#define IDENTIFICADOR_SAIDA 'S'
#define FORMATO_LINHA_REGISTRO "%c %d %d %lf\n"

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

ResultadoTransacao RegistrarEntradaProduto(int id_produto, int quantidade, double valor_unitario);
ResultadoTransacao RegistrarSaidaProduto(int id_produto, int quantidade, double valor_unitario);

typedef struct Contagem
{
    int id_produto, quantidade;
} Contagem;

/**
 * Retorna uma Lista onde cada Item é do tipo Contagem
 */
Lista *ContarEstoque();

void RenderizarRotaEstoque();
void InicializarRotaEstoque();

#endif