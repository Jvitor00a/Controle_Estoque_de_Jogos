#ifndef CONSULTA_H
#define CONSULTA_H

#include "produto.h"

Produto buscarProdutoPorId(int id);

Produto buscarProdutoPorNome(char* nome);

void exibirProduto(Produto produto);

void renderizarRotaDePesquisa();

#endif

