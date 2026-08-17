#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define qtdataquesdia 11
#define qtdataquesnoite 6

#define areadiatam 14
#define areanoitetam 9

#define qtdinimigosdia 20 
#define qtdinimigosnoite 15 

#define ARQUIVO_SCORES "scores.txt"
#define TOP_SCORES 3

// implementação de um cronômetro
typedef struct timespec crono;

typedef struct 
{
    crono timer_onda;

    int pontos, pontos_guardados;
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
    char areadia[14];
    char areanoite[9];
    char inimigosdia[20];
    char inimigosnoite[15];
} estado_t;

void le_top_scores(int top[TOP_SCORES])
{
    for (int i = 0; i < TOP_SCORES; i++) top[i] = 0;

    FILE *f = fopen(ARQUIVO_SCORES, "r");
    if (f == NULL) return;

    for (int i = 0; i < TOP_SCORES; i++) {
        if (fscanf(f, "%d", &top[i]) != 1) break;
    }

    fclose(f);
}

void salva_top_scores(const int top[TOP_SCORES])
{
    FILE *f = fopen(ARQUIVO_SCORES, "w");
    if (f == NULL) return;

    for (int i = 0; i < TOP_SCORES; i++) {
        fprintf(f, "%d\n", top[i]);
    }

    fclose(f);
}

void atualiza_top_scores(int score)
{
    int top[TOP_SCORES];
    le_top_scores(top);

    for (int i = 0; i < TOP_SCORES; i++) {
        if (score > top[i]) {
            for (int j = TOP_SCORES - 1; j > i; j--) {
                top[j] = top[j - 1];
            }
            top[i] = score;
            break;
        }
    }

    salva_top_scores(top);
}

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

// coloca os escudos e espacos vazios na area de combate (array "area")
void inicializa_base(estado_t *est)
{
    est->areadia[0] = ' ';
    est->areanoite[0] = ' ';
    for(int i = 1; i < 4; i++) {
        est->areadia[i] = ')';
        est->areanoite[i] = ')';
    }
    for(int i = 4; i < 14; i++) {
        est->areadia[i] = ' ';
        if(i < 9) est->areanoite[i] = ' ';
    }
}

// atribui valores iniciais as variaveis da struct de controle
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
    est->ehdia = true;
    est->pontos_guardados = 0;
}

//troca para a proxima arma do personagem
void troca_arma(estado_t *est)
{
    if (est->ehdia) {
        if (est->armaind < qtdataquesdia - 1) est->armaind++;
        else est->armaind = 0;
    }
    else {
        if (est->armaind < qtdataquesnoite - 1) est->armaind++;
        else est->armaind = 0;
    }
}

void guarda_ponto(estado_t *est, int i)
{
    if (est->ehdia) {
        est->pontos_guardados += (areadiatam - i);
    }
    else {
        est->pontos_guardados += (areanoitetam - i);
    }
}

// atira matando o primeiro inimigo de valor igual. Ou diminui 'N'
void atira(estado_t *est)
{
    for (int i = 1; i < areadiatam; i++) {
        char inim = est->areadia[i];
        char armaatual = est->armasdia[est->armaind];
        if (inim == armaatual) {
            if (inim != 'N') {
                est->areadia[i] = ' ';
                est->municao--;
                guarda_ponto(est, i);
                return;
            }
            else {
                est->areadia[i] = 'n';
                est->municao--;
                guarda_ponto(est, i);
                return;
            }
        }
        else if (armaatual == 'N' && inim == 'n') {
            est->areadia[i] = ' ';
            est->municao--;
            guarda_ponto(est, i);
            return;
        }
    }
    est->municao--;
}

// escolhe alguma acao do jogo dependendo do input do teclado 
void processar_teclado(estado_t *est)
{
    char input = lechar();
    switch (input)
    {
    case 27:
        est->partida_ativa = false;
        est->onda_ativa = false;
        break;
    case 9:
        troca_arma(est);
        break;
    case 13:
        atira(est);
        break;
    case 32:
        //if (!est->ehdia) sonar();
        break;
    default:
        break;
    }
}

// confere se um inimigo encostou em um escudo, se sim os dois "morrem"
void checa_escudo(estado_t *est)
{
    int i = est->escudos;
    if (est->areadia[i] != ')') {
        est->areadia[i] = ' ';
        est->escudos--;
    }
}

