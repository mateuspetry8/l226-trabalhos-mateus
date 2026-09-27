#include "calc.h"
#include "str.h"

#include <stdio.h>

static bool eh_letra(unichar c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '$';
}


static bool eh_ponto(unichar c) {
    return c == '.';
}

static bool eh_digito(unichar c) {
    return c >= '0' && c <= '9';
}

static bool eh_espaco(unichar c) {
    return c == ' ' || c == '\t' || c == '\n';
}

Lista tokeniza(Str txt)
{
    Lista tokens = l_cria();
    int tam = s_tam(txt);
    int i = 0;

    while (i < tam) {
        unichar c = s_ch(txt, i);

        if (eh_espaco(c)) {
            i++;
        }
        else if (eh_digito(c) || eh_ponto(c)) {
            int j = i;
            while (j < tam && (eh_digito(s_ch(txt, j)) || eh_ponto(s_ch(txt, j)))) {
                j++;
            }
            Str token = s_cria_substring(txt, i, j - i);
            l_insere_fim(tokens, token);
            i = j;
        }
        else if (eh_letra(c) || eh_sublinhado(c) || eh_cifrao(c)) {
            int j = i;
            while (j < tam && (eh_letra(s_ch(txt, j)) || eh_digito(s_ch(txt, j))
                              || eh_sublinhado(s_ch(txt, j)) || eh_cifrao(s_ch(txt, j)))) {
                j++;
            }
            Str token = s_cria_substring(txt, i, j - i);
            l_insere_fim(tokens, token);
            i = j;
        }
        else {
            Str token = s_cria_substring(txt, i, 1);
            l_insere_fim(tokens, token);
            i++;
        }
    }

    return tokens;
}

Str calculadora(Str expressão)
{

}