#ifndef LISTA_CONTAGEM_H
#define LISTA_CONTAGEM_H

typedef struct Contagem
{
    int id_produto, quantidade;
} Contagem;

#define LIST_ELEMENT_TYPE Contagem
#define INCLUDE_LIST_TYPE_INL
#include "lista_generica.inl"

#endif