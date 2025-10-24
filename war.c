// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//  Nível Mestre: Modular
// ============================================================================
//
// Você receberá a missão de implementar a funcionalidade de missões estratégicas 
// individuais para cada jogador, que deverá receber, no início do jogo, uma missão 
// sorteada de forma automática entre diversas descrições pré-definidas, armazenadas 
// em um vetor de strings. Essa missão será consultada durante o jogo para verificar 
// se a condição de vitória foi atingida. A nova camada de estratégia exige organização 
// modular do código, uso de ponteiros, passagem de parâmetros por valor e referência e 
// gerenciamento adequado da memória.
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

/* ---------- PROTÓTIPOS ---------- */
void cadastrarTerritorios(Territorio *mapa, int n);
void exibirTerritorios(Territorio *mapa, int n);
void atacar(Territorio *atacante, Territorio *defensor);
void liberarMemoria(Territorio *mapa);
long lerInteiroComFgets(const char *prompt, long min, long max, int allowZero);
void lerStringComFgets(const char *prompt, char *buf, size_t bufsize);

/* Missões */
void atribuirMissao(char *destino, char *missoes[], int totalMissoes, const char *corJogador);
void exibirMissao(const char *missao); /* passagem por valor (ponteiro passado por valor) */
int verificarMissao(char *missao, Territorio *mapa, int tamanho); /* retorna 1 se cumprida, 0 caso contrário */

/* ---------- MAIN ---------- */
int main(void) {
    srand((unsigned) time(NULL));

    const int jogadores = 2; /* fixo conforme seu pedido */

    /* Ler número de territórios */
    long n;
    while (1) {
        n = lerInteiroComFgets("Informe o numero de territorios (>=1): ", 1, 1000, 0);
        if (n >= 1) break;
        printf("Por favor informe um valor inteiro >= 1.\n");
    }
    int numTerritorios = (int)n;

    /* Alocar mapa dinamicamente */
    Territorio *mapa = (Territorio *) calloc((size_t) numTerritorios, sizeof(Territorio));
    if (mapa == NULL) {
        fprintf(stderr, "Erro: falha ao alocar memoria.\n");
        return 1;
    }

    /* Cadastrar territórios (usa fgets) */
    cadastrarTerritorios(mapa, numTerritorios);

    /* Ler cores dos jogadores (assim sabemos quais cores representam cada jogador) */
    char coresJogadores[jogadores][10];
    for (int p = 0; p < jogadores; ++p) {
        char prompt[80];
        snprintf(prompt, sizeof(prompt), "Digite o nome/rotulo da COR do jogador %d: ", p + 1);
        lerStringComFgets(prompt, coresJogadores[p], sizeof(coresJogadores[p]));
    }

    /* Definir vetor de missoes (descrições simples/genéricas) */
    char *missoesPool[] = {
        "Possuir pelo menos 3 territorios",
        "Possuir ao menos 10 tropas no total",
        "Possuir pelo menos 50%% dos territorios",
        "Possuir 2 territorios consecutivos",
        "Reduzir as tropas dos oponentes a zero"
    };
    const int totalMissoes = sizeof(missoesPool) / sizeof(missoesPool[0]);

    /* Alocar dinamicamente a string de missao para cada jogador e atribuir uma aleatória */
    char *missoesJogador[jogadores];
    int missaoCumprida[jogadores];
    for (int p = 0; p < jogadores; ++p) {
        /* aloca espaço suficiente para guardar metadata + descricao */
        missoesJogador[p] = (char *) malloc(256);
        if (missoesJogador[p] == NULL) {
            fprintf(stderr, "Erro: falha ao alocar memoria para missoes.\n");
            /* liberar já alocado e mapa antes de sair */
            for (int k = 0; k < p; ++k) free(missoesJogador[k]);
            free(mapa);
            return 1;
        }
        atribuirMissao(missoesJogador[p], missoesPool, totalMissoes, coresJogadores[p]);
        missaoCumprida[p] = 0; /* inicialmente não cumprida */
    }

    /* Exibir missão de cada jogador UMA vez no início */
    printf("\n=== MISSÕES INICIAIS DOS JOGADORES ===\n");
    for (int p = 0; p < jogadores; ++p) {
        printf("Jogador %d (cor %s):\n", p + 1, coresJogadores[p]);
        exibirMissao(missoesJogador[p]); /* passagem por valor (ponteiro) */
        printf("\n");
    }

    /* Loop principal: ataques */
    while (1) {
        exibirTerritorios(mapa, numTerritorios);
        printf("\n--- Iniciar ataque ---\n");
        printf("Digite 0 para SAIR.\n");

        long indiceAtacante = lerInteiroComFgets("Selecione o numero do ATACANTE (1..N, 0 sai): ", 0, numTerritorios, 1);
        if (indiceAtacante == 0) {
            printf("Encerrando o jogo. Obrigado por jogar!\n");
            break;
        }
        int ia = (int)indiceAtacante - 1;

        /* validar tropas do atacante */
        if ((mapa + ia)->tropas <= 0) {
            printf("O territorio selecionado nao possui tropas suficientes para atacar. Escolha outro.\n");
            continue;
        }

        /* escolher defensor — validar repetidamente até correto */
        int id;
        while (1) {
            long tmp = lerInteiroComFgets("Selecione o numero do DEFENSOR (1..N): ", 1, numTerritorios, 0);
            id = (int)tmp - 1;
            if (id == ia) {
                printf("Nao pode atacar o mesmo territorio. Escolha outro defensor.\n");
                continue;
            }
            /* Não atacar territorios da mesma cor */
            if (strcmp((mapa + ia)->cor, (mapa + id)->cor) == 0) {
                printf("Nao pode atacar territorio da mesma cor. Escolha outro defensor.\n");
                continue;
            }
            break;
        }

        /* Realizar ataque (usa ponteiros) */
        atacar(mapa + ia, mapa + id);

        /* Após o ataque, verificar MISSÕES de todos os jogadores (silenciosamente).
           Se uma missão for cumprida e ainda não havia sido notificada, mostrar notificação. */
        for (int p = 0; p < jogadores; ++p) {
            if (!missaoCumprida[p]) {
                int ok = verificarMissao(missoesJogador[p], mapa, numTerritorios);
                if (ok) {
                    printf("\n*** PARABENS: Jogador %d (cor %s) CUMPRIU A MISSÃO! ***\n",
                           p + 1, coresJogadores[p]);
                    /* mostrar a descricao amigavel */
                    exibirMissao(missoesJogador[p]);
                    printf("\n");
                    missaoCumprida[p] = 1;
                    /* Fim=B => não encerramos, apenas notificamos */
                }
            }
        }

        printf("\n--- Estado apos ataque ---\n");
        exibirTerritorios(mapa, numTerritorios);
        printf("--------------------------\n\n");
    }

    /* Liberar memória */
    for (int p = 0; p < jogadores; ++p) free(missoesJogador[p]);
    liberarMemoria(mapa);
    return 0;
}

