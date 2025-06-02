#include "suporte.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TamnhoPilha 500
#define ROWS 77
#define COLS_ACTION 27
#define COLS_GOTO 19
#define MAX_LEN 50

typedef struct {
    int estadoPilha;
    token tokenPilha;
} pilha;

pilha pilhaParser[TamnhoPilha];
int top = 0;

void push(int estadoPilha, token token) {
    pilhaParser[++top].estadoPilha = estadoPilha;
    pilhaParser[top].tokenPilha = token;
}

void pop(int quantidade) { 
    top -= quantidade; 
}

int RestornaEstadoTopoPilha() { 
    return pilhaParser[top].estadoPilha; 
}

enum { 
    SHIFT = 1, 
    REDUCE = 2,
    ACCEPT = 3,
    ERROR = -1 
};

typedef struct { 
    int tipoAção;
    int valor; 
} Ação;

Ação Acao[ROWS][COLS_ACTION];
int Desvio[ROWS][COLS_GOTO];

// Função para carregar a matriz de ações do CSV
void carregarAcao(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir SLR_Actions.csv");
        exit(1);
    }
    char linha[1024];
    int row = 0;
    while (fgets(linha, sizeof(linha), file) && row < ROWS) {
        char *token_str = strtok(linha, ",\n");
        int col = 0;
        while (token_str && col < COLS_ACTION) {
            if (strcmp(token_str, "--") == 0 || strlen(token_str) == 0) {
                Acao[row][col].tipoAção = ERROR;
                Acao[row][col].valor = -1;
            } else if (token_str[0] == 's') {
                Acao[row][col].tipoAção = SHIFT;
                Acao[row][col].valor = atoi(token_str + 1);
            } else if (token_str[0] == 'r') {
                Acao[row][col].tipoAção = REDUCE;
                Acao[row][col].valor = atoi(token_str + 1);
            } else if (strcmp(token_str, "acc") == 0) {
                Acao[row][col].tipoAção = ACCEPT;
                Acao[row][col].valor = 0;
            } else {
                Acao[row][col].tipoAção = ERROR;
                Acao[row][col].valor = -1;
            }
            token_str = strtok(NULL, ",\n");
            col++;
        }
        row++;
    }
    fclose(file);
}

// Função para carregar a matriz de goto do CSV
void carregarGoto(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir SLR_Goto.csv");
        exit(1);
    }
    char linha[1024];
    int row = 0;
    while (fgets(linha, sizeof(linha), file) && row < ROWS) {
        char *token_str = strtok(linha, ",\n");
        int col = 0;
        while (token_str && col < COLS_GOTO) {
            if (strcmp(token_str, "--") == 0 || strlen(token_str) == 0) {
                Desvio[row][col] = -1;
            } else {
                Desvio[row][col] = atoi(token_str);
            }
            token_str = strtok(NULL, ",\n");
            col++;
        }
        row++;
    }
    fclose(file);
}

// Função para mapear a classe do token para o índice da matriz
int indiceToken(const char *classe) {
    if (strcmp(classe, "inicio")==0) return 0;
    if (strcmp(classe, "varinicio")==0) return 1;
    if (strcmp(classe, "varfim")==0) return 2;
    if (strcmp(classe, "escreva")==0) return 3;
    if (strcmp(classe, "leia")==0) return 4;
    if (strcmp(classe, "se")==0) return 5;
    if (strcmp(classe, "entao")==0) return 6;
    if (strcmp(classe, "fimse")==0) return 7;
    if (strcmp(classe, "faca-ate")==0) return 8;
    if (strcmp(classe, "fimfaca")==0) return 9;
    if (strcmp(classe, "fim")==0) return 10;
    if (strcmp(classe, "inteiro")==0) return 11;
    if (strcmp(classe, "literal")==0) return 12;
    if (strcmp(classe, "real")==0) return 13;
    if (strcmp(classe, "NUM")==0) return 14;
    if (strcmp(classe, "ID")==0) return 15;
    if (strcmp(classe, "Comentário")==0) return 16;
    if (strcmp(classe, "EOF")==0) return 17;
    if (strcmp(classe, "OPR")==0) return 18;
    if (strcmp(classe, "RCB")==0) return 19;
    if (strcmp(classe, "OPM")==0) return 20;
    if (strcmp(classe, "AB_P")==0) return 21;
    if (strcmp(classe, "FC_P")==0) return 22;
    if (strcmp(classe, "PT_V")==0) return 23;
    if (strcmp(classe, "VIR")==0) return 24;
    // ... completar para todos
    return -1;
}

// Função para mapear o não-terminal para o índice da matriz GOTO
int indiceNaoTerminal(const char *naoTerminal) {
    // Implemente conforme sua lista de não-terminais
    // Exemplo:
    if (strcmp(naoTerminal, "P")==0) return 0;
    if (strcmp(naoTerminal, "V")==0) return 1;
    // ... completar para todos
    return -1;
}

