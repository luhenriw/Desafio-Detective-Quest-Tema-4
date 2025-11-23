/* detective.c
 *
 * Jogo Detective Quest - versão C
 *
 * Funcionalidades:
 * - Mansão representada por árvore binária de salas (cômodos).
 * - Ao visitar sala, mostra pista (se houver) definida por lógica do código.
 * - Pistas coletadas são armazenadas em uma BST (ordenada).
 * - Uma tabela hash associa cada pista a um suspeito.
 * - Jogador navega interativamente: esquerda (e), direita (d) ou sair (s).
 * - Ao final, lista pistas coletadas e pede o acusado; vence se ao menos
 *   duas pistas apontarem para o mesmo suspeito.
 *
 * Autor: Enigma Studios (implementação solicitada por usuário)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ----------- DEFINIÇÕES BÁSICAS ------------- */

#define HASH_SIZE 101  /* tamanho da tabela hash (primo razoável) */

/* Estrutura de uma sala (nó da árvore da mansão) */
typedef struct Sala {
    char *nome;             /* identificador único do cômodo */
    struct Sala *esq;       /* filho esquerdo */
    struct Sala *dir;       /* filho direito */
} Sala;

/* Nó da BST para armazenar pistas coletadas */
typedef struct PistaNode {
    char *pista;
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

/* Item para lista encadeada usada na tabela hash (chave: pista -> valor: suspeito) */
typedef struct HashItem {
    char *pista;
    char *suspeito;
    struct HashItem *prox;
} HashItem;

/* Tabela hash */
typedef struct {
    HashItem *buckets[HASH_SIZE];
} HashTable;

/* ----------- UTILITÁRIOS DE STRINGS --------- */

/* strdup portátil */
static char *strdup_local(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

/* transforma string para minúsculas (in-place) */
static void str_to_lower(char *s) {
    for (; *s; ++s) *s = (char)tolower((unsigned char)*s);
}

/* ----------- HASH (djb2) -------------------- */

static unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

/* Inicializa tabela hash */
static void inicializarHash(HashTable *ht) {
    for (int i = 0; i < HASH_SIZE; ++i) ht->buckets[i] = NULL;
}

/* Insere (ou substitui) associação pista -> suspeito na tabela hash.
 * Função exigida: inserirNaHash()
 */
void inserirNaHash(HashTable *ht, const char *pista, const char *suspeito) {
    if (!pista || !suspeito) return;
    unsigned long h = hash_djb2(pista) % HASH_SIZE;
    HashItem *cur = ht->buckets[h];
    while (cur) {
        if (strcmp(cur->pista, pista) == 0) {
            /* atualiza suspeito */
            free(cur->suspeito);
            cur->suspeito = strdup_local(suspeito);
            return;
        }
        cur = cur->prox;
    }
    /* não achou: cria novo item e insere no início da lista */
    HashItem *it = malloc(sizeof(HashItem));
    it->pista = strdup_local(pista);
    it->suspeito = strdup_local(suspeito);
    it->prox = ht->buckets[h];
    ht->buckets[h] = it;
}

/* Procura o suspeito associado a uma pista.
 * Função exigida: encontrarSuspeito()
 * Retorna NULL se não houver associação.
 */
char *encontrarSuspeito(HashTable *ht, const char *pista) {
    if (!pista) return NULL;
    unsigned long h = hash_djb2(pista) % HASH_SIZE;
    HashItem *cur = ht->buckets[h];
    while (cur) {
        if (strcmp(cur->pista, pista) == 0) return cur->suspeito;
        cur = cur->prox;
    }
    return NULL;
}

/* Libera memória da tabela hash */
static void liberarHash(HashTable *ht) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashItem *cur = ht->buckets[i];
        while (cur) {
            HashItem *next = cur->prox;
            free(cur->pista);
            free(cur->suspeito);
            free(cur);
            cur = next;
        }
        ht->buckets[i] = NULL;
    }
}

/* ----------- ÁRVORE DE PISTAS (BST) --------- */

/* Cria nó de pista */
PistaNode *criarPistaNode(const char *pista) {
    PistaNode *n = malloc(sizeof(PistaNode));
    n->pista = strdup_local(pista);
    n->esq = n->dir = NULL;
    return n;
}

/* Insere pista na BST de forma ordenada.
 * Função exigida: inserirPista() / adicionarPista()
 * Evita inserir duplicata textual (se já existe, não insere novamente).
 */
void inserirPista(PistaNode **root, const char *pista) {
    if (!pista) return;
    if (*root == NULL) {
        *root = criarPistaNode(pista);
        return;
    }
    int cmp = strcmp(pista, (*root)->pista);
    if (cmp == 0) {
        /* já coletada; não duplicar */
        return;
    } else if (cmp < 0) {
        inserirPista(&(*root)->esq, pista);
    } else {
        inserirPista(&(*root)->dir, pista);
    }
}

/* Impressão in-order das pistas coletadas */
void imprimirPistasInOrder(PistaNode *root) {
    if (!root) return;
    imprimirPistasInOrder(root->esq);
    printf(" - %s\n", root->pista);
    imprimirPistasInOrder(root->dir);
}

