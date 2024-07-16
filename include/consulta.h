#ifndef CONSULTA_H
#define CONSULTA_H

#include "produto.h"

Produto BuscarProdutoPorId(int id);

Produto BuscarProdutoPorNome(char *nome);

void ExibirProduto(Produto produto);

void RenderizarRotaDePesquisa();

#endif
