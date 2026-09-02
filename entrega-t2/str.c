// includes, constantes e declarações {{{1
#include "str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN_ALLOC 8    // alocação mínima

struct str {
  int nbytes;  // número de bytes válidos da codificação UTF-8
  int cap;     // capacidade alocada em bytes (potência de 2)
  byte *bytes; // sequência UTF-8; vazia => NULL
};

// A memória para conter os bytes de uma string deve ser alocada e/ou
//   realocada conforme a necessidade, cuidando para que a quantidade
//   de memória alocada seja sempre:
//   - nula (não alocada) se a string for vazia, ou
//   - não inferior ao necessário para armazenar os bytes da codificação utf8;
//   - não inferior à alocação mínima;
//   - não superior ao triplo do número de bytes necessários
//     (exceto quando for o mínimo);
//   - uma potência de 2.

// funções auxiliares {{{1

static void s_redimensiona(Str s, int novos_nbytes)
{
  assert(s != NULL);

  if (novos_nbytes == 0) {
    free(s->bytes);
    s->bytes = NULL;
    s->nbytes = 0;
    s->cap = 0;
    return;
  }

  int cap = MIN_ALLOC;
  while (cap < novos_nbytes)
    cap *= 2;

  while (cap > MIN_ALLOC && cap > 3 * novos_nbytes)
    cap /= 2;

  s->bytes = realloc(s->bytes, (size_t) cap);
  assert(s->bytes != NULL);

  s->cap = cap;
  s->nbytes = novos_nbytes;
}

static int pos_absoluta(Str_c s, int pos)
{
  if (pos < 0) {
    return s_tam(s) + 1 + pos;
  }
  return pos;
}

// verifica se a string cad está de acordo com a especificação
// aborta o programa se não tiver
static void s_ok(Str_c s)
{
  assert(s != NULL);

  if (s->bytes == NULL) {
    assert(s->nbytes == 0);
    assert(s->cap == 0);
    return;
  }

  assert(s->nbytes > 0);
  assert(s->cap >= MIN_ALLOC);
  assert(s->cap >= s->nbytes);
  assert((s->cap & (s->cap - 1)) == 0);
  assert(s->cap <= 3 * s->nbytes || s->cap == MIN_ALLOC);
  assert(u8_conta_unichar_nos_bytes(s->nbytes, s->bytes) >= 0);
}

// operações de criação e destruição {{{1

Str s_cria(char const *strC)
{
  Str s = malloc(sizeof(*s));
  assert(s != NULL);
  
  if (strC == NULL || *strC == '\0') {
    s->nbytes = 0;
    s->bytes = NULL;
    s->cap = 0;
    return s;
  }
  
  int nbytes = (int)strlen(strC);
  
  if (u8_conta_unichar_nos_bytes(nbytes, (byte *)strC) < 0) {
    s->nbytes = 0;
    s->bytes = NULL;
    s->cap = 0;
    return s;
  }
  
  s_redimensiona(s, nbytes);
  memcpy(s->bytes, strC, (size_t)nbytes);
  
  s_ok(s);
  return s;
}

void s_destroi(Str s)
{
  s_ok(s);
  free(s->bytes);
  free(s);
}

Str s_cria_substring(Str_c s, int pos, int tam)
{
  Str nova = s_cria("");
  s_substring(nova, s, pos, tam);
  return nova;
}

Str s_cria_cópia(Str_c s)
{
  return s_cria_substring(s, 0, -1);
}

