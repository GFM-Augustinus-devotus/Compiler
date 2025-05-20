#include "suporte.h"
#include "AnalisadorLexico.c"

#define TamnhoPilha 500

/*Estruturando a pilha do Analisador Sintático*/

typedef struct {
    int estadoPilha;
    token tokenPilha;
} pilha;

pilha pilhaParser[TamnhoPilha];

int top = 0;

void push(int estadoPilha, token token) {
     pilhaParser[top++].estadoPilha = estadoPilha;
     pilhaParser[top].tokenPilha = token;
    }

void pop(int quantidade) { 
    top -= quantidade; 
}

int RestornaEstadoTopoPilha() { 
    return pilhaParser[top].estadoPilha; 
}

/*Tabela Análise SLR*/

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

//São 76 estados do meu autômato LR(0) que vai de 0 a 75

Ação Acao[76][26]; // 76 estados e 26 tipos de Token dentre eles são 14 palavras reservadas.

int Desvio[76][19];// GOTO[estado][não-terminal]. No caso são 19 não terminais


//Função Principal (Shift-Reduce)


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
// Exemplo, NÃO definitivo

void mostrarReducao(int numeroProducao) {
    // Imprima a produção igual modelo do trabalho (TABELA 1)
    switch (numeroProducao) {
    
    case 1: printf("P` -> P\n");

    case 2: printf("P -> inicio V A\n");

    case 3: printf("V -> varinicio LV\n");

    case 4: printf("LV -> D LV\n");

    case 5: printf("LV -> varfim LV\n");

    case 6: printf("D -> L TIPO ptv\n");

    case 7: printf("L -> id vir L\n");

    case 8: printf("L -> id\n");

    case 9: printf("TIPO -> int\n");

    case 10: printf("TIPO -> real\n");

    case 11: printf("TIPO -> literal\n");

    case 12: printf("A -> ES A\n");

    case 13: printf("ES -> leia id ptv\n");

    case 14: printf("ES -> escreva ARG ptv\n");

    case 15: printf("ARG -> literal\n");

    case 16: printf("ARG -> num\n");

    case 17: printf("ARG -> id\n");

    case 18: printf("A -> CMD A\n");

    case 19: printf("CMD -> id rcb LD ptv\n");

    case 20: printf("LD -> OPRD opm OPRD\n");

    case 21: printf("LD -> OPRD\n");

    case 22: printf("OPRD -> id\n");

    case 23: printf("OPRD -> num\n");

    case 24: printf("A -> COND A\n");

    case 25: printf("COND -> CAB CP\n");

    case 26: printf("CAB -> se ab_p EXP_R fc_p entao\n ");

    case 27: printf("EXP_R -> OPRD opr OPRD\n");

    case 28: printf("CP -> ES CP\n");

    case 29: printf("CP -> CMD CP\n");

    case 30: printf("CP -> COND CP\n");

    case 31: printf("CP -> fimse\n");

    case 32: printf("A -> R A\n");

    case 33: printf("R -> facaAte ab_p EXP_R fc_p CP_R\n");

    case 34: printf("CP_R -> ES CP_R\n");

    case 35: printf("CP_R -> CMD CP_R\n");

    case 36: printf("CP_R -> COND CP_R\n");

    case 37: printf("CP_R -> fimFaca\n");

    case 38: printf("A -> fim");

    break;
    // ... completar para demais regras
    }
}

int main() {
    pilhaParser[0].estadoPilha = 0;
    top = 0;

    token lookahead = Scanner(); // PRIMEIRO TOKEN

    while (1) {
        int estado = RestornaEstadoTopoPilha();
        int indice = indiceToken(lookahead.Classe);
        Ação act = Acao[estado][indice];

        if (act.tipoAção == SHIFT) {
            push(act.valor, lookahead);
            lookahead = Scanner();

        } else if (act.tipoAção == REDUCE) {

            int producao = act.valor; // Número da produção
            int producao_quantidade = 1;                              //-----> Redução ocorre de 1 em 1
            pop(producao_quantidade); // desempilha

            int estado2 = RestornaEstadoTopoPilha();
            int naoTerminal = 1;                                     //-----> Ajustar, como vou identificar os não terminais?
            int novoEstado = Desvio[estado2][naoTerminal]; 
            push(novoEstado, lookahead); // empilha novo estado

            mostrarReducao(producao);

        } else if (act.tipoAção == ACCEPT) {
            printf("Análise sintática concluída com sucesso!\n");
            break;
        } else { 
            printf("ERRO SINTÁTICO: linha %d coluna %d. Token = %s\n", linha, coluna, lookahead.Lexema);
            // Chamar rotina de recuperação de erro
            break;
        }
    }
    return 0;
}


