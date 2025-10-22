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

// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

/* PROTÓTIPOS */
void cadastrarTerritorios(Territorio *mapa, int n);
void exibirTerritorios(Territorio *mapa, int n);
void atacar(Territorio *atacante, Territorio *defensor);
void liberarMemoria(Territorio *mapa);
long lerInteiroComFgets(const char *prompt, long min, long max, int allowZero);
void lerStringComFgets(const char *prompt, char *buf, size_t bufsize);

/* ---------- MAIN ---------- */
int main(void) {
    srand((unsigned) time(NULL));

    /* Ler número de territórios (somente fgets) */
    long n;
    while (1) {
        n = lerInteiroComFgets("Informe o numero de territorios (>=1): ", 1, 1000, 0);
        if (n >= 1) break;
        printf("Por favor informe um valor inteiro >= 1.\n");
    }

    /* Alocar vetor dinamicamente */
    Territorio *mapa = (Territorio *) calloc((size_t) n, sizeof(Territorio));
    if (mapa == NULL) {
        fprintf(stderr, "Erro: falha ao alocar memoria.\n");
        return 1;
    }

    cadastrarTerritorios(mapa, (int)n);

    /* Loop principal de ataque */
    while (1) {
        exibirTerritorios(mapa, (int)n);
        printf("\n----- Iniciar ataque -----\n");
        printf("Digite 0 para SAIR.\n");

        long indiceAtacante = lerInteiroComFgets("Selecione o numero do ATACANTE (1..N, 0 sai): ", 0, n, 1);
        if (indiceAtacante == 0) {
            printf("Encerrando o jogo. Obrigado por jogar!\n");
            break;
        }
        /* Converter para índice interno (0..n-1) */
        int ia = (int)indiceAtacante - 1;

        /* Validar tropa do atacante */
        if (mapa[ia].tropas <= 0) {
            printf("O territorio selecionado nao possui tropas suficientes para atacar. Escolha outro.\n");
            continue;
        }

        /* Ler defensor — repetir até válido (diferente do atacante) */
        int id;
        while (1) {
            long tmp = lerInteiroComFgets("Selecione o numero do DEFENSOR (1..N): ", 1, n, 0);
            id = (int)tmp - 1;
            if (id == ia) {
                printf("Nao pode atacar o mesmo territorio. Escolha outro defensor.\n");
                continue;
            }
            /* Não atacar territorios da mesma cor */
            if (strcmp(mapa[ia].cor, mapa[id].cor) == 0) {
                printf("Nao pode atacar territorio da mesma cor. Escolha outro defensor.\n");
                continue;
            }
            break;
        }

        /* Efetua ataque usando ponteiros */
        atacar(&mapa[ia], &mapa[id]);

        /* Mostra o resultado atualizado */
        printf("\n--- Estado apos ataque ---\n");
        exibirTerritorios(mapa, (int)n);
        printf("--------------------------\n\n");
    }

    liberarMemoria(mapa);
    return 0;
}

/*  Função para cadastrar territórios (usa fgets para ler strings e numeros).
    Aceita nomes com espaços. */
void cadastrarTerritorios(Territorio *mapa, int n) {
    char buffer[128];
    for (int i = 0; i < n; ++i) {
        printf("\n--- Cadastro do Territorio %d ---\n", i + 1);
        lerStringComFgets("Nome: ", mapa[i].nome, sizeof(mapa[i].nome));
        lerStringComFgets("Cor (dono): ", mapa[i].cor, sizeof(mapa[i].cor));

        /* Ler tropas como inteiro via fgets */
        long tropas;
        while (1) {
            tropas = lerInteiroComFgets("Tropas (numero inteiro >= 0): ", 0, 1000000, 0);
            if (tropas >= 0) break;
            printf("Valor invalido. Digite um inteiro >= 0.\n");
        }
        mapa[i].tropas = (int) tropas;
    }
}

