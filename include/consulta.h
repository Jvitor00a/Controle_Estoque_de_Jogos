#ifndef CONSULTA_H
#define CONSULTA_H

#include "produto.h"

Produto BuscarProdutoPorID(int id);

Produto BuscarProdutoPorNome(char *nome);

void ExibirProduto(Produto produto);

void RenderizarRotaDePesquisa();

#endif
