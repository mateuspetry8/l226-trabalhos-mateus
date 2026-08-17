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

// toca um arquivo de som da pasta Sons, no formato <id>.3.wav
void tocar_som_id(const char *id, bool esperar)
{
    char comando[128];
    const char *fim = esperar ? "" : " &";

    snprintf(comando, sizeof(comando),
             "aplay -q Sons/%s.3.wav >/dev/null 2>&1%s",
             id, fim);
    system(comando);
}

// mapeia um tipo de ataque para o id do arquivo de som correspondente
const char *id_som_ataque(char ataque)
{
    static char id[2];

    if (ataque >= '0' && ataque <= '9') {
        id[0] = ataque;
        id[1] = '\0';
        return id;
    }
    if (ataque == 'N' || ataque == 'n') return "11";
    return "x";
}

void tocar_som_ataque(char ataque)
{
    tocar_som_id(id_som_ataque(ataque), false);
}

void tocar_som_arma(char arma)
{
    tocar_som_ataque(arma);
}

void tocar_som_escudo()
{
    tocar_som_id("12", false);
}

void tocar_som_espaco()
{
    tocar_som_id("x", false);
}

void tocar_som_tiro_errado()
{
    tocar_som_espaco();
}

void tocar_som_fim_onda()
{
    tocar_som_id("12", true);
    tocar_som_id("x", true);
    tocar_som_id("12", true);
}

void tocar_som_fim_partida()
{
    tocar_som_id("11", true);
    tocar_som_id("12", true);
    tocar_som_id("11", true);
}

// toca o som da posicao informada (escudo, vazio ou ataque)
void tocar_som_posicao(char c)
{
    if (c == ')') {
        tocar_som_id("12", true);
        return;
    }
    if (c == ' ') {
        tocar_som_id("x", true);
        return;
    }
    tocar_som_id(id_som_ataque(c), true);
}

// sonar: toca os sons das posicoes, na ordem em que aparecem na area
void sonar(estado_t *est)
{
    char *area = est->ehdia ? est->areadia : est->areanoite;
    int areatam = est->ehdia ? areadiatam : areanoitetam;

    for (int i = 1; i < areatam; i++) {
        tocar_som_posicao(area[i]);
    }
}

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

int chance_dia_por_onda(int onda)
{
    if (onda <= 1) return 100;
    if (onda == 2) return 80;
    if (onda == 3) return 60;
    if (onda == 4) return 40;
    return 20;
}

void sorteia_turno_onda(estado_t *est)
{
    int chance_dia = chance_dia_por_onda(est->onda);
    est->ehdia = ((rand() % 100) < chance_dia);
}

void prepara_areas_onda(estado_t *est)
{
    est->areadia[0] = ' ';
    est->areanoite[0] = ' ';

    for (int i = 1; i < 4; i++) {
        char bloco = (i <= est->escudos) ? ')' : ' ';
        est->areadia[i] = bloco;
        est->areanoite[i] = bloco;
    }

    for (int i = 4; i < areadiatam; i++) {
        est->areadia[i] = ' ';
        if (i < areanoitetam) est->areanoite[i] = ' ';
    }
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
    inicializa_base(est);
    sorteia_turno_onda(est);
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

    if (est->ehdia) tocar_som_arma(est->armasdia[est->armaind]);
    else tocar_som_arma(est->armasnoite[est->armaind]);
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
    char *area = est->ehdia ? est->areadia : est->areanoite;
    int areatam = est->ehdia ? areadiatam : areanoitetam;
    const char *armas = est->ehdia ? est->armasdia : est->armasnoite;
    char armaatual = armas[est->armaind];

    for (int i = 1; i < areatam; i++) {
        char inim = area[i];
        if (inim == armaatual) {
            if (inim != 'N') {
                area[i] = ' ';
                est->municao--;
                guarda_ponto(est, i);
                tocar_som_arma(armaatual);
                return;
            }
            else {
                area[i] = 'n';
                est->municao--;
                guarda_ponto(est, i);
                tocar_som_arma(armaatual);
                return;
            }
        }
        else if (armaatual == 'N' && inim == 'n') {
            area[i] = ' ';
            est->municao--;
            guarda_ponto(est, i);
            tocar_som_arma(armaatual);
            return;
        }
    }
    est->municao--;
    tocar_som_tiro_errado();
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
        sonar(est);
        break;
    default:
        break;
    }
}

// confere se um inimigo encostou em um escudo, se sim os dois "morrem"
void checa_escudo(estado_t *est)
{
    int i = est->escudos;
    char *area = est->ehdia ? est->areadia : est->areanoite;

    if (area[i] != ')') {
        area[i] = ' ';
        est->escudos--;
        tocar_som_escudo();
    }
}

