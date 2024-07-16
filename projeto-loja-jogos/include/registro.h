#ifndef REGISTRO_H
#define REGISTRO_H

#include <string.h>
#include "lista.h"
#include "rota.h"

#define ARQUIVO_REGISTRO "registro_estoque.txt"
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

ResultadoTransacao RegistrarEntradaProduto(int idProduto, int quantidade);
ResultadoTransacao RegistrarSaidaProduto(int idProduto, int quantidade);

typedef struct Contagem
{
    int idProduto, quantidade;
} Contagem;

/**
 * Retorna uma Lista onde cada Item é do tipo Contagem
 */
Lista *ContarEstoque();

void RenderizarRotaEstoque();
void InicializarRotaEstoque();

#endif