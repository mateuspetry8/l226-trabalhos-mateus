#include "calc.h"
#include "str.h"

#include <stdio.h>

typedef enum {
    CL_MAIS_MENOS,
    CL_MUL_DIV,
    CL_POT,
    CL_ABRE,
    CL_FECHA,
    CL_INVALIDA
} ClasseOperador;

static ClasseOperador classifica_operador(Str token)
{
    if (s_tam(token) != 1) return CL_INVALIDA;

    unichar c = s_ch(token, 0);
    switch (c) {
        case '+': case '-': return CL_MAIS_MENOS;
        case '*': case '/': return CL_MUL_DIV;
        case '^':            return CL_POT;
        case '(':            return CL_ABRE;
        case ')':            return CL_FECHA;
        default:              return CL_INVALIDA;
    }
}

typedef enum {
    ACAO_ERRO,
    ACAO_EMPILHA,
    ACAO_DESCARTA,
    ACAO_OPERA,
    ACAO_TERMINA
} Acao;

enum { LINHA_V, LINHA_MM, LINHA_MD, LINHA_POT, LINHA_ABRE, N_LINHAS };
enum { COL_FIM, COL_MM, COL_MD, COL_POT, COL_ABRE, COL_FECHA, N_COLUNAS };

static const Acao tabela[N_LINHAS][N_COLUNAS] = {
    //           FIM           +-             */             ^              (              )
    /* V    */ { ACAO_TERMINA, ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_ERRO },
    /* +-   */ { ACAO_OPERA,   ACAO_OPERA,    ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_OPERA },
    /* * /  */ { ACAO_OPERA,   ACAO_OPERA,    ACAO_OPERA,    ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_OPERA },
    /* ^    */ { ACAO_OPERA,   ACAO_OPERA,    ACAO_OPERA,    ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_OPERA },
    /* (    */ { ACAO_ERRO,    ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_DESCARTA },
};

static Acao decide_acao(Str topo, Str entrada)
{
    int linha;
    if (topo == NULL) {
        linha = LINHA_V;
    } else {
        switch (classifica_operador(topo)) {
            case CL_MAIS_MENOS: linha = LINHA_MM;   break;
            case CL_MUL_DIV:    linha = LINHA_MD;   break;
            case CL_POT:        linha = LINHA_POT;  break;
            case CL_ABRE:       linha = LINHA_ABRE; break;
            default: return ACAO_ERRO;
        }
    }

    int coluna;
    if (entrada == NULL) {
        coluna = COL_FIM;
    } else {
        switch (classifica_operador(entrada)) {
            case CL_MAIS_MENOS: coluna = COL_MM;    break;
            case CL_MUL_DIV:    coluna = COL_MD;    break;
            case CL_POT:        coluna = COL_POT;   break;
            case CL_ABRE:       coluna = COL_ABRE;  break;
            case CL_FECHA:      coluna = COL_FECHA; break;
            default: return ACAO_ERRO;
        }
    }

    return tabela[linha][coluna];
}

static bool eh_letra(unichar c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '$';
}

static bool eh_sublinhado(unichar c) {
    return c == '_';
}

static bool eh_cifrao(unichar c) {
    return c == '$';
}

static bool eh_ponto(unichar c) {
    return c == '.';
}

static bool eh_digito(unichar c) {
    return c >= '0' && c <= '9';
}

static bool eh_espaco(unichar c) {
    return c == ' ' || c == '\t' || c == '\n';
}

Lista tokeniza(Str txt)
{
    Lista tokens = l_cria();
    int tam = s_tam(txt);
    int i = 0;

    while (i < tam) {
        unichar c = s_ch(txt, i);

        if (eh_espaco(c)) {
            i++;
        }
        else if (eh_digito(c) || eh_ponto(c)) {
            int j = i;
            while (j < tam && (eh_digito(s_ch(txt, j)) || eh_ponto(s_ch(txt, j)))) {
                j++;
            }
            Str token = s_cria_substring(txt, i, j - i);
            l_insere_fim(tokens, token);
            i = j;
        }
        else if (eh_letra(c) || eh_sublinhado(c) || eh_cifrao(c)) {
            int j = i;
            while (j < tam && (eh_letra(s_ch(txt, j)) || eh_digito(s_ch(txt, j))
                              || eh_sublinhado(s_ch(txt, j)) || eh_cifrao(s_ch(txt, j)))) {
                j++;
            }
            Str token = s_cria_substring(txt, i, j - i);
            l_insere_fim(tokens, token);
            i = j;
        }
        else {
            Str token = s_cria_substring(txt, i, 1);
            l_insere_fim(tokens, token);
            i++;
        }
    }

    return tokens;
}

Str calculadora(Str expressão)
{
    Lista tokens = tokeniza(expressão);
    Lista pilha_operandos = l_cria();
    Lista pilha_operadores = l_cria();

    int tam = l_tam(tokens);

    for(int i = 0; i < tam; i++) {
        Str token = l_dado_pos(tokens, i);
        if (classifica_operador(token) == CL_INVALIDA) {
            l_empilha(pilha_operandos, token);
        }
        else {
            Acao acao = decide_acao(l_topo(pilha_operadores), token);

            switch (acao) {
                case ACAO_EMPILHA:
                    l_empilha(pilha_operadores, token);
                    break;
                case ACAO_DESCARTA:
                    l_desempilha(pilha_operadores);
                    break;
                case ACAO_OPERA:
                    break;
                case ACAO_TERMINA:
                    break;
                case ACAO_ERRO:
                    break;
            }
        }
    }
}