// Função para imprimir as produções reduzidas
void mostrarReducao(int numeroProducao) {
    switch (numeroProducao) {
        case 1: printf("P` -> P\n"); break;
        case 2: printf("P -> inicio V A\n"); break;
        case 3: printf("V -> varinicio LV\n"); break;
        case 4: printf("LV -> D LV\n"); break;
        case 5: printf("LV -> varfim LV\n"); break;
        case 6: printf("D -> L TIPO ptv\n"); break;
        case 7: printf("L -> id vir L\n"); break;
        case 8: printf("L -> id\n"); break;
        case 9: printf("TIPO -> int\n"); break;
        case 10: printf("TIPO -> real\n"); break;
        case 11: printf("TIPO -> literal\n"); break;
        case 12: printf("A -> ES A\n"); break;
        case 13: printf("ES -> leia id ptv\n"); break;
        case 14: printf("ES -> escreva ARG ptv\n"); break;
        case 15: printf("ARG -> literal\n"); break;
        case 16: printf("ARG -> num\n"); break;
        case 17: printf("ARG -> id\n"); break;
        case 18: printf("A -> CMD A\n"); break;
        case 19: printf("CMD -> id rcb LD ptv\n"); break;
        case 20: printf("LD -> OPRD opm OPRD\n"); break;
        case 21: printf("LD -> OPRD\n"); break;
        case 22: printf("OPRD -> id\n"); break;
        case 23: printf("OPRD -> num\n"); break;
        case 24: printf("A -> COND A\n"); break;
        case 25: printf("COND -> CAB CP\n"); break;
        case 26: printf("CAB -> se ab_p EXP_R fc_p entao\n"); break;
        case 27: printf("EXP_R -> OPRD opr OPRD\n"); break;
        case 28: printf("CP -> ES CP\n"); break;
        case 29: printf("CP -> CMD CP\n"); break;
        case 30: printf("CP -> COND CP\n"); break;
        case 31: printf("CP -> fimse\n"); break;
        case 32: printf("A -> R A\n"); break;
        case 33: printf("R -> facaAte ab_p EXP_R fc_p CP_R\n"); break;
        case 34: printf("CP_R -> ES CP_R\n"); break;
        case 35: printf("CP_R -> CMD CP_R\n"); break;
        case 36: printf("CP_R -> COND CP_R\n"); break;
        case 37: printf("CP_R -> fimFaca\n"); break;
        case 38: printf("A -> fim\n"); break;
        default: printf("Redução desconhecida: %d\n", numeroProducao); break;
    }
}

// Tratamento de erro modo pânico
void tratamentoErroModoPanico(token *lookahead) {
    printf("ERRO SINTÁTICO: linha %d coluna %d. Token = %s\n", linha, coluna, lookahead->Lexema);
    // Consome tokens até encontrar um ponto e vírgula ou EOF
    while (strcmp(lookahead->Classe, "PT_V") != 0 && strcmp(lookahead->Classe, "EOF") != 0) {
        *lookahead = Scanner();
    }
    printf("Recuperação modo pânico: análise retomada após o token '%s'\n", lookahead->Lexema);
}

int main() {

    carregarAcao("SLR_Actions.csv");
    carregarGoto("SLR_Goto.csv");

    pilhaParser[0].estadoPilha = 0;
    top = 0;

    token lookahead = Scanner(); // PRIMEIRO TOKEN

    while (1) {
        int estado = RestornaEstadoTopoPilha();
        int indice = indiceToken(lookahead.Classe);
        if (indice < 0) {
            tratamentoErroModoPanico(&lookahead);
            continue;
        }
        Ação act = Acao[estado][indice];

        if (act.tipoAção == SHIFT) {
            push(act.valor, lookahead);
            lookahead = Scanner();

        } else if (act.tipoAção == REDUCE) {
            int producao = act.valor;
            // Aqui você pode definir o tamanho da redução conforme sua gramática
            int producao_quantidade = 1; // Exemplo: ajuste conforme necessário
            pop(producao_quantidade);

            int estado2 = RestornaEstadoTopoPilha();
            // Você precisa mapear o número da produção para o não-terminal correto
            int naoTerminal = 0; // Ajuste conforme sua tabela de produções
            int novoEstado = Desvio[estado2][naoTerminal];
            push(novoEstado, lookahead);

            mostrarReducao(producao);

        } else if (act.tipoAção == ACCEPT) {
            printf("Análise sintática concluída com sucesso!\n");
            break;
        } else { 
            tratamentoErroModoPanico(&lookahead);
        }
    }
    return 0;
}