#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>

typedef struct ItemLista
{
    void *dados;
    struct ItemLista *proximo;
    struct ItemLista *anterior;
} ItemLista;

typedef struct Lista
{
    int tamanho;
    ItemLista *primeiro;
    ItemLista *ultimo;
} Lista;

Lista *CriarLista();

ItemLista *ListaAcrescentar(Lista *lista, void *dados);

typedef bool (*CallbackFiltro)(ItemLista *);

ItemLista *ListaEncontrar(Lista *lista, CallbackFiltro filtro);

#endif
