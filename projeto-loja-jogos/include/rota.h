#ifndef ROTA_H
#define ROTA_H

typedef struct Rota
{
    char *nome;
    void (*fnRenderizarRota)();
    void (*fnInicializarRota)();
} Rota;

#endif