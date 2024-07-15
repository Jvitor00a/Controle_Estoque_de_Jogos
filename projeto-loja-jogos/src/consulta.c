#include <stdio.h>
#include <string.h>
#include "raygui.h"
#include "raylib.h"
#include "consulta.h"


Produto buscarProdutoPorID(int id) {
    FILE *file = fopen("data/produtos.txt", "r"); 
    Produto produto;
    Produto produtoNaoEncontrado = {0, "", "", 0.0, 0}; 

    if (file == NULL) { 
        printf("Erro ao abrir o arquivo de produtos.\n");
        return produtoNaoEncontrado;
    }

    
    while (fscanf(file, "%d %s %s %f %d", &produto.id, produto.nome, produto.categoria, &produto.preco, &produto.quantidade) != EOF) {
        if (produto.id == id) {
            fclose(file); 
            return produto;
        }
    }

    fclose(file); 
    return produtoNaoEncontrado;
}


Produto buscarProdutoPorNome(char* nome) {
    FILE *file = fopen("data/produtos.txt", "r"); 
    Produto produto;
    Produto produtoNaoEncontrado = {0, "", "", 0.0, 0}; 

    if (file == NULL) { 
        printf("Erro ao abrir o arquivo de produtos.\n");
        return produtoNaoEncontrado;
    }


    while (fscanf(file, "%d %s %s %f %d", &produto.id, produto.nome, produto.categoria, &produto.preco, &produto.quantidade) != EOF) {
        if (strcmp(produto.nome, nome) == 0) {
            fclose(file);
            return produto;
        }
    }

    fclose(file);
    return produtoNaoEncontrado;
}


void exibirProduto(Produto produto) {
    if (produto.id == 0) { 
        printf("Produto não encontrado.\n");
    } else {
        printf("ID: %d\n", produto.id);
        printf("Nome: %s\n", produto.nome);
        printf("Categoria: %s\n", produto.categoria);
        printf("Valor: %.2f\n", produto.preco);
        printf("Quantidade: %d\n", produto.quantidade);
    }
}

bool editandoCaixaPesquisa;
char *textoCaixaPesquisa;

void renderizarRotaDePesquisa(){
    // raygui: controls drawing
            //----------------------------------------------------------------------------------
            if (GuiTextBox((Rectangle){ 0, 40, 648, 24 }, &textoCaixaPesquisa, 128, editandoCaixaPesquisa)) editandoCaixaPesquisa = !editandoCaixaPesquisa;
            GuiGroupBox((Rectangle){ 0, 24, 648, 336 }, "Detalhes");
            GuiLabel((Rectangle){ 48, 64, 120, 24 }, "Id");
            GuiLabel((Rectangle){ 152, 64, 80, 24 }, "Nome");
            GuiLabel((Rectangle){ 288, 64, 80, 24 }, "Categoria");
            GuiLabel((Rectangle){ 424, 64, 64, 24 }, "        Valor");
            GuiLabel((Rectangle){ 544, 64, 80, 24 }, "Quantidade");
            GuiLabel((Rectangle){ 24, 96, 72, 40 }, "001");
            GuiLabel((Rectangle){ 128, 104, 80, 24 }, "Krai");
            GuiLabel((Rectangle){ 280, 104, 120, 24 }, "utensilios");
            GuiLabel((Rectangle){ 424, 104, 80, 24 }, "399,90");
            GuiLabel((Rectangle){ 552, 104, 88, 24 }, "100");
            //----------------------------------------------------------------------------------

}
