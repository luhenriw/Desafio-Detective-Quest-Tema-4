#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ======================================================
// Struct da sala da mansão (árvore binária comum)
// ======================================================
typedef struct Sala {
    char nome[50];
    char pista[100];       // conteúdo da pista no cômodo
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// ======================================================
// Struct de nó da BST de pistas
// ======================================================
typedef struct PistaNode {
    char conteudo[100];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

// ======================================================
// Função: criarSala()
// Cria dinamicamente um cômodo com nome e pista opcional
// ======================================================
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro ao alocar memória para sala.\n");
        exit(1);
    }

    strcpy(nova->nome, nome);

    if (pista != NULL)
        strcpy(nova->pista, pista);
    else
        strcpy(nova->pista, ""); // sem pista

    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// ======================================================
// Função: inserirPista()
// Insere automaticamente uma pista na BST de pistas
// ======================================================
PistaNode* inserirPista(PistaNode *raiz, const char *conteudo) {
    if (conteudo == NULL || strlen(conteudo) == 0)
        return raiz; // pista vazia → ignora

    if (raiz == NULL) {
        PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
        strcpy(novo->conteudo, conteudo);
        novo->esq = novo->dir = NULL;
        return novo;
    }

    if (strcmp(conteudo, raiz->conteudo) < 0)
        raiz->esq = inserirPista(raiz->esq, conteudo);
    else
        raiz->dir = inserirPista(raiz->dir, conteudo);

    return raiz;
}

// ======================================================
// Função: exibirPistas()
// Faz o percurso in-ordem (ordem alfabética)
// ======================================================
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL) return;

    exibirPistas(raiz->esq);
    printf(" - %s\n", raiz->conteudo);
    exibirPistas(raiz->dir);
}

// ======================================================
// Função: explorarSalasComPistas()
// Navega pela mansão, coleta pistas e insere na BST
// ======================================================
void explorarSalasComPistas(Sala *atual, PistaNode **arvorePistas) {
    char opcao;

    while (atual != NULL) {
        printf("\nVocê está em: %s\n", atual->nome);

        // Coleta automática da pista do cômodo
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: \"%s\"\n", atual->pista);
            *arvorePistas = inserirPista(*arvorePistas, atual->pista);
        } else {
            printf("Nenhuma pista neste cômodo.\n");
        }

        // Exibe caminhos possíveis
        printf("\nEscolha seu caminho:\n");
        if (atual->esquerda) printf(" - (e) Ir para a esquerda\n");
        if (atual->direita) printf(" - (d) Ir para a direita\n");
        printf(" - (s) Sair da exploração\n");

        printf("Opção: ");
        scanf(" %c", &opcao);

        if (opcao == 'e' || opcao == 'E') {
            if (atual->esquerda)
                atual = atual->esquerda;
            else
                printf("Não há caminho à esquerda!\n");
        }
        else if (opcao == 'd' || opcao == 'D') {
            if (atual->direita)
                atual = atual->direita;
            else
                printf("Não há caminho à direita!\n");
        }
        else if (opcao == 's' || opcao == 'S') {
            printf("\nEncerrando exploração...\n");
            return;
        }
        else {
            printf("Opção inválida!\n");
        }
    }
}

// ======================================================
// Função principal
// ======================================================
int main() {
    // -------------------------------
    // Criação do mapa fixo da mansão
    // -------------------------------
    Sala *hall        = criarSala("Hall de Entrada", "Pegadas misteriosas");
    Sala *salaEstar   = criarSala("Sala de Estar", "Um livro rasgado");
    Sala *cozinha     = criarSala("Cozinha", "Faca fora do lugar");
    Sala *biblioteca  = criarSala("Biblioteca", "Página arrancada de um diário");
    Sala *jardim      = criarSala("Jardim", NULL);
    Sala *porao       = criarSala("Porão", "Caixa secreta aberta");

    // Ligação das salas (árvore binária)
    hall->esquerda = salaEstar;
    hall->direita  = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita  = jardim;

    cozinha->direita = porao;

    // Árvore BST das pistas coletadas
    PistaNode *arvorePistas = NULL;

    printf("=== Detective Quest — Expansão: Coleta de Pistas ===\n");

    // Inicia exploração
    explorarSalasComPistas(hall, &arvorePistas);

    // ---------------------------
    // Exibir pistas coletadas
    // ---------------------------
    printf("\n=== Pistas coletadas (ordem alfabética) ===\n");
    if (arvorePistas == NULL)
        printf("Nenhuma pista coletada.\n");
    else
        exibirPistas(arvorePistas);

    printf("\nFim da investigação!\n");

    return 0;
}