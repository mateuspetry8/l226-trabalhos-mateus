#include "str.h"
#include "calc.h"
#include <stdio.h>


void le_arquivo_e_calcula(void)
{
    Str conteudo = s_cria_de_arquivo("entrada.txt");
    Str quebra_linha = s_cria("\n");

    Lista linhas = l_cria_separando(conteudo, quebra_linha);
    Lista saida = l_cria();

    int n = l_tam(linhas);
    for (int i = 0; i < n; i++) {
        Str linha = l_dado_pos(linhas, i);
        Str resultado = calculadora(linha);
        l_insere_fim(saida, resultado);
    }

    Str texto_saida = s_cria_unindo(saida, quebra_linha);
    s_grava_arquivo(texto_saida, "saida.txt");

    // limpeza
    s_destroi(conteudo);
    s_destroi(quebra_linha);
    s_destroi(texto_saida);

    while (!l_vazia(linhas)) s_destroi(l_remove_inicio(linhas));
    l_destroi(linhas);

    while (!l_vazia(saida)) s_destroi(l_remove_inicio(saida));
    l_destroi(saida);
    calculadora_finaliza();
}

int main()
{
    le_arquivo_e_calcula();
}

//gcc -g -o teste teste.c utf8.c str.c lista.c calc.c -lm