/* ---------- IMPLEMENTAÇÃO DAS FUNÇÕES ---------- */

/* Cadastra territorios (usa fgets para nomes e cores, e lerInteiroComFgets para tropas) */
void cadastrarTerritorios(Territorio *mapa, int n) {
    for (int i = 0; i < n; ++i) {
        printf("\n--- Cadastro do Territorio %d ---\n", i + 1);
        lerStringComFgets("Nome: ", (mapa + i)->nome, sizeof((mapa + i)->nome));
        lerStringComFgets("Cor (dono): ", (mapa + i)->cor, sizeof((mapa + i)->cor));
        long tropas;
        while (1) {
            tropas = lerInteiroComFgets("Tropas (numero inteiro >= 0): ", 0, 1000000, 0);
            if (tropas >= 0) break;
            printf("Valor invalido. Digite um inteiro >= 0.\n");
        }
        (mapa + i)->tropas = (int)tropas;
    }
}

/* Exibe mapa; usa ponteiros para acessar cada Territorio */
void exibirTerritorios(Territorio *mapa, int n) {
    printf("\n==== MAPA DOS TERRITORIOS ====\n");
    for (int i = 0; i < n; ++i) {
        Territorio *t = mapa + i;
        printf("[%d] %s | Cor: %s | Tropas: %d\n", i + 1, t->nome, t->cor, t->tropas);
    }
}

/* Ataque simples (um dado para cada lado 1..6) */
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
        /* move metade das tropas do atacante para o defensor */
        int metade = atacante->tropas / 2;
        /* atualiza defensor: cor e tropas (recebe a metade) */
        strncpy(defensor->cor, atacante->cor, sizeof(defensor->cor) - 1);
        defensor->cor[sizeof(defensor->cor) - 1] = '\0';
        defensor->tropas = metade;
        /* atacante perde a metade */
        atacante->tropas = atacante->tropas - metade;
    } else {
        printf("Resultado: DEFENSOR RESISTIU. Atacante perde 1 tropa.\n");
        if (atacante->tropas > 0) atacante->tropas--;
    }
}

/* Libera memória do mapa */
void liberarMemoria(Territorio *mapa) {
    free(mapa);
}

/* Atribui (sorteia) uma missão para destino. 
   A string armazenada tem formato: "ID:<id>;OWN:<corJogador>;DESC:<descricao>"
   - destino deve ter espaço suficiente (alocado pelo chamador). */
void atribuirMissao(char *destino, char *missoes[], int totalMissoes, const char *corJogador) {
    if (!destino || !missoes || totalMissoes <= 0 || !corJogador) return;
    int id = rand() % totalMissoes;
    /* montar a string com metadata para facilitar a verificação posterior */
    snprintf(destino, 256, "ID:%d;OWN:%s;DESC:%s", id, corJogador, missoes[id]);
}

