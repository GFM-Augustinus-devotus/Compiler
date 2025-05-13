#ifndef  SUPORTE_H
#define  SUPORTE_H


    #include <stdio.h>
    #include <malloc.h>
    #include <stdarg.h>
    #include <unistd.h>
    #include <string.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <math.h>
    #include <ctype.h>

    typedef struct token {
        char Classe[30];
        char Lexema[30];
        char Tipo[30]; 
    } token;

    typedef struct TabelaDeSimbolos Hash;
    
    Hash* criarTabela(int tamanhoTabela);

    int chaveLexemaToken(char * lexema);

    int chaveDivisao(int chave, int tamanhoTabela);

    int sondagemLinear(int posicao, int indice, int tamanhoTabela);

    int buscaTokenTabela(Hash *tabela, char *lexema);

    void insereTokenTabela(Hash *tabela, struct token t1);

    int atualizaTokenTabela(Hash *tabela, char *lexema1, char *lexema2);

    token Scanner();

#endif  