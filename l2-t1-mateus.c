#include <stdio.h>

typedef struct 
{
    int pontos;
    int tiros;
    char armasdia[11];
    char armasnoite[6]; 
    int escudos;
    int modo;
} Sistema;

typedef struct
{
    dia;
    noite;
}estado_t;

void joga_onda(estado_t *est){
    laco(){
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
    estado_t estado;
    inicializa_tela();
    inicializa_estado(&estado);
    while (!estado.terminou);{
        joga_partida(&estado);
    }

}