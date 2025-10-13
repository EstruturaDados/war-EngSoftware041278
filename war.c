// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//  Nível Novato: Cadastro Inicial dos Territórios
// ============================================================================
//
// OBJETIVOS:
// - Criar a struct Territorio.
// - Usar vetor estático de 5 elementos para armazenar os territórios.
// - Cadastrar dados via terminal.
// - Exibir o estado atual do mapa.
// - Seguir padrão modular e boas práticas de código.
//
// ============================================================================

#include <stdio.h>
#include <string.h>
#include <locale.h>

// --- Constantes Globais ---
#define NUM_TERRITORIOS 5
#define MAX_NOME 50
#define MAX_COR 30

// --- Estrutura de Dados ---
typedef struct {
    char nome[MAX_NOME];
    char corExercito[MAX_COR];
    int numeroTropas;
} Territorio;

// --- Protótipos das Funções ---
// Funções de setup e entrada de dados
void inicializarTerritorios(Territorio territorios[], int tamanho);
void limparBufferEntrada(void);

// Funções de interface
void exibirMapa(const Territorio territorios[], int tamanho);

// --- Função Principal (main) ---
int main() {
    setlocale(LC_ALL, "Portuguese");

    Territorio territorios[NUM_TERRITORIOS];

    printf("=============================================\n");
    printf("     PROJETO WAR - Cadastro de Territórios   \n");
    printf("=============================================\n\n");

    inicializarTerritorios(territorios, NUM_TERRITORIOS);

    printf("\n=============================================\n");
    printf("           ESTADO ATUAL DO MAPA              \n");
    printf("=============================================\n\n");

    exibirMapa(territorios, NUM_TERRITORIOS);

    return 0;
}

// --- Implementação das Funções ---

// inicializarTerritorios():
// Lê os dados de cada território via terminal.
void inicializarTerritorios(Territorio territorios[], int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        printf("Cadastro do Território %d\n", i + 1);

        printf("Nome do território: ");
        fgets(territorios[i].nome, sizeof(territorios[i].nome), stdin);
        territorios[i].nome[strcspn(territorios[i].nome, "\n")] = '\0';

        printf("Cor do exército: ");
        fgets(territorios[i].corExercito, sizeof(territorios[i].corExercito), stdin);
        territorios[i].corExercito[strcspn(territorios[i].corExercito, "\n")] = '\0';

        printf("Número de tropas: ");
        scanf("%d", &territorios[i].numeroTropas);
        limparBufferEntrada();

        printf("---------------------------------------------\n");
    }
}

// exibirMapa():
// Exibe o estado atual dos territórios de forma organizada.
// Usa const para garantir apenas leitura dos dados.
void exibirMapa(const Territorio territorios[], int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        printf("Território %d:\n", i + 1);
        printf(" Nome: %s\n", territorios[i].nome);
        printf(" Cor do Exército: %s\n", territorios[i].corExercito);
        printf(" Número de Tropas: %d\n", territorios[i].numeroTropas);
        printf("---------------------------------------------\n");
    }
}

// limparBufferEntrada():
// Limpa o buffer do teclado (stdin) após uso de scanf, evitando problemas com fgets.
void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}
