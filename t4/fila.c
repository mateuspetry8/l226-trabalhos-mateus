#include "fila.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct no {
    void *dado;
    struct no *prox;
    struct no *ant;
} No;

struct fila {
    No *sentinela;
    size_t tamd;
    No *atual;
    int sentido;
};

Fila f_cria(int tam_do_dado)
{
    if (tam_do_dado <= 0) return NULL;

    Fila f = malloc(sizeof(struct fila));
    if (f == NULL) return NULL;

    f->sentinela = malloc(sizeof(No));
    if (f->sentinela == NULL) { free(f); return NULL; }
    f->sentinela->dado = NULL;
    f->sentinela->prox = f->sentinela;
    f->sentinela->ant = f->sentinela;
    f->tamd = tam_do_dado;
    f->atual = f->sentinela;
    f->sentido = 0;
    
    return f;
}

void f_destrói(Fila self)
{
    if (self == NULL) return;

    No *atual = self->sentinela->prox;
    while(atual != self->sentinela) {
        No *prox = atual->prox;
        free(atual->dado);
        free(atual);
        atual = prox;
    }
    free(self->sentinela);
    free(self);
}

bool f_tá_vazia(Fila self)
{
    return self->sentinela->prox == self->sentinela;
}

void f_remove(Fila self, void *pdado)
{
    if (f_tá_vazia(self)) return;

    No *libera = self->sentinela->prox;
    self->sentinela->prox = self->sentinela->prox->prox;
    self->sentinela->prox->ant = self->sentinela;
    if (pdado != NULL) memcpy(pdado, libera->dado, self->tamd);
    free(libera->dado);
    free(libera);
}

void f_insere(Fila self, void *pdado)
{
    No *novo = malloc(sizeof(No));
    if (novo == NULL) return;
    novo->dado = malloc(self->tamd);
    if (novo->dado == NULL) { free(novo); return; }
    memcpy(novo->dado, pdado, self->tamd);
    novo->prox = self->sentinela;
    novo->ant = self->sentinela->ant;
    novo->ant->prox = novo;
    self->sentinela->ant = novo;
}

void f_inicia_percurso(Fila self, int pos_inicial)
{
    if (pos_inicial >= 0) {
        self->sentido = 1;
        self->atual = self->sentinela->prox;
        for(int i = 0; i < pos_inicial && self->atual != self->sentinela; i++) {
            self->atual = self->atual->prox;
        }
    }
    else {
        self->sentido = -1;
        self->atual = self->sentinela->ant;
        for(int i = 1; i < -pos_inicial && self->atual != self->sentinela; i++) {
            self->atual = self->atual->ant;
        }
    }
}

bool f_próximo(Fila self, void *pdado)
{
    if (self->atual == self->sentinela) return false;

    if (pdado != NULL)
        memcpy(pdado, self->atual->dado, self->tamd);

    if (self->sentido == 1) self->atual = self->atual->prox;    
    if (self->sentido == -1) self->atual = self->atual->ant; 
    return true;
}