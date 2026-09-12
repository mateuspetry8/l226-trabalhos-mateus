#include "str.h"
#include <stdio.h>

int main()
{
    s_imprime(s_cria_número(10));
    s_imprime(s_cria_número(14320));
    s_imprime(s_cria_número(0));
    s_imprime(s_cria_número(1));
    s_imprime(s_cria_número(999));
        s_imprime(s_cria_número(9.34));

}

//gcc -g -o teste teste.c utf8.c str.c