// Retorna uma nova string com o conteúdo do arquivo chamado nome.
// Retorna uma string vazia em caso de erro.
Str s_cria_de_arquivo(char *nome)
{
  Str s = s_cria("");

  if (nome == NULL) {
    return s;
  }

  FILE *f = fopen(nome, "rb");
  if (f == NULL) {
    return s;
  }

  if (fseek(f, 0, SEEK_END) != 0) {
    fclose(f);
    return s;
  }

  long nbytes_l = ftell(f);
  if (nbytes_l < 0) {
    fclose(f);
    return s;
  }

  rewind(f);

  int nbytes = (int) nbytes_l;
  if (nbytes == 0) {
    fclose(f);
    return s;
  }

  byte *buf = malloc((size_t) nbytes);
  if (buf == NULL) {
    fclose(f);
    return s;
  }

  size_t lidos = fread(buf, 1, (size_t) nbytes, f);
  fclose(f);

  if (lidos != (size_t) nbytes) {
    free(buf);
    return s;
  }

  if (u8_conta_unichar_nos_bytes(nbytes, buf) < 0) {
    free(buf);
    return s;
  }

  s_redimensiona(s, nbytes);
  memcpy(s->bytes, buf, (size_t) nbytes);
  free(buf);

  s_ok(s);
  return s;
}

// operações de acesso {{{1

int s_tam(Str_c s)
{
  s_ok(s);
  return u8_conta_unichar_nos_bytes(s->nbytes, s->bytes);
}

char *s_strc(Str_c s)
{
  s_ok(s);
  char *strC = malloc((size_t) s->nbytes + 1);
  if (strC == NULL) {
    return NULL;
  }
  memcpy(strC, s->bytes, (size_t) s->nbytes);
  strC[s->nbytes] = '\0';
  return strC;
}

unichar s_ch(Str_c s, int pos)
{
  s_ok(s);

  int pos_abs = pos_absoluta(s, pos);

  if (pos_abs < 0 || pos_abs >= s_tam(s)) {
    return UNI_INV;
  }

  byte *ptr = u8_avanca_unichar(s->bytes, pos_abs);
  if (ptr == NULL) return UNI_INV;

  unichar c;
  if (u8_unichar_nos_bytes(4, ptr, &c) < 0) return UNI_INV;

  return c;
}


// operações de busca e comparação {{{1

bool s_igual(Str_c s, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  if(s->nbytes != sb->nbytes) return false;
  return memcmp(s->bytes, sb->bytes, (size_t) s->nbytes) == 0;
}

int s_busca_c(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  for(int i = pos_absoluta(s, pos); i < s_tam(s); i++) {
    unichar c = s_ch(s, i);
    for(int j = 0; j < s_tam(sb); j++) {
      if(c == s_ch(sb, j)) {
        return i;
      }
    }
  }
  return -1;
}

int s_busca_nc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  for(int i = pos_absoluta(s, pos); i < s_tam(s); i++) {
    unichar c = s_ch(s, i);
    bool achou = false;
    for(int j = 0; j < s_tam(sb); j++) {
      if(c == s_ch(sb, j)) {
        achou = true;
        break;
      }
    }
    if(!achou) {
      return i;
    }
  }
  return -1;
}

int s_busca_rc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  for(int i = pos_absoluta(s, pos); i >= 0; i--) {
    unichar c = s_ch(s, i);
    for(int j = 0; j < s_tam(sb); j++) {
      if(c == s_ch(sb, j)) {
        return i;
      }
    }
  }
  return -1;
}

int s_busca_rnc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  for(int i = pos_absoluta(s, pos); i >= 0; i--) {
    unichar c = s_ch(s, i);
    bool achou = false;
    for(int j = 0; j < s_tam(sb); j++) {
      if(c == s_ch(sb, j)) {
        achou = true;
        break;
      }
    }
    if(!achou) {
      return i;
    }
  }
  return -1;
}

int s_busca_s(Str_c s, int pos, Str_c buscada)
{
  s_ok(s);
  s_ok(buscada);
  if(s_tam(buscada) == 0) {
    return pos_absoluta(s, pos);
  }
  for(int i = pos_absoluta(s, pos); i <= s_tam(s) - s_tam(buscada); i++) {
    bool achou = true;
    for(int j = 0; j < s_tam(buscada); j++) {
      if(s_ch(s, i + j) != s_ch(buscada, j)) {
        achou = false;
        break;
      }
    }
    if(achou) {
      return i;
    }
  }
  return -1;
}


// operações de alteração {{{1

