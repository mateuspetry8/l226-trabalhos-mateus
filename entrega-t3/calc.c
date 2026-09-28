#include "calc.h"
#include "str.h"
#include "dicionario.h"

#include <math.h>
#include <stdio.h>

typedef enum {
    CL_MAIS_MENOS,
    CL_MUL_DIV,
    CL_POT,
    CL_ABRE,
    CL_FECHA,
    CL_IGUAL,
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
        case '=':            return CL_IGUAL;
        default:             return CL_INVALIDA;
    }
}

typedef enum {
    ACAO_ERRO,
    ACAO_EMPILHA,
    ACAO_DESCARTA,
    ACAO_OPERA,
    ACAO_TERMINA
} Acao;

enum { LINHA_V, LINHA_MM, LINHA_MD, LINHA_POT, LINHA_ABRE, LINHA_IGUAL, N_LINHAS };
enum { COL_FIM, COL_MM, COL_MD, COL_POT, COL_ABRE, COL_FECHA, COL_IGUAL, N_COLUNAS };

static const Acao tabela[N_LINHAS][N_COLUNAS] = {
    //           FIM           +-             */             ^              (              )               =
    /* V    */ { ACAO_TERMINA, ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_ERRO,     ACAO_EMPILHA},
    /* +-   */ { ACAO_OPERA,   ACAO_OPERA,    ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_OPERA,    ACAO_EMPILHA},
    /* * /  */ { ACAO_OPERA,   ACAO_OPERA,    ACAO_OPERA,    ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_OPERA,    ACAO_EMPILHA},
    /* ^    */ { ACAO_OPERA,   ACAO_OPERA,    ACAO_OPERA,    ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_OPERA,    ACAO_EMPILHA},
    /* (    */ { ACAO_ERRO,    ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_DESCARTA, ACAO_EMPILHA},
    /* =    */ { ACAO_OPERA,   ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_EMPILHA,  ACAO_OPERA,    ACAO_EMPILHA },
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
            case CL_IGUAL:      linha = LINHA_IGUAL; break;
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
            case CL_IGUAL:      coluna = COL_IGUAL;  break;
            default: return ACAO_ERRO;
        }
    }

    return tabela[linha][coluna];
}

