#include "grafo.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct aresta {
    int destino;
    void *valor;
    struct aresta *prox;
} Aresta;

typedef struct {
    void *valor;
    Aresta *prim_aresta;
} No;

struct grafo {
    int tam_no, tam_aresta;
    int num_nos, cap;
    No *nos;
    Aresta *aresta_atual;
};

Grafo g_cria(int tam_nó, int tam_aresta)
{
    Grafo g = malloc(sizeof(*g));
    if (g == NULL) return NULL;
    g->tam_no = tam_nó;
    g->tam_aresta = tam_aresta;
    g->num_nos = 0;
    g->cap = 8;
    g->nos = malloc(g->cap * sizeof(No));
    if (g->nos == NULL) { free(g); return NULL; }
    
    return g;
}

void g_destrói(Grafo self)
{
    if (self == NULL) return;
    for (int i = 0; i < self->num_nos; i++) {
        free(self->nos[i].valor);
        libera_arestas(self->nos[i].prim_aresta);
    }
    free(self->nos);
    free(self);
}

static void libera_arestas(Aresta *a)
{
    while (a != NULL) {
        Aresta *prox = a->prox;
        free(a->valor);
        free(a);
        a = prox;
    }
}

//Nos

int g_insere_nó(Grafo self, void *p_dado)
{
    if(self->num_nos == self->cap) {
        int nova_cap = self->cap * 2;
        No *novoV = realloc(self->nos, nova_cap * sizeof(No));
        if (novoV == NULL) return -1;
        self->nos = novoV;
        self->cap = nova_cap;
    }
    No *no = &self->nos[self->num_nos];
    no->valor = malloc(self->tam_no);
    if (no->valor == NULL) return -1; 
    memcpy(no->valor, p_dado, self->tam_no);
    no->prim_aresta = NULL;

    return self->num_nos++;
}

void g_remove_nó(Grafo self, int nó)
{
    free(self->nos[nó].valor);
    libera_arestas(self->nos[nó].prim_aresta);

    for (int i = nó; i < self->num_nos - 1; i++) {
        self->nos[i] = self->nos[i + 1];
    }
    self->num_nos--;

    for(int i = 0; i < self->num_nos; i++) {
        Aresta **pa = &self->nos[i].prim_aresta;
        while(*pa != NULL) {
            Aresta *a = *pa; 
            if(a->destino == nó) {
                *pa = a->prox;
                free(a->valor);
                free(a);
            }
            else {
                if (a->destino > nó) a->destino--;
                pa = &a->prox;
            }
        }
    }
    
}

void g_altera_valor_do_nó(Grafo self, int nó, void *p_dado)
{
    memcpy(self->nos[nó].valor, p_dado, self->tam_no);
}

void *g_valor_do_nó(Grafo self, int nó)
{
    return self->nos[nó].valor;
}

int g_num_nós(Grafo self)
{
    return self->num_nos;
}

// Arestas

void g_altera_valor_da_aresta(Grafo self, int nó_origem, int nó_destino,
                              void *p_dado)
{
    Aresta **pa = &self->nos[nó_origem].prim_aresta;
    while (*pa != NULL && (*pa)->destino != nó_destino) pa = &(*pa)->prox;
    Aresta *a = *pa;

    if (p_dado == NULL) {
        if (a == NULL) return;
        *pa = a->prox;
        free(a->valor);
        free(a);
        return;
    }
    if (a == NULL) {
        a = malloc(sizeof(Aresta));
        if (a == NULL) return;
        a->valor = malloc(self->tam_aresta);
        if (a->valor == NULL) { free(a); return; }
        a->destino = nó_destino;
        a->prox = self->nos[nó_origem].prim_aresta;
        self->nos[nó_origem].prim_aresta = a;
    }
    memcpy(a->valor, p_dado, self->tam_aresta);
}
