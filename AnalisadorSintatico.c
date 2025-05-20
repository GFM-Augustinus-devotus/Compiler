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

void pop(int count) { 
    top -= count; 
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
    int value; 
} Ação;

//São 76 estados do meu autômato LR(0) que vai de 0 a 75

Ação Acao[76][26]; // 76 estados e 26 tipos de Token dentre eles são 14 palavras reservadas.

int Desvio[76][15];      // GOTO[estado][não-terminal]