/* Exibe todos os territórios com indices 1..N */
void exibirTerritorios(Territorio *mapa, int n) {
    printf("\n==== MAPA DOS TERRITORIOS ====\n");
    for (int i = 0; i < n; ++i) {
        printf("[%d] %s | Cor: %s | Tropas: %d\n", i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

/* Simulação de ataque:
   - Cada lado rola um dado 1..6
   - Se atacante ganha (dado atacante > dado defensor):
       transfere metade das tropas do atacante para o defensor (move)
       transfere a cor (defensor passa a ter cor do atacante)
   - Caso contrário (empate ou defensor ganha):
       atacante perde 1 tropa (se tiver > 0)
   Observação: todas as alterações são feitas via ponteiro. */
void atacar(Territorio *atacante, Territorio *defensor) {
    if (atacante == NULL || defensor == NULL) return;

    int dadoA = (rand() % 6) + 1;
    int dadoD = (rand() % 6) + 1;

    printf("\nAtaque: %s (Tropas: %d, Cor: %s) -> %s (Tropas: %d, Cor: %s)\n",
           atacante->nome, atacante->tropas, atacante->cor,
           defensor->nome, defensor->tropas, defensor->cor);

    printf("Rolagem: Atacante = %d | Defensor = %d\n", dadoA, dadoD);

    if (dadoA > dadoD) {
        printf("Resultado: ATACANTE VENCEU!\n");
        /* mover metade das tropas do atacante para o defensor */
        int metade = atacante->tropas / 2;
        /* Atualiza defensor */
        strncpy(defensor->cor, atacante->cor, sizeof(defensor->cor) - 1);
        defensor->cor[sizeof(defensor->cor) - 1] = '\0';
        defensor->tropas = metade;
        /* Atualiza atacante (perde a metade) */
        atacante->tropas = atacante->tropas - metade;
        /* Observação: se atacante tinha 1 tropa, metade = 0 -> defensor recebe 0 e atacante fica 1 */
    } else {
        printf("Resultado: DEFENSOR RESISTIU. Atacante perde 1 tropa.\n");
        if (atacante->tropas > 0) atacante->tropas--;
    }
}

/* Libera memória alocada para o mapa */
void liberarMemoria(Territorio *mapa) {
    free(mapa);
}

/* Função utilitária: lê inteiro via fgets e converte com strtol.
   - prompt: mensagem exibida ao usuário
   - min, max: limites aceitos (inclusive)
   - allowZero: quando 1, permite 0 como entrada válida (útil para opção de saída)
   Retorna o número lido (long). Em caso de entrada inválida, repete a leitura. */
long lerInteiroComFgets(const char *prompt, long min, long max, int allowZero) {
    char buf[128];
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            /* Entrada encerrada (EOF) — trata como saída */
            printf("\nEntrada encerrada. Saindo.\n");
            exit(0);
        }

        /* remover espaços iniciais/trailing */
        char *p = buf;
        while (isspace((unsigned char)*p)) p++;
        if (*p == '\0' || *p == '\n') {
            /* string vazia -> inválida (exceto talvez quando allowZero e interpretarmos vazio como 0?) */
            printf("Entrada vazia. Tente novamente.\n");
            continue;
        }

        errno = 0;
        char *endptr;
        long val = strtol(p, &endptr, 10);

        /* Verificar se algo foi convertido e não havia lixo após número (exceto newline/spaces) */
        if (endptr == p) {
            printf("Entrada nao reconhecida como numero. Tente novamente.\n");
            continue;
        }
        /* Avança sobre espaços */
        while (isspace((unsigned char)*endptr)) endptr++;
        if (*endptr != '\0' && *endptr != '\n') {
            printf("Entrada contem caracteres invalidos apos o numero. Tente novamente.\n");
            continue;
        }
        if (errno == ERANGE) {
            printf("Numero fora do intervalo. Tente novamente.\n");
            continue;
        }

        /* Validação de intervalo: se allowZero=1 e val==0, aceita mesmo que min>0 */
        if (val == 0 && allowZero) return 0;
        if (val < min || val > max) {
            printf("Numero fora do intervalo [%ld..%ld]. Tente novamente.\n", min, max);
            continue;
        }
        return val;
    }
}

/* Função utilitária: lê string com fgets e remove newline final */
void lerStringComFgets(const char *prompt, char *buf, size_t bufsize) {
    if (buf == NULL || bufsize == 0) return;
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, (int)bufsize, stdin) == NULL) {
            printf("\nEntrada encerrada. Saindo.\n");
            exit(0);
        }
        /* remove \n final, se existir */
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';

        /* aceitar string vazia? aqui exigimos não vazia */
        if (buf[0] == '\0') {
            printf("Entrada vazia. Por favor informe um valor.\n");
            continue;
        }
        return;
    }
}

