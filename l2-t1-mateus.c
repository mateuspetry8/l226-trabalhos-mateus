#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// implementação de um cronômetro
typedef struct timespec crono;

typedef struct 
{
    int pontos;
    int municao;
    int escudos;
    int onda;

    bool terminou;
    bool ehdia;

    char armas[11];
    char areadia[13];
    char areanoite[8];
    char inimigosdia[20];
    char inimigosnoite[15];
} estado_t;

// inicializa um cronômetro com a hora atual
void crono_inicia(crono *c)
{
    clock_gettime(CLOCK_MONOTONIC, c);
}

// retorna o tempo passado desde que o cronômetro *c foi iniciado, em segundos
double crono_parcial(crono *c)
{
    crono agora;
    clock_gettime(CLOCK_MONOTONIC, &agora);

    double segundos = agora.tv_sec - c->tv_sec;
    double nanosegundos = agora.tv_nsec - c->tv_nsec;
    return segundos + 1e-9 * nanosegundos;
}

// configura o terminal para o modo "cru", para permitir a leitura
//   de cada caractere digitado sem esperar pelo "enter".
void configura_terminal()
{
    if (system("stty raw opost -echo min 0 time 1") != 0) {
        perror("erro na execução de system(\"stty\")");
        fprintf(stderr, "você tem o programa stty instalado?\n");
        exit(1);
    };
    if (setvbuf(stdin, NULL, _IONBF, 0) != 0) {
        perror("erro na execução de setvbuf()");
        exit(1);
    }
}

// lê um caractere do teclado.
char lechar()
{
    fflush(stdout);
    char c;
    if (fread(&c, 1, 1, stdin) == 1) return c;
    return 0;
}

void inicializa_estado(estado_t *est)
{
    est->pontos = 0;
    est->onda = 1;
    est->municao = 30;
    est->escudos = 3;
    est->terminou = false;
}

void joga_onda(estado_t *est){
    while(){
        processar_teclado(est);
        processar_tempo(est);
        apresenta(est);
    }
}

void joga_partida(estado_t *est)
{
    while (!(est->terminou)) {
        joga_onda(est);
    };
}

// configura o terminal para o modo normal
void normaliza_terminal()
{
    system("stty sane");
}

int main(){
    configura_terminal();
    estado_t estado;
    inicializa_tela();
    inicializa_estado(&estado);
    while (!estado.terminou) {
        joga_partida(&estado);
    }
    normaliza_terminal();
}