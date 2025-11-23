#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------
// Struct que representa uma sala da mansão
// -----------------------------------------
typedef struct Sala {
    char nome[50];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// -----------------------------------------
// Função: criarSala
// Cria dinamicamente uma sala com nome
// -----------------------------------------
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));

    if(nova == NULL) {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }

    strcpy(nova->nome, nome);
    nova->esquerda = NULL;
    nova->direita = NULL;

    return nova;
}

// -----------------------------------------
// Função: explorarSalas
// Navega pela árvore com escolhas do player
// -----------------------------------------
void explorarSalas(Sala *atual) {
    char escolha;

    while(atual != NULL) {
        printf("\nVocê está agora em: %s\n", atual->nome);

        // Se não houver caminhos
        if(atual->esquerda == NULL && atual->direita == NULL) {
            printf("Você chegou ao fim! Não há mais caminhos.\n");
            return;
        }

        printf("Escolha um caminho:\n");
        if(atual->esquerda != NULL) printf(" - (e) Ir para a esquerda\n");
        if(atual->direita != NULL) printf(" - (d) Ir para a direita\n");
        printf(" - (s) Sair da exploração\n");
        printf("Opção: ");
        scanf(" %c", &escolha);

        if(escolha == 'e' || escolha == 'E') {
            if(atual->esquerda != NULL) atual = atual->esquerda;
            else printf("Não existe caminho à esquerda!\n");
        }
        else if(escolha == 'd' || escolha == 'D') {
            if(atual->direita != NULL) atual = atual->direita;
            else printf("Não existe caminho à direita!\n");
        }
        else if(escolha == 's' || escolha == 'S') {
            printf("Saindo da mansão...\n");
            return;
        }
        else {
            printf("Opção inválida!\n");
        }
    }
}

// -----------------------------------------
// Função principal: monta a árvore e inicia
// -----------------------------------------
int main() {

    // Criando manualmente a árvore binária da mansão
    Sala *hall = criarSala("Hall de Entrada");
    Sala *salaEstar = criarSala("Sala de Estar");
    Sala *cozinha = criarSala("Cozinha");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *jardim = criarSala("Jardim");
    Sala *porao = criarSala("Porão");

    // Montando estrutura da mansão
    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;

    cozinha->direita = porao;

    // Começa exploração
    printf("=== Detective Quest: Exploração da Mansão ===\n");
    explorarSalas(hall);

    return 0;
}