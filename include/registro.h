#ifndef REGISTRO_H
#define REGISTRO_H

#include "cadastro.h"
#include "lista_contagem.h"
#include "rota.h"
#include <string.h>

#define ARQUIVO_REGISTRO "data/estoque.txt"
#define IDENTIFICADOR_ENTRADA 'E'
#define IDENTIFICADOR_SAIDA 'S'
#define FORMATO_LINHA_REGISTRO "%c %d %d %lf\n"

typedef struct ResultadoTransacao
{
    TipoResultado tipo;
    char *msg_erro;
} ResultadoTransacao;

ResultadoTransacao RegistrarEntradaProduto(int id_produto, int quantidade, double valor_unitario);
ResultadoTransacao RegistrarSaidaProduto(int id_produto, int quantidade, double valor_unitario);

ListaContagem ContarEstoque();

int ContarProduto(int id);

void RenderizarRotaEstoque();
void InicializarRotaEstoque();

#endif