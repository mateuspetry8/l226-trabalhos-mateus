#include "lista.h"

#include <stdio.h>
#include <stdlib.h>

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

// cria uma lista contendo substrings de s
// as substrings são separadas por quaisquer caractere de sep
// os caracteres de sep não aparecem nas substrings
// exemplos:
//   "a,ba,ca, te", ", " -> ["a" "ba" "ca" "te"]
//   "aba \ncate\n", "\n" -> ["aba " "cate"]
Lista l_cria_separando(Str s, Str sep)
{

}

// libera a memória ocupada por uma lista
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

// retorna o número de elementos na lista
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

// retorna true se a lista tiver cheia
bool l_cheia(Lista l)
{
    return false;
}

// retorna true se a lista tiver vazia
bool l_vazia(Lista l)
{
    return l->sentinela->prox == l->sentinela;
}

// imprime os dados que estão na lista
void l_imprime(Lista l)
{
    No *atual = l->sentinela->prox;

    while (atual != l->sentinela)
    {
        s_imprime(atual->dado);
        atual = atual->prox;
    }
}

// insere o dado d no início da lista l
void l_insere_inicio(Lista l, dado_t d)
{
    No *novo = malloc(sizeof(No));
    novo->prox = l->sentinela->prox;
    novo->ant = l->sentinela;
    novo->dado = d;
    novo->prox->ant = novo;
    l->sentinela->prox = novo;
}

// insere o dado d no final da lista l
void l_insere_fim(Lista l, dado_t d)
{
    No *novo = malloc(sizeof(No));
    novo->prox = l->sentinela;
    novo->ant = l->sentinela->ant;
    novo->dado = d;
    novo->ant->prox = novo;
    l->sentinela->ant = novo;
}

// insere o dado d na lista l, de forma que ele fique na posição p
// a primeira posição é 0
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

// retorna o dado no início da lista
dado_t l_dado_inicio(Lista l)
{
    return l->sentinela->prox->dado;
}

// retorna o dado no final da lista
dado_t l_dado_fim(Lista l)
{
    return l->sentinela->ant->dado;
}

// retorna o dado na posição pos da lista
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

// remove e retorna o dado no início da lista
dado_t l_remove_inicio(Lista l)
{
    No *libera = l->sentinela->prox;
    l->sentinela->prox = l->sentinela->prox->prox;
    l->sentinela->prox->ant = l->sentinela;
    dado_t d = libera->dado;
    free(libera);
    return d;
}

// remove e retorna o dado no final da lista
dado_t l_remove_fim(Lista l)
{
    No *libera = l->sentinela->ant;
    l->sentinela->ant = l->sentinela->ant->ant;
    l->sentinela->ant->prox = l->sentinela;
    dado_t d = libera->dado;
    free(libera);
    return d;
}

// remove e retorna o dado na posição pos da lista
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

// funções para usar a lista como uma fila

// l_cria, l_destroi, l_vazia

// retorna o dado que está no início da fila
dado_t l_primeiro(Lista l)
{

}

// insere um dado no fim da fila
void l_insere(Lista l, dado_t d)
{

}

// remove e retorna o dado que está no início da fila
dado_t l_remove(Lista l)
{

}


// funções para usar a lista como uma pilha

// l_cria, l_destroi, l_vazia

// retorna o dado que está no topo da pilha
dado_t l_topo(Lista l)
{

}

// empilha um dado no topo da pilha
void l_empilha(Lista l, dado_t d)
{

}

// remove e retorna o dado que está no topo da pilha
dado_t l_desempilha(Lista l)
{

}