void avanca_area(estado_t *est, char area[], int areatam,
                 char *inimigos, int qtdinimigos)
{
    // area[0] e um controle de derrota; com escudo vivo, ele fica sempre vazio.
    if (est->escudos > 0) area[0] = ' ';

    // Sem escudos, inimigo em area[1] ja atingiu o jogador neste tick.
    if (est->escudos == 0 && area[1] != ' ') {
        area[0] = area[1];
        est->onda_ativa = false;
        est->partida_ativa = false;
        tocar_som_fim_partida();
        return;
    }

    char atual, prox;
    for (int i = 1; i < areatam - 1; i++) {
        atual = area[i];
        prox = area[i + 1];
        if (atual != ')' && prox != ' ') area[i] = prox;
        else if (atual == ')' && prox != ' ') area[i] = prox;
        else if (atual != ')' && prox == ' ') area[i] = prox;
    }

    if (est->escudos != 0) checa_escudo(est);

    area[areatam - 1] = inimigos[est->rodada];
    if (area[areatam - 1] != ' ') tocar_som_ataque(area[areatam - 1]);
    if (est->rodada < qtdinimigos - 1) est->rodada++;
    else inimigos[est->rodada] = ' ';
}

// os inimigos avancam na array "area", o ultimo elemento 
// dela sera o proximo elemento da base dos inimigos ativos.
void avanca_inimigos(estado_t *est)
{
    if (est->ehdia) avanca_area(est, est->areadia, areadiatam, est->inimigosdia, qtdinimigosdia);
    else avanca_area(est, est->areanoite, areanoitetam, est->inimigosnoite, qtdinimigosnoite);
}

void processar_tempo(estado_t *est)
{
    double tempo_base_dia = 2.0;
    for (int i = 1; i < est->onda; i++) {
        tempo_base_dia *= 0.9;
    }

    double intervalo_mov = est->ehdia ? tempo_base_dia : (tempo_base_dia * 3.0);

    if (crono_parcial(&est->timer_onda) >= intervalo_mov) {
        avanca_inimigos(est);
        crono_inicia(&est->timer_onda);
    }
}

void sorteia_inimigos(estado_t *est)
{
    if (est->ehdia) {
        for (int i = 0; i < qtdinimigosdia; i++) {
            int randini;
            randini = rand() % qtdataquesdia;
            est->inimigosdia[i] = est->armasdia[randini];
        }
    }
    else {
        for (int i = 0; i < qtdinimigosnoite; i++) {
            int randini;
            randini = rand() % qtdataquesnoite;
            est->inimigosnoite[i] = est->armasnoite[randini];
        }
    }
}

void acabou_onda(estado_t *est)
{
    char *area = est->ehdia ? est->areadia : est->areanoite;
    int areatam = est->ehdia ? areadiatam : areanoitetam;
    int qtdinimigos = est->ehdia ? qtdinimigosdia : qtdinimigosnoite;

    if (est->rodada >= (qtdinimigos - 1)) {
        for (int i = 1; i < areatam; i++) {
            if (area[i] != ' ' && area[i] != ')') return;
        }
        est->onda_ativa = false;
        tocar_som_fim_onda();
    }
}

void inicializa_nova_onda(estado_t *est)
{
    est->onda++;
    sorteia_turno_onda(est);
    est->armaind = 0;
    est->rodada = 0;
    est->municao = 30;
    est->onda_ativa = true;
    est->pontos_guardados = 0;
    prepara_areas_onda(est);
}

void calcula_pontos(estado_t *est)
{
    est->pontos += 2 * est->municao;
    est->pontos += 10 * est->escudos;
    est->pontos += est->pontos_guardados;
}

void proxima_onda(estado_t *est)
{
    calcula_pontos(est);

    int onda_seguinte = est->onda + 1;
    int chance_dia = chance_dia_por_onda(onda_seguinte);

    char input_onda = 0;
    while (est->partida_ativa) {
        printf("\r\033[KPontos: %d, Escudos: %d, Onda: %d. "
               "Chance de dia: %d%%. Pressione 'r' para a proxima onda",
               est->pontos, est->escudos, onda_seguinte, chance_dia);
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
    if (!est->ehdia) {
        printf("\r\033[K%d", est->pontos);
        return;
    }

    const char *armas = est->ehdia ? est->armasdia : est->armasnoite;
    char *area = est->ehdia ? est->areadia : est->areanoite;
    int areatam = est->ehdia ? areadiatam : areanoitetam;
    const char *turno = est->ehdia ? "DIA" : "NOITE";

    printf("\r\033[K%s %d %d %c", turno, est->pontos, est->municao,
           armas[est->armaind]);
    for (int i = 1; i < areatam; i++) {
        printf("%c", area[i]);
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