void s_substitui(Str s, int pos, int tam, Str_c sb)
{
  s_ok(s);

  Str tmp = NULL;
  if (sb == NULL) {
    tmp = s_cria("");
    sb = tmp;
  }
  s_ok(sb);

  int n = s_tam(s);
  int ini = pos_absoluta(s, pos);
  if (ini < 0) {
    ini = 0;
  } else if (ini > n) {
    ini = n;
  }

  int fim;
  if (tam < 0) {
    fim = n;
  } 
  else {
    fim = ini + tam;
    if (fim < ini) {
      fim = ini;
    }
    if (fim > n) {
      fim = n;
    }
  }

  int ini_bytes = 0;
  int fim_bytes = s->nbytes;

  if (ini > 0) {
    ini_bytes = (int) (u8_avanca_unichar(s->bytes, ini) - s->bytes);
  }
  if (fim < n) {
    fim_bytes = (int) (u8_avanca_unichar(s->bytes, fim) - s->bytes);
  }

  int prefixo = ini_bytes;
  int sufixo = s->nbytes - fim_bytes;
  int troca = sb->nbytes;
  int novo_nbytes = prefixo + troca + sufixo;

  byte *res = NULL;
  if (novo_nbytes > 0) {
    res = malloc((size_t) novo_nbytes);
    assert(res != NULL);

    memcpy(res, s->bytes, (size_t) prefixo);
    memcpy(res + prefixo, sb->bytes, (size_t) troca);
    memcpy(res + prefixo + troca, s->bytes + fim_bytes, (size_t) sufixo);
  }

  s_redimensiona(s, novo_nbytes);

  if (novo_nbytes > 0) {
    memcpy(s->bytes, res, (size_t) novo_nbytes);
    free(res);
  }

  if (tmp != NULL) {
    s_destroi(tmp);
  }

  s_ok(s);
}

void s_substring(Str s, Str_c sb, int pos, int tam)
{
  s_ok(s);
  s_ok(sb);

  int n = s_tam(sb);
  int ini = pos_absoluta(sb, pos);
  if (ini < 0) {
    ini = 0;
  } else if (ini > n) {
    ini = n;
  }

  int fim;
  if (tam < 0) {
    fim = n;
  } 
  else {
    fim = ini + tam;
    if (fim < ini) {
      fim = ini;
    }
    if (fim > n) {
      fim = n;
    }
  }

  int ini_bytes = 0;
  int fim_bytes = sb->nbytes;

  if (ini > 0) {
    ini_bytes = (int) (u8_avanca_unichar(sb->bytes, ini) - sb->bytes);
  }
  if (fim < n) {
    fim_bytes = (int) (u8_avanca_unichar(sb->bytes, fim) - sb->bytes);
  }

  int novo_nbytes = fim_bytes - ini_bytes;

  s_redimensiona(s, novo_nbytes);

  if (novo_nbytes > 0) {
    memcpy(s->bytes, sb->bytes + ini_bytes, (size_t) novo_nbytes);
  }

  s_ok(s);
}

void s_copia(Str s, Str_c sb)
{
  s_substring(s, sb, 0, -1);
}

void s_insere(Str s, int pos, Str_c sb)
{
  s_substitui(s, pos, 0, sb);
}

void s_insere_c(Str s, int pos, unichar c)
{
  s_ok(s);
  byte buf[4];
  int nbytes = u8_converte_pra_utf8(c, buf);
  if (nbytes < 0) { 
    return; 
  }
  Str tmp = s_cria((char *)buf);
  s_insere(s, pos, tmp);
  s_destroi(tmp);
}

void s_anexa(Str s, Str_c sb)
{
  s_substitui(s, -1, 0, sb);
}

void s_anexa_c(Str s, unichar c)
{
  s_insere_c(s, -1, c);
}

void s_remove(Str s, int pos, int tam)
{
  s_substitui(s, pos, tam, NULL);
}

void s_apara(Str s, Str_c sobras)
{
  s_ok(s);
  s_ok(sobras);
  //...
}

// operações de E/S {{{1

void s_imprime(Str_c s)
{
  s_ok(s);
  //...
}

void s_grava_arquivo(Str_c s, char *nome)
{
  s_ok(s);
  //...
}


// vim: foldmethod=marker shiftwidth=2