/* Exibe a missão: parse e mostra apenas a descrição amigável (passagem por valor do ponteiro) */
void exibirMissao(const char *missao) {
    if (!missao) return;
    int id;
    char owner[64];
    char desc[200];
    /* parse simples: ID:<id>;OWN:<owner>;DESC:<desc> */
    if (sscanf(missao, "ID:%d;OWN:%63[^;];DESC:%199[^\n]", &id, owner, desc) >= 3) {
        printf("Missao (ID=%d) para cor '%s': %s\n", id, owner, desc);
    } else {
        /* fallback: imprime a string inteira */
        printf("Missao: %s\n", missao);
    }
}

/* Verifica se a missão está cumprida.
   A missão contém metadata (ID e OWN). A função interpreta o ID e executa a verificação
   correspondente sobre o mapa. Retorna 1 se cumprida, 0 caso contrário. */
int verificarMissao(char *missao, Territorio *mapa, int tamanho) {
    if (!missao || !mapa || tamanho <= 0) return 0;
    int id;
    char owner[64];
    char desc[200];
    if (sscanf(missao, "ID:%d;OWN:%63[^;];DESC:%199[^\n]", &id, owner, desc) < 3) {
        return 0; /* formato inválido */
    }

    /* Implementação das missões genéricas (consistentes com as descrições do pool) */
    switch (id) {
        case 0: { /* Possuir pelo menos 3 territorios */
            int cnt = 0;
            for (int i = 0; i < tamanho; ++i) {
                if (strcmp((mapa + i)->cor, owner) == 0) cnt++;
            }
            return (cnt >= 3) ? 1 : 0;
        }
        case 1: { /* Possuir ao menos 10 tropas no total */
            int soma = 0;
            for (int i = 0; i < tamanho; ++i) {
                if (strcmp((mapa + i)->cor, owner) == 0) soma += (mapa + i)->tropas;
            }
            return (soma >= 10) ? 1 : 0;
        }
        case 2: { /* Possuir pelo menos 50% dos territorios */
            int cnt = 0;
            for (int i = 0; i < tamanho; ++i) {
                if (strcmp((mapa + i)->cor, owner) == 0) cnt++;
            }
            int necessidade = (tamanho + 1) / 2; /* metade arredondada para cima */
            return (cnt >= necessidade) ? 1 : 0;
        }
        case 3: { /* Possuir 2 territorios consecutivos */
            for (int i = 0; i < tamanho - 1; ++i) {
                if (strcmp((mapa + i)->cor, owner) == 0 && strcmp((mapa + i + 1)->cor, owner) == 0) {
                    return 1;
                }
            }
            return 0;
        }
        case 4: { /* Reduzir as tropas dos oponentes a zero (para todos os territorios não do owner) */
            int somaOponentes = 0;
            for (int i = 0; i < tamanho; ++i) {
                if (strcmp((mapa + i)->cor, owner) != 0) somaOponentes += (mapa + i)->tropas;
            }
            return (somaOponentes == 0) ? 1 : 0;
        }
        default:
            return 0;
    }
}

/* ---------- UTILITÁRIOS DE LEITURA ---------- */

/* Lê inteiro via fgets e converte com strtol:
   - prompt: mensagem
   - min,max: intervalo permitido (inclusive)
   - allowZero: se 1 permite 0 como entrada válida (mesmo que min>0)
   Repete até ler valor válido (bloqueante). */
long lerInteiroComFgets(const char *prompt, long min, long max, int allowZero) {
    char buf[128];
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf("\nEntrada encerrada. Saindo.\n");
            exit(0);
        }
        /* trim leading spaces */
        char *p = buf;
        while (isspace((unsigned char)*p)) p++;
        if (*p == '\0' || *p == '\n') {
            printf("Entrada vazia. Tente novamente.\n");
            continue;
        }
        errno = 0;
        char *endptr;
        long val = strtol(p, &endptr, 10);
        if (endptr == p) {
            printf("Entrada nao reconhecida como numero. Tente novamente.\n");
            continue;
        }
        while (isspace((unsigned char)*endptr)) endptr++;
        if (*endptr != '\0' && *endptr != '\n') {
            printf("Entrada contem caracteres invalidos apos o numero. Tente novamente.\n");
            continue;
        }
        if (errno == ERANGE) {
            printf("Numero fora do intervalo. Tente novamente.\n");
            continue;
        }
        if (val == 0 && allowZero) return 0;
        if (val < min || val > max) {
            printf("Numero fora do intervalo [%ld..%ld]. Tente novamente.\n", min, max);
            continue;
        }
        return val;
    }
}

/* Lê string com fgets, remove newline, e repete se vazia */
void lerStringComFgets(const char *prompt, char *buf, size_t bufsize) {
    if (buf == NULL || bufsize == 0) return;
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, (int)bufsize, stdin) == NULL) {
            printf("\nEntrada encerrada. Saindo.\n");
            exit(0);
        }
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        if (buf[0] == '\0') {
            printf("Entrada vazia. Por favor informe um valor.\n");
            continue;
        }
        return;
    }
}
