#include <stdio.h>
#include "registro.h"

int main()
{
    Lista *resultado = ContarEstoque();

    ItemLista *atual = resultado->primeiro;

    printf("Encontrados %d produtos em estoque\n", resultado->tamanho);

    for (int i = 0; i < resultado->tamanho && atual != NULL; i++, atual = atual->proximo)
    {
        Contagem *cont = (Contagem *)atual->dados;
        printf("Produto: ID#%d Qtd: %d\n", cont->idProduto, cont->quantidade);
    }

    return 0;
}