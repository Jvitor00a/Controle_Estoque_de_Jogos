#include <stdio.h>
#include "registro.h"

int main()
{
    ResultadoContagem *resultado = contar_estoque();

    for (int i = 0; i < resultado->num_produtos; i++)
    {
        printf("Produto: ID#%d Qtd: %d\n", resultado->produtos[i].idProduto, resultado->produtos[i].quantidade);
    }

    return 0;
}