#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef struct 
{
    int pontos;
    int tiros;
    char armasdia[11];
    char armasnoite[6]; 
    int escudos;
    int onda;
    bool terminou;
} estado_t;

char lechar()
{
    fflush(stdout);
    char c;
    if (fread(&c, 1, 1, stdin) == 1) return c;
    return 0;
}

void inicializa_estado(estado_t *est)
{
    est->tiros = 0;
    est->escudos = 3;
    strcpy(est->armasdia, "0123456789n");
    strcpy(est->armasnoite, "02468n");
}

void joga_onda(estado_t *est){
    while(){
        processar_teclado(est);
        processar_tempo(est);
        apresenta(est);
    }
}

void joga_partida(estado_t *estado)
{
    while(){
        joga_onda();
    };
}

int main(){
    system("stty raw -echo min 0 time 1 opost");
    setvbuf(stdin, NULL, _IONBF, 0);
    estado_t estado;
    inicializa_tela();
    inicializa_estado(&estado);
    while (!estado.terminou);{
        joga_partida(&estado);
    }
    system("stty sane");
}