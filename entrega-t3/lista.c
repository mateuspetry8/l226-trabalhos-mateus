#include "lista.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int dado;
    struct No *prox;
    struct No *ant;
} No;

struct lista{
    No *sentinela;
};


Lista l_cria()
{
    Lista l = malloc(sizeof(Lista));
    l->sentinela = malloc(sizeof(No));
    l->sentinela->prox = l->sentinela;
    l->sentinela->ant = l->sentinela;
    return l;
}

Lista l_cria_separando(Str s, Str sep)
{

}

// libera a memória ocupada por uma lista
void l_destroi(Lista l)
{

}

// retorna o número de elementos na lista
int l_tam(Lista l)
{

}

// retorna true se a lista tiver cheia
bool l_cheia(Lista l)
{

}

// retorna true se a lista tiver vazia
bool l_vazia(Lista l)
{

}

// imprime os dados que estão na lista
void l_imprime(Lista l)
{

}

// insere o dado d no início da lista l
void l_insere_inicio(Lista l, dado_t d)
{

}

// insere o dado d no final da lista l
void l_insere_fim(Lista l, dado_t d)
{

}

// insere o dado d na lista l, de forma que ele fique na posição p
// a primeira posição é 0
void l_insere_pos(Lista l, dado_t d, int p)
{

}

// retorna o dado no início da lista
dado_t l_dado_inicio(Lista l)
{

}

// retorna o dado no final da lista
dado_t l_dado_fim(Lista l)
{

}

// retorna o dado na posição pos da lista
dado_t l_dado_pos(Lista l, int pos)
{

}

// remove e retorna o dado no início da lista
dado_t l_remove_inicio(Lista l)
{

}

// remove e retorna o dado no final da lista
dado_t l_remove_fim(Lista l)
{

}

// remove e retorna o dado na posição pos da lista
dado_t l_remove_pos(Lista l, int pos)
{

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
