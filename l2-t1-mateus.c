#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define areadiatam 13
#define areanoitetam 8

#define qtdinimigosdia 20 
#define qtdinimigosnoite 15 

// implementação de um cronômetro
typedef struct timespec crono;

typedef struct 
{
    crono timer_onda;

    int pontos;
    int municao;
    int escudos;
    int onda;
    int armaind;
    int rodada;

    bool onda_ativa;
    bool partida_ativa;
    bool ehdia;

    const char *armasdia;
    const char *armasnoite;
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

// configura o terminal para o modo normal
void normaliza_terminal()
{
    system("stty sane");
}

// lê um caractere do teclado.
char lechar()
{
    fflush(stdout);
    char c;
    if (fread(&c, 1, 1, stdin) == 1) return c;
    return 0;
}

void inicializa_base(estado_t *est)
{
    for(int i = 0; i < 3; i++) {
        est->areadia[i] = ')';
        est->areanoite[i] = ')';
    }
    for(int i = 3; i < 13; i++) {
        est->areadia[i] = ' ';
        if(i < 8) est->areanoite[i] = ' ';
    }
}

void inicializa_estado(estado_t *est)
{
    inicializa_base(est);
    est->armasdia = "0123456789N";
    est->armasnoite = "02468N";
    est->armaind = 0;
    est->pontos = 0;
    est->onda = 1;
    est->rodada = 0;
    est->municao = 30;
    est->escudos = 3;
    est->onda_ativa = true;
    est->partida_ativa = true;
}

void processar_teclado(estado_t *est)
{
    char input = lechar();
    switch (input)
    {
    case 27:
        est->partida_ativa = false;
        break;
    case 9:
        est->armaind++;
        break;
    case 13:
        atira();
        break;
    case 32:
        //if (!est->ehdia) sonar();
        break;
    default:
        break;
    }
}

void avanca_dia(estado_t *est)
{
    char atual, prox;
    for(int i = 0; i < areadiatam - 1; i++) {
        atual = est->areadia[i];
        prox = est->areadia[i + 1];
        if(!(atual == ')' && prox == ' ')) atual = prox;
    }
    est->areadia[areadiatam - 1] = est->inimigosdia[est->rodada];
    if (est->rodada < qtdinimigosdia - 1 )est->rodada++;
}

void avanca_noite(estado_t *est)
{
    char atual, prox;
    for(int i = 0; i < areanoitetam - 1; i++) {
        atual = est->areanoite[i];
        prox = est->areanoite[i + 1];
        if(!(atual == ')' && prox == ' ')) atual = prox;
    }
    est->areadia[areanoitetam - 1] = est->inimigosnoite[est->rodada];
    if (est->rodada < qtdinimigosnoite - 1 )est->rodada++;
}

void avanca_inimigos(estado_t *est)
{
    if (est->ehdia) avanca_dia(est);
    else avanca_noite(est);
}

void processar_tempo(estado_t *est)
{
    if (crono_parcial(&est->timer_onda) >= 1.0) {
        avanca_inimigos(est);
        crono_inicia(&est->timer_onda);
    }
}

void sorteia_inimigos(estado_t *est)
{
    if (est->ehdia) {
        for (int i = 0; i < 20; i++) {
            int randini;
            randini = rand() % 11;
            est->inimigosdia[i] = est->armasdia[randini];
        }
    }
    else {
        for (int i = 0; i < 15; i++) {
            int randini;
            randini = rand() % 6;
            est->inimigosnoite[i] = est->armasnoite[randini];
        }
    }
}

void apresenta(estado_t *est)
{
    printf("%d %d %c", est->pontos, est->municao, est->armasdia[est->armaind]);
}

void joga_onda(estado_t *est) 
{
    sorteia_inimigos(est);
    while(est->onda_ativa) {
        apresenta(est);
        processar_teclado(est);
        processar_tempo(est);
    }
    est->rodada = 0;
}

void joga_partida(estado_t *est)
{
    while ((est->partida_ativa)) {
        crono_inicia(&est->timer_onda);
        est->onda_ativa = 1;
        joga_onda(est);
        /*if (!est->partida_ativa) {
            if (!pergunta_continuar()) {
                est->partida_ativa = 1;
            }
        }*/
    }
}

int main(){
    configura_terminal();
    estado_t estado;
    srand(time(NULL));
    //inicializa_tela();
    inicializa_estado(&estado);
    joga_partida(&estado);
    normaliza_terminal();
}