# Parte III

## 1. Estruturas de dados

### 1.1 Inovações em vetor fixo com busca linear
- **Problema:** em `individuo.c`, `id_neurônio_entre` busca linearmente no vetor global `inovações[MAX_INOVAÇÕES]` (10000 posições).
- **Por que é problema:** o custo da busca cresce com o número de inovações, e quando o vetor fica cheio as novas inovações não são registradas, quebrando a comparação entre redes.
- **Alternativa:** tabela de dispersão (hash) com chave (id_origem, id_destino).
- **Melhoria:** busca e inserção em O(1) médio, sem limite fixo.

### 1.2 Busca linear de neurônios e sinapses por id
- **Problema:** `indivíduo_neurônio_com_id` e `indivíduo_sinapse_com_ids` percorrem todos os neurônios para achar uma posição, e são chamadas para cada sinapse em `compatibilidade` e `indivíduo_mistura_sinapses`.
- **Por que é problema:** a separação em espécies compara cada indivíduo com cada espécie, assim o custo se multiplica (indivíduos x espécies x sinapses x neurônios) e fica pior conforme maior a rede.
- **Alternativa:** dicionário id → posição em cada indivíduo (tabela de dispersão ou AVL).
- **Melhoria:** cada tradução de id vai de O(n) para O(1) médio (ou O(log n)), acelerando a etapa mais cara de cada geração.

### 1.3 Vetores de tamanho fixo
- **Problema:** `geracao.c` e `especie.c` usam vetores com tamanho máximo (`N_INDIVÍDUOS`, `MAX_ESPÉCIES`, `MAX_IND_ESP`), como o próprio código admite em comentário.
- **Por que é problema:** cada espécie reserva espaço para 500 ponteiros mesmo com poucos indivíduos, e ultrapassar o limite derruba o programa por `assert`.
- **Alternativa:** vetor dinâmico (`realloc` com crescimento por dobra).
- **Melhoria:** memória proporcional ao uso, inserção em O(1) amortizado e acesso por índice mantido (necessário para `qsort`).

### 1.4 Ordenação completa para obter só os melhores
- **Problema:** vários `qsort` por geração, embora muitas vezes só se precise do campeão ou dos 30% sobreviventes.
- **Por que é problema:** ordenar tudo custa O(n log n) quando seria suficiente selecionar os k melhores.
- **Alternativa:** fila de prioridade (heap) por pontuação.
- **Melhoria:** o melhor sai em O(1) e os k melhores em O(n + k log n). A ordenação completa continua adequada onde a ordem total é necessária.

## 2. API do grafo

1. **`g_existe_caminho(g, origem, destino)`:** como está agora `rede_insere_sinapse` insere a aresta, roda `g_tem_ciclo` no grafo inteiro e remove se houver ciclo. Verificar se já existe caminho do destino até a origem percorre só parte do grafo e ai não precisa da inserção temporária.

2. **Percurso de todas as arestas (e `g_num_arestas`):** o laço "para cada nó, para cada aresta que parte dele" se repete em `compatibilidade`, `indivíduo_mistura_sinapses`, `indivíduo_grava` e `mutação_altera_valor_sinapses`; um percurso único acaba com essa repetição.

3. **`g_clona(g)`:** a mistura e a cópia de indivíduos reinserem aresta por aresta, verificando ciclos desnecessariamente (o dominante já é acíclico); clonar o grafo é mais rápido e simples.