// os inimigos avancam na array "area", o ultimo elemento dela sera o proximo elemento da base dos inimigos ativos.
void avanca_dia(estado_t *est)
{
    char atual, prox;
    for(int i = 1; i < areadiatam - 1; i++) {
        atual = est->areadia[i];
        prox = est->areadia[i + 1];
        if(atual != ')' && prox != ' ') est->areadia[i] = prox;
        else if (atual == ')' && prox != ' ') est->areadia[i] = prox;
        else if (atual != ')' && prox == ' ') est->areadia[i] = prox;
    }
    if (est->escudos != 0) checa_escudo(est);
    if (est->areadia[0] != ' ') {
        est->onda_ativa = false;
        est->partida_ativa = false;
    }
    est->areadia[areadiatam - 1] = est->inimigosdia[est->rodada];
    if (est->rodada < qtdinimigosdia - 1 )est->rodada++;
    else est->inimigosdia[est->rodada] = ' ';
}

void avanca_noite(estado_t *est)
{
    
}

// os inimigos avancam na array "area", o ultimo elemento 
// dela sera o proximo elemento da base dos inimigos ativos.
void avanca_inimigos(estado_t *est)
{
    if (est->ehdia) avanca_dia(est);
    else avanca_noite(est);
}

void processar_tempo(estado_t *est)
{
    if (crono_parcial(&est->timer_onda) >= 1.5) {
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

void acabou_onda(estado_t *est)
{
    if (est->rodada >= 19) {
        for(int i = 1; i < areadiatam; i++) {
            if (est->areadia[i] != ' ' && est->areadia[i] != ')') return;
        }
        est->onda_ativa = false;
    }
}

void inicializa_nova_onda(estado_t *est)
{
    est->onda++;
    est->armaind = 0;
    est->rodada = 0;
    est->municao = 30;
    est->onda_ativa = true;
    est->pontos_guardados = 0;
}

void calcula_pontos(estado_t *est)
{
    if (est->ehdia) {
        est->pontos += 2 * est->municao;
        est->pontos += 10 * est->escudos;
        est->pontos += est->pontos_guardados;
    }
}

void proxima_onda(estado_t *est)
{
    calcula_pontos(est);

    char input_onda = 0;
    while (est->partida_ativa) {
        printf("\r\033[KPontos: %d, Escudos: %d, Onda: %d. "  
               "Pressione 'r' para a proxima onda", 
               est->pontos, est->escudos, est->onda + 1);
        input_onda = lechar();
        if (input_onda == 27) {
            est->partida_ativa = false;
            return;
        }
        if (input_onda == 'r') break;
    }

    if (!est->partida_ativa) return;

    inicializa_nova_onda(est);
}

void perguntar_continuar_perdeu(estado_t *est)
{
    int top[TOP_SCORES];
    atualiza_top_scores(est->pontos);
    le_top_scores(top);

    char input_fim = 0;
    while (!est->partida_ativa) {
        printf("\r\033[KPontos: %d, Ondas: %d. Top 3: %d | %d | %d. Voce morreu, "
            "pressione 'r' jogar novamente", est->pontos, est->onda,
            top[0], top[1], top[2]);
        input_fim = lechar();
        if (input_fim == 27) {
            return;
        }
        if (input_fim == 'r') {
            est->partida_ativa = true;
            inicializa_estado(est);
        }
    }
}

void apresenta(estado_t *est)
{
    printf("\r\033[K%d %d %c", est->pontos, est->municao, 
                                est->armasdia[est->armaind]);
    for (int i = 1; i < areadiatam; i++) {
        printf("%c", est->areadia[i]);
    }
}

void joga_onda(estado_t *est) 
{
    sorteia_inimigos(est);
    while(est->onda_ativa) {
        apresenta(est);
        processar_teclado(est);
        processar_tempo(est);
        acabou_onda(est);
    }
    if (est->partida_ativa) proxima_onda(est);
}

void joga_partida(estado_t *est)
{
    while ((est->partida_ativa)) {
        crono_inicia(&est->timer_onda);
        est->onda_ativa = 1;
        joga_onda(est);
        if (!est->partida_ativa) {
            (perguntar_continuar_perdeu(est));
        }
    }
}

int main(){
    configura_terminal();
    estado_t estado;
    srand(time(NULL));
    inicializa_estado(&estado);
    //inicializa_tela();
    joga_partida(&estado);
    normaliza_terminal();
}