#ifndef CADASTRO_H
#define CADASTRO_H

#include "lista_produto.h"
#include <stdbool.h>

#define ARQUIVO_PRODUTOS "data/produtos.txt"

ListaProduto ListarProdutos();

typedef enum TipoResultado
{
    RESULTADO_SUCESSO,
    RESULTADO_FALHA
} TipoResultado;

typedef struct ResultadoCadastro
{
    TipoResultado tipo;
    char *msg_erro;
    Produto *produto;
} ResultadoCadastro;

ResultadoCadastro CadastrarProduto(char *nome, char *categoria, float);
bool InicializarCadastro();

#endif