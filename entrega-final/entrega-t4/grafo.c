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

    int modo;            // 0 nenhuma, 1 partem, 2 chegam
    int nó_consulta;     // nó origem (modo 1) ou destino (modo 2)
    int origem_atual;
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
    g->modo = 0;
    
    return g;
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
    self->modo = 0;
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

void *g_valor_da_aresta(Grafo self, int nó_origem, int nó_destino)
{
    Aresta *a = self->nos[nó_origem].prim_aresta;
    while(a != NULL) {
        if(a->destino == nó_destino) return a->valor;
        a = a->prox;
    }
    return NULL;
}

void g_arestas_que_partem(Grafo self, int nó_origem)
{
    self->modo = 1;
    self->nó_consulta = nó_origem;
    self->aresta_atual = self->nos[nó_origem].prim_aresta;
}

void g_arestas_que_chegam(Grafo self, int nó_destino)
{
    self->modo = 2;
    self->nó_consulta = nó_destino;
    self->origem_atual = -1;
    self->aresta_atual = NULL;
}

void *g_próxima_aresta(Grafo self, int *p_nó_vizinho)
{
    if (self->modo == 1) {
        Aresta *a = self->aresta_atual;
        if(a == NULL) return NULL;
        self->aresta_atual = a->prox;
        if (p_nó_vizinho != NULL) *p_nó_vizinho = a->destino;
        return a->valor;
    }
    if (self->modo == 2) {
        for (;;) {
            Aresta *a = self->aresta_atual;
            while (a != NULL && a->destino != self->nó_consulta) a = a->prox;
            if (a != NULL) {
                self->aresta_atual = a->prox;
                if (p_nó_vizinho != NULL) *p_nó_vizinho = self->origem_atual;
                return a->valor;
            }
            if (self->origem_atual + 1 >= self->num_nos) {
                self->aresta_atual = NULL;
                return NULL;
            }
        self->origem_atual++;
        self->aresta_atual = self->nos[self->origem_atual].prim_aresta;
        }
    }
    return NULL;
}

// Algoritmos

static int kahn(Grafo self, Fila ordem)
{
    int n = self->num_nos;
    if (n == 0) return 0;

    int *grau = calloc(n, sizeof(int));
    if (grau == NULL) return -1;
    for (int i = 0; i < n; i++) {
        for (Aresta *a = self->nos[i].prim_aresta; a != NULL; a = a->prox) {
            grau[a->destino]++;
        }
    }

    Fila prontos = f_cria(sizeof(int));
    if (prontos == NULL) {
        free(grau);
        return -1;
    }
    for (int i = 0; i < n; i++) {
        if (grau[i] == 0) f_insere(prontos, &i);
    }

    int tirados = 0;
    while (!f_tá_vazia(prontos)) {
        int u;
        f_remove(prontos, &u);
        tirados++;
        if (ordem != NULL) f_insere(ordem, &u);
        for (Aresta *a = self->nos[u].prim_aresta; a != NULL; a = a->prox) {
            if (--grau[a->destino] == 0) f_insere(prontos, &a->destino);
        }
    }

    f_destrói(prontos);
    free(grau);
    return tirados;
}

bool g_tem_ciclo(Grafo self)
{
    int tirados = kahn(self, NULL);
    if (tirados < 0) return false; 
    return tirados < self->num_nos;
}

Fila g_ordem_topológica(Grafo self)
{
    Fila ordem = f_cria(sizeof(int));
    if (ordem == NULL) return NULL;
    int tirados = kahn(self, ordem);
    if (tirados != self->num_nos) {
        f_destrói(ordem);
        ordem = f_cria(sizeof(int));
    }
    return ordem;
}