/* Conta quantas pistas na BST apontam para um suspeito dado (usando tabela hash).
 * Função auxiliar para verificação final.
 */
int contarPistasParaSuspeito(PistaNode *root, HashTable *ht, const char *suspeito) {
    if (!root) return 0;
    int count = 0;
    char *s = encontrarSuspeito(ht, root->pista);
    if (s && strcmp(s, suspeito) == 0) count = 1;
    return count + contarPistasParaSuspeito(root->esq, ht, suspeito)
                 + contarPistasParaSuspeito(root->dir, ht, suspeito);
}

/* Libera BST de pistas */
void liberarPistas(PistaNode *root) {
    if (!root) return;
    liberarPistas(root->esq);
    liberarPistas(root->dir);
    free(root->pista);
    free(root);
}

/* ----------- ÁRVORE DA MANSÃO (Salas) --------- */

/* Cria dinamicamente uma sala com nome.
 * Função exigida: criarSala()
 */
Sala *criarSala(const char *nome) {
    Sala *s = malloc(sizeof(Sala));
    s->nome = strdup_local(nome);
    s->esq = s->dir = NULL;
    return s;
}

/* Libera recursivamente a mansão (salas) */
void liberarMansao(Sala *root) {
    if (!root) return;
    liberarMansao(root->esq);
    liberarMansao(root->dir);
    free(root->nome);
    free(root);
}

/* ----------- LÓGICA QUE ASSOCIA SALAS ÀS PISTAS --------- */

/* Retorna a pista associada a um nome de sala.
 * "Pista estática definida por lógica no código", conforme requisito.
 * Retorna NULL se não houver pista nessa sala.
 */
const char *pistaParaSala(const char *nomeSala) {
    if (strcmp(nomeSala, "Entrada") == 0) {
        return "pegada lamacenta";
    } else if (strcmp(nomeSala, "Sala de Estar") == 0) {
        return "taça com manchas de vinho";
    } else if (strcmp(nomeSala, "Cozinha") == 0) {
        return "cheiro de produto de limpeza";
    } else if (strcmp(nomeSala, "Biblioteca") == 0) {
        return "livro faltando";
    } else if (strcmp(nomeSala, "Escritório") == 0) {
        return "nota rasgada";
    } else if (strcmp(nomeSala, "Quarto Mestre") == 0) {
        return "fio de cabelo loiro";
    } else if (strcmp(nomeSala, "Closet") == 0) {
        return "botão quebrado";
    } else if (strcmp(nomeSala, "Jardim") == 0) {
        return "pegadas molhadas";
    } else if (strcmp(nomeSala, "Porão") == 0) {
        return "ferramenta com manchas";
    } else {
        return NULL;
    }
}

/* ----------- EXPLORAÇÃO INTERATIVA DAS SALAS --------- */

/* Explora a mansão começando de `inicio`. Em cada sala:
 * - Exibe nome da sala.
 * - Se existir pista associada, exibe e coleta (insere na BST) e garante associação na hash já definida.
 * - Permite escolher: esquerda (e), direita (d), sair (s).
 *
 * Função exigida: explorarSalas()
 *
 * Observação: A exploração é interativa; a função retorna quando o jogador decide sair.
 */
void explorarSalas(Sala *inicio, PistaNode **pistasColetadas, HashTable *ht) {
    if (!inicio) {
        printf("Nenhuma sala para explorar.\n");
        return;
    }

    Sala *atual = inicio;
    char escolha[32];

    printf("Iniciando exploração da mansão. Comandos: 'e'=esquerda, 'd'=direita, 's'=sair\n");
    while (1) {
        printf("\nVocê está na sala: %s\n", atual->nome);
        const char *p = pistaParaSala(atual->nome);
        if (p) {
            printf("Encontrou uma pista: %s\n", p);
            /* insere na BST (evita duplicatas) */
            inserirPista(pistasColetadas, p);
        } else {
            printf("Nenhuma pista encontrada aqui.\n");
        }

        /* mostrar opções */
        printf("Opções: ");
        if (atual->esq) printf("[e] esquerda(%s) ", atual->esq->nome);
        if (atual->dir) printf("[d] direita(%s) ", atual->dir->nome);
        printf("[s] sair (terminar exploração)\n");
        printf("Escolha: ");
        if (!fgets(escolha, sizeof(escolha), stdin)) {
            printf("Erro de leitura. Saindo.\n");
            break;
        }
        /* considerar primeiro caractere não-branco */
        char c = 0;
        for (int i = 0; escolha[i]; ++i) {
            if (!isspace((unsigned char)escolha[i])) {
                c = escolha[i];
                break;
            }
        }
        if (c == 's' || c == 'S') {
            printf("Você encerrou a exploração.\n");
            break;
        } else if ((c == 'e' || c == 'E') && atual->esq) {
            atual = atual->esq;
        } else if ((c == 'd' || c == 'D') && atual->dir) {
            atual = atual->dir;
        } else {
            printf("Opção inválida ou direção inexistente. Tente novamente.\n");
        }
    }
}