static bool eh_letra(unichar c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
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

static bool eh_operando(Str token)
{
    if (s_tam(token) == 0) return false;
    unichar c = s_ch(token, 0);
    return eh_digito(c) || eh_ponto(c) || eh_letra(c) || eh_cifrao(c);
}

static void limpa_calculo(Lista tokens, Lista pilha_operandos, Lista pilha_operadores)
{
    while (!l_vazia(tokens)) s_destroi(l_remove_inicio(tokens));
    l_destroi(tokens);
    while (!l_vazia(pilha_operandos)) s_destroi(l_remove_inicio(pilha_operandos));
    l_destroi(pilha_operandos);
    while (!l_vazia(pilha_operadores)) s_destroi(l_remove_inicio(pilha_operadores));
    l_destroi(pilha_operadores);
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

static Dicionário variaveis = NULL;

static bool valor_operando(Str op, double *valor)
{
    if (s_tam(op) == 0) return false;

    unichar c = s_ch(op, 0);
    if (eh_digito(c) || eh_ponto(c)) {
        *valor = s_número(op);
        return true;
    }

    if (variaveis == NULL) return false;

    Str guardado = (Str) dic_busca(variaveis, op);
    if (guardado == VALOR_NÃO_EXISTE) return false;

    *valor = s_número(guardado);
    return true;
}

static bool operar(Str operador, Lista pilha_operandos)
{
    if(l_tam(pilha_operandos) < 2) return false;

    Str s2 = l_desempilha(pilha_operandos);
    Str s1 = l_desempilha(pilha_operandos);

    if (s_ch(operador, 0) == '=') {
        double valor;
        bool ok = s_tam(s1) > 0
               && (eh_letra(s_ch(s1, 0)) || eh_cifrao(s_ch(s1, 0)))
               && valor_operando(s2, &valor);
        if (ok) {
            Str chave = s_cria_cópia(s1);
            Str guardado = s_cria_número(valor);
            Str antigo = (Str) dic_insere(variaveis, chave, guardado);
            if (antigo != VALOR_NÃO_EXISTE) {
                s_destroi(antigo);  
                s_destroi(chave); 
            }
            l_empilha(pilha_operandos, s_cria_número(valor)); // cópia própria da pilha
        }

        s_destroi(s2);
        s_destroi(s1);
        return ok;
    }

    double num2, num1;
    bool ok = valor_operando(s2, &num2) && valor_operando(s1, &num1);

    s_destroi(s2);
    s_destroi(s1);

    if(!ok) return false;

    double resultado = 0.0;

    switch (s_ch(operador, 0)) {
        case '+':
            resultado = num1 + num2;
            break;
        case '-':
            resultado = num1 - num2;
            break;
        case '*':
            resultado = num1 * num2;
            break;
        case '/':
            if (num2 == 0) return false; 
            resultado = num1 / num2;
            break;
        case '^':
            resultado = pow(num1, num2);
            break;
        default:
            return false; 
    }
    Str resultado_str = s_cria_número(resultado);
    l_empilha(pilha_operandos, resultado_str);
    return true;
}

static bool chave_igual(chave_t a, chave_t b)
{
    return s_igual((Str) a, (Str) b);
}

static bool chave_menor(chave_t a, chave_t b)
{
    Str sa = (Str) a;
    Str sb = (Str) b;
    int na = s_tam(sa);
    int nb = s_tam(sb);
    int n = na < nb ? na : nb;
    for (int i = 0; i < n; i++) {
        unichar ca = s_ch(sa, i);
        unichar cb = s_ch(sb, i);
        if (ca != cb) return ca < cb;
    }
    return na < nb;
}

Str calculadora(Str expressão)
{
    if (variaveis == NULL) {
        variaveis = dic_cria(chave_menor, chave_igual);
    }

    Lista tokens = tokeniza(expressão);
    Lista pilha_operandos = l_cria();
    Lista pilha_operadores = l_cria();

    Str resultado = NULL;
    int i = 0;
    int tam = l_tam(tokens);
    bool erro = false;
    bool terminou = false;

    while(!erro && !terminou) {
        Str token = (i < tam) ? l_dado_pos(tokens, i) : NULL;
        Str topo = l_vazia(pilha_operadores) ? NULL : l_topo(pilha_operadores);

        if (token != NULL && eh_operando(token)) {
            l_empilha(pilha_operandos, s_cria_cópia(token));
            i++;
            continue;
        }

        Acao acao = decide_acao(topo, token);

        switch (acao) {
            case ACAO_EMPILHA: 
                l_empilha(pilha_operadores, s_cria_cópia(token));
                i++;
                break;
            case ACAO_DESCARTA:
                Str descartado = l_desempilha(pilha_operadores);
                s_destroi(descartado);
                i++;
                break;
            case ACAO_OPERA: {
                Str op = l_desempilha(pilha_operadores);
                if (!operar(op, pilha_operandos)) {
                    erro = true;
                }
                s_destroi(op);
                break;
            }
            case ACAO_TERMINA:
                terminou = true;
                break;
            case ACAO_ERRO:
                erro = true;
                break;
        }
    }

    double valor;
    if (!erro && l_tam(pilha_operandos) == 1 
        && valor_operando(l_topo(pilha_operandos), &valor)) {
        resultado = s_cria_número(valor);
    } else {
        resultado = s_cria("#ERRO ");
    }

    limpa_calculo(tokens, pilha_operandos, pilha_operadores);
    return resultado;
}

void le_arquivo_e_calcula()
{
    Str conteudo = s_cria_de_arquivo("entrada.txt");
    Str quebra_linha = s_cria("\n");

    Lista linhas = l_cria_separando(conteudo, quebra_linha);
    Lista saida = l_cria();

    int n = l_tam(linhas);
    for (int i = 0; i < n; i++) {
        Str linha = l_dado_pos(linhas, i);
        Str resultado = calculadora(linha);
        l_insere_fim(saida, resultado);
    }

    Str texto_saida = s_cria_unindo(saida, quebra_linha);
    s_grava_arquivo(texto_saida, "saida.txt");

    // limpeza
    s_destroi(conteudo);
    s_destroi(quebra_linha);
    s_destroi(texto_saida);

    while (!l_vazia(linhas)) s_destroi(l_remove_inicio(linhas));
    l_destroi(linhas);

    while (!l_vazia(saida)) s_destroi(l_remove_inicio(saida));
    l_destroi(saida);
}