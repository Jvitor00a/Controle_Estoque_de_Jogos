#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

ItemLista *CriarItemLista(void *dados, ItemLista *anterior)
{
    ItemLista *item = (ItemLista *)malloc(sizeof(ItemLista));

    if (item == NULL)
    {
        perror("Falha ao alocar item para ser acrescentado à Lista");
        abort();
    }

    *item = (ItemLista){.dados = dados, .anterior = anterior, .proximo = NULL};

    return item;
}

Lista *CriarLista()
{
    Lista *lista = (Lista *)malloc(sizeof(Lista));

    if (lista == NULL)
    {
        perror("Falha ao alocar Lista");
        abort();
    }

    *lista = (Lista){.tamanho = 0, .primeiro = NULL, .ultimo = NULL};

    return lista;
}

ItemLista *ListaAcrescentar(Lista *lista, void *dados)
{
    ItemLista *novoItem = CriarItemLista(dados, lista->ultimo);

    if (lista->primeiro == NULL || lista->ultimo == NULL)
    {
        lista->primeiro = novoItem;
        lista->ultimo = novoItem;
        lista->tamanho++;

        return lista->primeiro;
    }

    lista->ultimo->proximo = novoItem;
    lista->tamanho++;

    return lista->ultimo;
}

ItemLista *ListaEncontrar(Lista *lista, CallbackFiltro fnFiltragem)
{
    int i;
    ItemLista *atual;

    for (i = 0, atual = lista->primeiro; i < lista->tamanho, atual != NULL; i++, atual = atual->proximo)
    {
        if (fnFiltragem(atual))
            return atual;
    }

    return NULL;
}