/* ----------- VERIFICAÇÃO FINAL (JULGAMENTO) ------------ */

/* Conduz à fase de julgamento final:
 * - Lista pistas coletadas
 * - Solicita que jogador acuse um suspeito
 * - Verifica se ao menos duas pistas apontam para esse suspeito
 *
 * Função exigida: verificarSuspeitoFinal()
 */
void verificarSuspeitoFinal(PistaNode *pistasColetadas, HashTable *ht) {
    printf("\n----- FASE DE JULGAMENTO -----\n");
    if (!pistasColetadas) {
        printf("Você não coletou nenhuma pista. Não há como acusar com base em evidências.\n");
        return;
    }

    printf("Pistas coletadas (%s):\n", "ordem alfabética");
    imprimirPistasInOrder(pistasColetadas);

    char acusado[128];
    printf("\nQuem você acusa? Digite o nome do suspeito: ");
    if (!fgets( acusado, sizeof(acusado), stdin)) {
        printf("Erro de leitura. Encerrando.\n");
        return;
    }
    /* remove newline */
    acusado[strcspn(acusado, "\n")] = '\0';
    /* remover espaços laterais simples */
    char *start = acusado;
    while (*start && isspace((unsigned char)*start)) start++;
    if (*start == '\0') {
        printf("Nome inválido. Acusação inválida.\n");
        return;
    }

    /* contar pistas que apontam para o acusado */
    int contagem = contarPistasParaSuspeito(pistasColetadas, ht, start);
    printf("\nPistas que apontam para '%s': %d\n", start, contagem);
    if (contagem >= 2) {
        printf("Parabéns. Existem evidências suficientes (%d pistas). %s é o culpado!\n", contagem, start);
    } else {
        printf("Evidências insuficientes. Apenas %d pista(s) apontam para %s. O culpado não foi comprovado.\n", contagem, start);
    }
}

/* ----------- FUNÇÃO MAIN - MONTA MAPA FIXO E ASSOCIAÇÕES ---------- */

int main(void) {
    /* Inicializa estrutura de pistas e hash */
    PistaNode *pistasColetadas = NULL;
    HashTable ht;
    inicializarHash(&ht);

    /* Monta a mansão (árvore binária de salas) - fixo, manual */
    /*
              Entrada
             /       \
        Sala de      Cozinha
        Estar
        /   \         /   \
    Biblioteca Escritório Quarto Mestre Jardim
                       \
                       Closet
    */
    Sala *entrada = criarSala("Entrada");
    Sala *salaEstar = criarSala("Sala de Estar");
    Sala *cozinha = criarSala("Cozinha");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *escritorio = criarSala("Escritório");
    Sala *quarto = criarSala("Quarto Mestre");
    Sala *closet = criarSala("Closet");
    Sala *jardim = criarSala("Jardim");
    Sala *porao = criarSala("Porão");

    /* conexões */
    entrada->esq = salaEstar;
    entrada->dir = cozinha;
    salaEstar->esq = biblioteca;
    salaEstar->dir = escritorio;
    cozinha->esq = quarto;
    cozinha->dir = jardim;
    escritorio->dir = closet; /* escritório tem um caminho ao closet */
    /* adicionar porão ligado ao quarto (como exemplo de nó adicional) */
    quarto->esq = porao;

    /* Define associações pista -> suspeito na hash (pré-definidas) */
    /* Estas ligações devem refletir a "história". Exemplos: */
    inserirNaHash(&ht, "pegada lamacenta", "Mariana");
    inserirNaHash(&ht, "pega das molhadas", "Mariana"); /* cuidado: typo intencional evitado abaixo */
    inserirNaHash(&ht, "pegadas molhadas", "Mariana");
    inserirNaHash(&ht, "taça com manchas de vinho", "Carlos");
    inserirNaHash(&ht, "cheiro de produto de limpeza", "Ricardo");
    inserirNaHash(&ht, "livro faltando", "Ana");
    inserirNaHash(&ht, "nota rasgada", "Carlos");
    inserirNaHash(&ht, "fio de cabelo loiro", "Ana");
    inserirNaHash(&ht, "botão quebrado", "Ricardo");
    inserirNaHash(&ht, "ferramenta com manchas", "Carlos");

    /* Observação: todas as pistas definidas em pistaParaSala devem ter uma entrada na hash */
    /* Caso alguma pista não esteja na tabela, encontrarSuspeito() retornará NULL. */

    printf("=== Bem-vindo(a) a Detective Quest ===\n");
    printf("Seu objetivo: explorar a mansão, coletar pistas e acusar o culpado.\n");

    /* Exploração interativa */
    explorarSalas(entrada, &pistasColetadas, &ht);

    /* Verificação final (julgamento) */
    verificarSuspeitoFinal(pistasColetadas, &ht);

    /* liberar recursos */
    liberarPistas(pistasColetadas);
    liberarHash(&ht);
    liberarMansao(entrada);

    printf("\nObrigado por jogar Detective Quest!\n");
    return 0;
}