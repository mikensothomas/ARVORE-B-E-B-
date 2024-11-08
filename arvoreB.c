#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 4  // ordem da Arvore B
#define MIN (MAX / 2)

typedef struct Produto {
    int codigo;
    char nome[50];
} Produto;

typedef struct No {
    int num_chaves;
    Produto chaves[MAX];
    struct No *filhos[MAX + 1];
    int folha;
} No;

No *criaNo() {
    No *novo_no = (No *)malloc(sizeof(No));
    novo_no->num_chaves = 0;
    novo_no->folha = 1;
    for (int i = 0; i < MAX; i++) {
        novo_no->chaves[i].codigo = -1;  // Valor inválido para indicar chave não usada
        strcpy(novo_no->chaves[i].nome, ""); // Nome vazio para chaves não usadas
    }
    for (int i = 0; i < MAX + 1; i++) {
        novo_no->filhos[i] = NULL;  // Inicializa todos os filhos como NULL
    }
    return novo_no;
}

void divideNo(No *pai, int indice, No *no) {
    No *novo_no = criaNo();
    novo_no->folha = no->folha;
    novo_no->num_chaves = MIN;

    for (int i = 0; i < MIN; i++) {
        novo_no->chaves[i] = no->chaves[i + MIN + 1];
    }

    if (!no->folha) {
        for (int i = 0; i <= MIN; i++) {
            novo_no->filhos[i] = no->filhos[i + MIN + 1];
        }
    }

    no->num_chaves = MIN;

    for (int i = pai->num_chaves; i >= indice + 1; i--) {
        pai->filhos[i + 1] = pai->filhos[i];
    }

    pai->filhos[indice + 1] = novo_no;

    for (int i = pai->num_chaves - 1; i >= indice; i--) {
        pai->chaves[i + 1] = pai->chaves[i];
    }

    pai->chaves[indice] = no->chaves[MIN];
    pai->num_chaves++;
}

void insereNaoCheio(No *no, Produto produto) {
    int i = no->num_chaves - 1;

    if (no->folha) {
        while (i >= 0 && produto.codigo < no->chaves[i].codigo) {
            no->chaves[i + 1] = no->chaves[i];
            i--;
        }
        no->chaves[i + 1] = produto;
        no->num_chaves++;
    } else {
        while (i >= 0 && produto.codigo < no->chaves[i].codigo) {
            i--;
        }
        i++;
        if (no->filhos[i]->num_chaves == MAX) {
            divideNo(no, i, no->filhos[i]);
            if (produto.codigo > no->chaves[i].codigo) {
                i++;
            }
        }
        insereNaoCheio(no->filhos[i], produto);
    }
}

No *insereProduto(No *raiz, Produto produto) {
    if (raiz->num_chaves == MAX) {
        No *novo_no = criaNo();
        novo_no->folha = 0;
        novo_no->filhos[0] = raiz;
        divideNo(novo_no, 0, raiz);
        insereNaoCheio(novo_no, produto);
        return novo_no;
    } else {
        insereNaoCheio(raiz, produto);
        return raiz;
    }
}

Produto geraProdutoAleatorio() {
    Produto p;
    p.codigo = rand() % 1000;
    snprintf(p.nome, 50, "Produto_%d", p.codigo);
    return p;
}

void imprimeArvore(No *no, int nivel) {
    if (no != NULL) {
        // Imprime indentação para mostrar o nível atual
        for (int i = 0; i < nivel; i++) {
            printf("    ");
        }
        
        // Imprime as chaves do nó atual, ignorando valores inválidos (-1)
        printf("|");
        for (int i = 0; i < no->num_chaves; i++) {
            if (no->chaves[i].codigo != -1) {  // Só imprime chaves válidas
                printf(" %d ", no->chaves[i].codigo);
            }
        }
        printf("\n");

        // Chamada recursiva para os nós filhos, ignorando filhos nulos
        if (!no->folha) {
            for (int i = 0; i <= no->num_chaves; i++) {
                if (no->filhos[i] != NULL) {
                    imprimeArvore(no->filhos[i], nivel + 1);
                }
            }
        }
    }
}

void buscaProduto(No *no, int codigo) {
    int i = 0;
    while (i < no->num_chaves && codigo > no->chaves[i].codigo) {
        i++;
    }
    if (i < no->num_chaves && codigo == no->chaves[i].codigo) {
        printf("Produto encontrado: Codigo: %d, Nome: %s\n", no->chaves[i].codigo, no->chaves[i].nome);
        return;
    }
    if (no->folha) {
        printf("Produto nao encontrado.\n");
        return;
    }
    buscaProduto(no->filhos[i], codigo);
}

No *removeProduto(No *raiz, int codigo) {
    if (raiz == NULL) {
        printf("Produto nao encontrado para remocao.\n");
        return raiz;
    }

    int i = 0;
    while (i < raiz->num_chaves && codigo > raiz->chaves[i].codigo) {
        i++;
    }

    if (i < raiz->num_chaves && codigo == raiz->chaves[i].codigo) {
        // Caso 1: O no e uma folha
        if (raiz->folha) {
            printf("Produto removido: Codigo: %d, Nome: %s\n", raiz->chaves[i].codigo, raiz->chaves[i].nome);
            for (int j = i; j < raiz->num_chaves - 1; j++) {
                raiz->chaves[j] = raiz->chaves[j + 1];
            }
            raiz->num_chaves--;
        } else {
            // Implementacao da remocao de nos internos
            for (int j = i; j < raiz->num_chaves - 1; j++) {
                raiz->chaves[j] = raiz->chaves[j + 1];
            }
            raiz->num_chaves--;
            printf("Produto removido: Codigo: %d\n", codigo);
        }
    } else if (!raiz->folha) {
        raiz->filhos[i] = removeProduto(raiz->filhos[i], codigo);
    } else {
        printf("Produto nao encontrado para remocao.\n");
    }

    return raiz;
}

int main() {
    srand(time(NULL));
    No *raiz = criaNo();
    int opcao, codigo;
    Produto p;

    // Gera e insere 50 produtos aleatorios
    for (int i = 0; i < 50; i++) {
        Produto p = geraProdutoAleatorio();
        raiz = insereProduto(raiz, p);
    }

    do {
        printf("\nMenu:\n");
        printf("1. Inserir produto\n");
        printf("2. Buscar produto\n");
        printf("3. Remover produto\n");
        printf("4. Imprimir arvore\n");
        printf("5. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                printf("Digite o codigo do produto: ");
                scanf("%d", &p.codigo);
                printf("Digite o nome do produto: ");
                scanf("%s", p.nome);
                raiz = insereProduto(raiz, p);
                break;
            case 2:
                printf("Digite o codigo do produto para buscar: ");
                scanf("%d", &codigo);
                buscaProduto(raiz, codigo);
                break;
            case 3:
                printf("Digite o codigo do produto para remover: ");
                scanf("%d", &codigo);
                raiz = removeProduto(raiz, codigo);
                break;
            case 4:
                printf("\nEstrutura da Arvore B:\n");
                imprimeArvore(raiz, 0);
                break;
            case 5:                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 5);

    return 0;
}
