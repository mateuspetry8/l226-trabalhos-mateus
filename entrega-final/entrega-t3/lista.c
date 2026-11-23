#include "lista.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct No {
    dado_t dado;
    struct No *prox;
    struct No *ant;
} No;

struct lista{
    No *sentinela;
};


Lista l_cria()
{
    Lista l = malloc(sizeof(*l));
    l->sentinela = malloc(sizeof(No));
    l->sentinela->prox = l->sentinela;
    l->sentinela->ant = l->sentinela;
    return l;
}

Lista l_cria_separando(Str s, Str sep)
{
    Lista nova = l_cria();
    int n = s_tam(s);
    int posant = 0;

    while (posant < n) {
        while (posant < n && s_busca_c(s, posant, sep) == posant) {
            posant++;
        }
        if (posant >= n) break;

        int pos = s_busca_c(s, posant, sep);
        int tam = (pos == -1) ? -1 : pos - posant;

        dado_t d = s_cria_substring(s, posant, tam);
        l_insere_fim(nova, d);

        posant = (pos == -1) ? n : pos;
    }

    return nova;
}

void l_destroi(Lista l)
{
    No *atual = l->sentinela->prox;
    while(atual != l->sentinela) {
        No *prox = atual->prox;
        free(atual);
        atual = prox;
    }
    free(l->sentinela);
    free(l);
    //talvez free em Str
}

int l_tam(Lista l)
{
    int tam = 0;
    No *atual = l->sentinela;

    while(atual->prox != l->sentinela) {
        tam++;
        atual = atual->prox;
    }

    return tam;
}

bool l_cheia(Lista l)
{
    return false;
}

bool l_vazia(Lista l)
{
    return l->sentinela->prox == l->sentinela;
}

void l_imprime(Lista l)
{
    No *atual = l->sentinela->prox;

    while (atual != l->sentinela)
    {
        s_imprime(atual->dado);
        atual = atual->prox;
    }
}

void l_insere_inicio(Lista l, dado_t d)
{
    No *novo = malloc(sizeof(No));
    novo->prox = l->sentinela->prox;
    novo->ant = l->sentinela;
    novo->dado = d;
    novo->prox->ant = novo;
    l->sentinela->prox = novo;
}

void l_insere_fim(Lista l, dado_t d)
{
    No *novo = malloc(sizeof(No));
    novo->prox = l->sentinela;
    novo->ant = l->sentinela->ant;
    novo->dado = d;
    novo->ant->prox = novo;
    l->sentinela->ant = novo;
}

void l_insere_pos(Lista l, dado_t d, int p)
{
    int tam = l_tam(l);
    if (p < 0 || p > tam) {
        return; 
    }

    if (p == 0) {
        l_insere_inicio(l, d);
        return;
    }
    if (p == tam) {
        l_insere_fim(l, d);
        return;
    }
    
    No *atual = l->sentinela->prox;
    for(int i = 0; i < p; i++) {
        atual = atual->prox;
    }

    No *novo = malloc(sizeof(No));
    novo->dado = d;
    novo->ant = atual->ant;
    novo->prox = atual;
    atual->ant->prox = novo;
    atual->ant = novo;
}

dado_t l_dado_inicio(Lista l)
{
    if (l_vazia(l)) return NULL;
    return l->sentinela->prox->dado;
}

dado_t l_dado_fim(Lista l)
{
    if (l_vazia(l)) return NULL;
    return l->sentinela->ant->dado;
}

dado_t l_dado_pos(Lista l, int pos)
{
    if (pos < 0 || pos >= l_tam(l)) {
        return NULL;
    }
    
    No *atual = l->sentinela->prox;
    for(int i = 0; i < pos; i++) {
        atual = atual->prox;
    }
    return atual->dado;
}

dado_t l_remove_inicio(Lista l)
{
    if (l_vazia(l)) return NULL;

    No *libera = l->sentinela->prox;
    l->sentinela->prox = l->sentinela->prox->prox;
    l->sentinela->prox->ant = l->sentinela;
    dado_t d = libera->dado;
    free(libera);
    return d;
}

dado_t l_remove_fim(Lista l)
{
    if (l_vazia(l)) return NULL;

    No *libera = l->sentinela->ant;
    l->sentinela->ant = l->sentinela->ant->ant;
    l->sentinela->ant->prox = l->sentinela;
    dado_t d = libera->dado;
    free(libera);
    return d;
}

dado_t l_remove_pos(Lista l, int pos)
{
    int tam = l_tam(l);
    if (pos < 0 || pos >= tam) {
        return NULL; 
    }

    if (pos == 0) {
        return l_remove_inicio(l);
    }
    if (pos == tam - 1) {
        return l_remove_fim(l);
    }
    
    No *libera = l->sentinela->prox;
    for(int i = 0; i < pos; i++) {
        libera = libera->prox;
    }

    dado_t d = libera->dado;
    libera->ant->prox = libera->prox;
    libera->prox->ant = libera->ant;
    free(libera);
    return d;
}

dado_t l_primeiro(Lista l)
{
    return l_dado_inicio(l);
}

void l_insere(Lista l, dado_t d)
{
    l_insere_fim(l, d);
}

dado_t l_remove(Lista l)
{
    return l_remove_inicio(l);
}


// funções para usar a lista como uma pilha

dado_t l_topo(Lista l)
{
    return l_dado_inicio(l);
}

void l_empilha(Lista l, dado_t d)
{
    l_insere_inicio(l, d);
}

dado_t l_desempilha(Lista l)
{
    return l_remove_inicio(l);
}
