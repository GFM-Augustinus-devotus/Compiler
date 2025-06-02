#include "suporte.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_TAMANHO 10 
#define TamnhoPilha 500
#define ROWS 77
#define COLS_ACTION 27
#define COLS_GOTO 19
#define MAX_LEN 50

FILE *Arquivo;

struct TabelaDeSimbolos{
    struct token **itens;
    int tamanhoTabela, quantidadeItens;
};

Hash* criarTabela(int tamanhoTabela) {
    
    Hash* tabela = (Hash*) malloc(sizeof(Hash));
    if (tabela != NULL) {

        tabela->tamanhoTabela = tamanhoTabela;
        tabela->quantidadeItens = 0; 
        tabela->itens = (struct token**) malloc(tamanhoTabela * sizeof(token*));

        if (tabela->itens == NULL) {
            free(tabela);
            return NULL; 
        }

    }else{
        printf("\n Não foi possível criar a tabela de símbolos\n");
    }
    return tabela;
}

int chaveLexemaToken(char * lexema){
    int i, chave = 7;
    int tam = strlen(lexema);
    for(i = 0; i < tam; i++){
        chave = 31 * chave + (int) lexema[i];
    }
    return chave;
}

int sondagemLinear(int posicao, int indice, int tamanhoTabela){
    return ((posicao + indice) & 0x7FFFFFFF) % tamanhoTabela;
}


int chaveDivisao(int chave, int tamanhoTabela){
    
    return (chave & 0x7FFFFFFF) % tamanhoTabela;
}


int buscaTokenTabela(Hash* tabela, char *lexema) {
    if (tabela == NULL) {
        printf("A tabela de símbolos não existe\n");
        return 0;
    }

    int posicao = chaveDivisao(chaveLexemaToken(lexema), tabela->tamanhoTabela);

    for (int i = 0; i < tabela->tamanhoTabela; i++) {
        int nova_posicao = sondagemLinear(posicao, i, tabela->tamanhoTabela);

        if (tabela->itens[nova_posicao] == NULL) {
            
            break;
        }

        if (strcmp(tabela->itens[nova_posicao]->Lexema, lexema) == 0) {

            printf("Classe: %s Lexema: %s Tipo: %s\n\n",
                   tabela->itens[nova_posicao]->Classe,
                   tabela->itens[nova_posicao]->Lexema,
                   tabela->itens[nova_posicao]->Tipo);
            return 1; 
        }
    }

    return 0;
}

void insereTokenTabela(Hash *tabela, struct token t1){
    
    if(tabela == NULL){
        printf("A tabela de síbolos não existe\n");
        return;
    }else if (tabela->quantidadeItens == tabela->tamanhoTabela){
        printf("A tabela está cheia e não pode ser acrecentado mais itens\n");
        return;
    }

    int chave = chaveLexemaToken(t1.Lexema);
    int i, posicao, nova_posicao;

    posicao = chaveDivisao(chave , tabela->tamanhoTabela);

    for(i=0; i<tabela->tamanhoTabela; i++){
        nova_posicao = sondagemLinear(posicao , i , tabela->tamanhoTabela);

        if(tabela->itens[nova_posicao] == NULL){
            struct token* novo;
            novo = (struct token*) malloc(sizeof(struct token));

            if(novo == NULL){
                printf("Erro ao armazenar o token na tabela de símbolos\n");
                return;

            }else{
                *novo = t1;
                tabela->itens[nova_posicao] = novo;
                tabela->quantidadeItens++;
                return;
            }
        }
    }
    printf("Erro ao armazenar o token na tabela de símbolos\n");
    return;
}


//------------------------------------------

char buffer1[BUFFER_TAMANHO];
char buffer2[BUFFER_TAMANHO];
char *forward;
char *lexemeBegin;
int indentificadorBuffer = 1;
FILE *Arquivo;
int fimDoArquivo = 0;
int linha = 1;
int coluna = 0;
int *linhaAtual;
int *colunaAtual;

void loadBuffer() {
    size_t bytesRead;
    if (indentificadorBuffer == 1) {
        bytesRead = fread(buffer1, 1, BUFFER_TAMANHO, Arquivo);
        buffer1[bytesRead] = EOF; 
        forward = buffer1;
        lexemeBegin = buffer1;
        indentificadorBuffer = 2;
    } else {
        bytesRead = fread(buffer2, 1, BUFFER_TAMANHO, Arquivo);
        buffer2[bytesRead] = EOF; 
        forward = buffer2;
        lexemeBegin = buffer2;
        indentificadorBuffer = 1;
    }
    if (bytesRead < BUFFER_TAMANHO) {
        fimDoArquivo = 1;
    }
}

char getNextChar() {
    if (*forward == EOF) {
        if (fimDoArquivo) {
            return EOF;
        } else {
            loadBuffer();
            return *forward++;
        }
    }

    char caracterApontado = *forward++;

    if (caracterApontado == '\n') {
        linha++;
        coluna = 0;
    } else {
        coluna++;
    }

    return caracterApontado;
}

void retract() {
    forward--;
    coluna--; 
}

char *scanSuporte(int *linhaAtual, int *colunaAtual) {
    char currentChar = getNextChar();

    if (currentChar == EOF) {
        return NULL; 
    }

    char *caracter = (char *)malloc(1);
    if (caracter == NULL) {
        perror("Erro ao alocar memória para o lexema");
        exit(EXIT_FAILURE);
    }

    caracter[0] = currentChar; 


    *linhaAtual = linha;
    *colunaAtual = coluna;

    return caracter;
}

int Error(token erro){

    if(strcmp(erro.Classe , "ERRO 1") == 0){
        printf("ID declarado de forma inálida. Linha: %d , Coluna: %d\n\n", linha , coluna);
        return 1;
    }

    if(strcmp(erro.Classe , "ERRO 2") == 0){
        printf("Literal incompleto. Linha: %d , Coluna: %d\n\n", linha , coluna);
        return 1;
    }

    if(strcmp(erro.Classe, "ERRO 3") ==0){
        printf("Valor numérico declarado de forma errada. Linha: %d , Coluna: %d \n\n", linha, coluna);
        return 1;
    }

    if(strcmp(erro.Classe, "ERRO 4") ==0){
        printf("Caractere inválido para a linguagem. Linha: %d , Coluna: %d \n\n", linha, coluna);
        return 1;
    }

    if(strcmp(erro.Classe, "ERRO 5") ==0){
        printf("Caractere inválido para fazer comentários. Linha: %d , Coluna: %d \n\n", linha, coluna);
        return 1;
    }

    if(strcmp(erro.Classe, "ERRO 6") ==0){
        printf("Comentário que não termina com }. Linha: %d , Coluna: %d \n\n", linha, coluna);
        return 1;
    }

    if(strcmp(erro.Classe, "ERRO 7") ==0){
        printf("Numeral incompleto. Linha: %d , Coluna: %d \n\n", linha, coluna);
        return 1;
    }

    return 0;
}


int estado = 0;        
token Scanner(){

    token retornaToken;
    int indice = 0;
    int x = 1;
    char *c;
    char lexema[25];
    memset(lexema, 0, sizeof(lexema));
    memset(retornaToken.Classe, 0, sizeof(retornaToken.Classe));
    memset(retornaToken.Lexema, 0, sizeof(retornaToken.Lexema));
    memset(retornaToken.Tipo, 0, sizeof(retornaToken.Tipo));
    
    if(estado != 5 && estado != 6 && estado != 7 && estado != 8 && estado != 9 && estado != 19) {
        c = scanSuporte(&linha, &coluna);
    }

    while(x == 1){

        if(c == NULL){
            strcpy(retornaToken.Classe , "EOF");
            strcpy(retornaToken.Lexema , "EOF");
            strcpy(retornaToken.Tipo , "NULO");
            x = 0;
            break;
    
        }

        if(isalpha(c[0]) && estado != 4){

            estado = 1;

        }else if(c[0] == ';' && lexema[0] == '\0'){

            estado = 2;

        }else if(estado != 2 && ((c[0] == '\n' && lexema[0] == '\0') || (c[0] == ' ' && lexema[0] == '\0') || (c[0] == '\t' && lexema[0] == '\0'))){
            
            estado = 3;

        }else if (c[0] == '"'){
           
            estado = 4;
        
        }else if (c[0] == '('){

            estado = 5;

        }else if (c[0] == ')'){
            
            estado = 6;

        }else if (c[0] == '>'){
            
            estado = 7;

        }else if(c[0] == '<'){

            estado = 8;
        
        }else if(c[0] == '+' || c[0] == '-' || c[0] == '*' || c[0] == '/'){

            estado = 9;
        
        }else if(c[0] == ','){

            estado = 10;
        
        }else if(c[0] == '!' || c[0] == '@' || c[0] == '#' || c[0] == '$' || c[0] == '%'  || c[0] == '&' || c[0] == '|' || c[0] == '^' || c[0] == '~' || c[0] == '[' ||c[0] == ']' || c[0] == '?' ){

            estado = 11;
        
        }else if(c[0] == '{'){
            
            estado = 12; 
        
        }else if(isdigit(c[0])){

            estado = 19;
        
        }

        switch (estado){
        
            case 1:

                if(isalpha(c[0]) || isdigit(c[0]) || c[0] == '_'){
                    estado = 1;
                    lexema[indice] = c[0];
                    indice++;
                    c = scanSuporte(&linha, &coluna);
                }
                if(c[0] == ' ' ||  c[0] == '\n' || c[0] == '\t' || c[0] == ';' || c[0] == '(' || c[0] == ')' || c[0] == '>' || c[0] == '<' || c[0] == '+' ||  c[0] == '*' ||  c[0] == '/'){
                    strcpy(retornaToken.Classe , "ID");
                    strcpy(retornaToken.Lexema , lexema);
                    strcpy(retornaToken.Tipo , "NULO");

                    if(c[0] == ';') estado = 2;
                    if(c[0] == '\n' || c[0] == ' ' || c[0] == '\t') estado = 3;
                    if(c[0] == '(') estado = 5;
                    if(c[0] == ')') estado = 6;
                    if(c[0] == '>') estado = 7;
                    if(c[0] == '<') estado = 8;
                    if(c[0] == '+') estado = 9;
                    if(c[0] == '-') estado = 9;
                    if(c[0] == '*') estado = 9;
                    if(c[0] == '/') estado = 9;
                    x = 0;
                    break;
                }else if(isalpha(c[0]) || isdigit(c[0]) || c[0] == '_'){
                    estado = 1;
                    break;
                }else{
                    if(c[0] == '-'){
                        lexema[indice] = c[0];
                        indice++;
                        estado = 1;
                        c = scanSuporte(&linha, &coluna);
                        break;
                    }
                    strcpy(retornaToken.Classe , "ERRO 1");
                    strcpy(retornaToken.Lexema , lexema);
                    strcpy(retornaToken.Tipo , "NULO");
                    x = 0;
                    break;
                }
            break;

        case 2:
                strcpy(retornaToken.Classe , "PT_V");
                strcpy(retornaToken.Lexema , " ; ");
                strcpy(retornaToken.Tipo , "NULO");
                estado = 0;
                x = 0;
                break;
        
        case 3:
                x = 0;
                break;

        case 4: 
            
            while(c[0] != '"'){
                if(c == NULL){
                    strcpy(retornaToken.Classe , "ERRO 2");
                    strcpy(retornaToken.Lexema , lexema);
                    strcpy(retornaToken.Tipo , "NULO");
                    x = 0;
                    break;
                }

                lexema[indice] = c[0];
                indice++;
                estado = 4;
                c = scanSuporte(&linha, &coluna);     
                break;

            }if(c != NULL){
                if(c[0] == '"' && lexema[0] != '\0'){
                    lexema[indice] = c[0];
                    strcpy(retornaToken.Classe , "Lit");
                    strcpy(retornaToken.Lexema , lexema);
                    strcpy(retornaToken.Tipo , "NULO");
                    estado = 0;     
                    x = 0;
                    break;
                    
                }else{
                    lexema[indice] = c[0];
                    indice++;
                    c = scanSuporte(&linha, &coluna);
                    estado = 4;
                    break;
                }
            }else{
                strcpy(retornaToken.Classe , "ERRO 2");
                strcpy(retornaToken.Lexema , lexema);
                strcpy(retornaToken.Tipo , "NULO");
                x = 0;
                break;
            }
           

        case 5:

            strcpy(retornaToken.Classe , "AB_P");
            strcpy(retornaToken.Lexema , "(");
            strcpy(retornaToken.Tipo , "NULO");
            x = 0;
            estado = 0;
            break;
        
        case 6:

            strcpy(retornaToken.Classe , "FC_P");
            strcpy(retornaToken.Lexema , ")");
            strcpy(retornaToken.Tipo , "NULO");
            x = 0;
            estado = 0;
            break;

        case 7:
            c = scanSuporte(&linha, &coluna); 
            if(c[0] == '='){
                strcpy(retornaToken.Classe , "OPR");
                strcpy(retornaToken.Lexema , ">=");
                strcpy(retornaToken.Tipo , "NULO");
                estado = 0;
                x = 0;
                break;
            }else{
                strcpy(retornaToken.Classe , "OPR");
                strcpy(retornaToken.Lexema , ">");
                strcpy(retornaToken.Tipo , "NULO");
                estado = 7;
                x = 0;
                break;
            }

        case 8:
        
        c = scanSuporte(&linha, &coluna); 
        if(c[0] == '='){
            strcpy(retornaToken.Classe , "OPR");
            strcpy(retornaToken.Lexema , "<=");
            strcpy(retornaToken.Tipo , "NULO");
            estado = 0;
            x = 0;
            break;
        }else if(c[0] == '-'){
            strcpy(retornaToken.Classe , "RCB");
            strcpy(retornaToken.Lexema , "<-");
            strcpy(retornaToken.Tipo , "NULO");
            estado = 0;
            x = 0;
            break;
        }else{
            strcpy(retornaToken.Classe , "OPR");
            strcpy(retornaToken.Lexema , "<");
            strcpy(retornaToken.Tipo , "NULO");
            estado = 8;
            x = 0;
            break;
        }

        case 9:
            if(strcmp(lexema , "faca") == 0){
                estado = 1;
                x = 0;
                break;
            }
            strcpy(retornaToken.Classe , "OPM");
            strcpy(retornaToken.Lexema , c);
            strcpy(retornaToken.Tipo , "NULO");
            estado = 0;
            x = 0;
            break;

        case 10:

            strcpy(retornaToken.Classe , "VIR");
            strcpy(retornaToken.Lexema , " , ");
            strcpy(retornaToken.Tipo , "NULO");
            estado = 0;
            x = 0;
            break;
        
        case 11:

        strcpy(retornaToken.Classe , "ERRO 4");
        strcpy(retornaToken.Lexema , c);
        strcpy(retornaToken.Tipo , "NULO");
        estado = 0;
        x = 0;
        break;

        case 12:
        lexema[indice] = c[0];
        indice++;
        
        while(c[0] != '}'){

        c = scanSuporte(&linha, &coluna);
            
        if(c == NULL){
            strcpy(retornaToken.Classe , "ERRO 6");
            strcpy(retornaToken.Lexema , lexema);
            strcpy(retornaToken.Tipo , "NULO");
            return retornaToken;

        }

            if(c[0] == '!' || c[0] == '@' || c[0] == '#' || c[0] == '$' || c[0] == '%'  || c[0] == '&' || c[0] == '|' || c[0] == '^' || c[0] == '~' || c[0] == '[' ||c[0] == ']' || c[0] == '?' ){
                strcpy(retornaToken.Classe , "ERRO 5");
                strcpy(retornaToken.Lexema , c);
                strcpy(retornaToken.Tipo , "NULO");
                return retornaToken;

            }else if(c == NULL){
                    strcpy(retornaToken.Classe , "ERRO 6");
                    strcpy(retornaToken.Lexema , lexema);
                    strcpy(retornaToken.Tipo , "NULO");
                    return retornaToken;
    
                }
                lexema[indice] = c[0];
                indice++;
                estado = 12;
            }
                strcpy(retornaToken.Classe , "Comentário");
                strcpy(retornaToken.Lexema , lexema);
                strcpy(retornaToken.Tipo , "NULO");
                estado = 0;
                x = 0;
                break;

        case 19:

            lexema[indice] = c[0];
            indice++;

            c = scanSuporte(&linha, &coluna);
            while(isdigit(c[0])){
                lexema[indice] = c[0];
                indice++;
                c = scanSuporte(&linha, &coluna); 
            }

            if(c[0] == ' ' || c[0] == ')' || c[0] == ';'){
                strcpy(retornaToken.Classe , "NUM");
                strcpy(retornaToken.Lexema , lexema);
                strcpy(retornaToken.Tipo , "inteiro");
                x = 0;
                break;

            }else if(c[0] == '.'){
                lexema[indice] = c[0];
                indice++;
                c = scanSuporte(&linha, &coluna); 

                if(!isdigit(c[0])){
                    strcpy(retornaToken.Classe , "ERRO 7");
                    strcpy(retornaToken.Lexema , lexema);
                    strcpy(retornaToken.Tipo , "NULO");
                    return retornaToken;
    
                }
                
                while(isdigit(c[0])){
                    
                    lexema[indice] = c[0];
                    indice++;
                    c = scanSuporte(&linha, &coluna); 
                }

                if(c[0] == 'E' || c[0] == 'e'){
                    lexema[indice] = c[0];
                    indice++;
                    c = scanSuporte(&linha, &coluna);

                    while(isdigit(c[0])){
                        lexema[indice] = c[0];
                        indice++;
                        c = scanSuporte(&linha, &coluna); 
                    }

                    if(c[0] == '+' || c[0] == '-'){
                    lexema[indice] = c[0];
                    indice++;
                    c = scanSuporte(&linha, &coluna);

                    while(isdigit(c[0])){
                        lexema[indice] = c[0];
                        indice++;
                        c = scanSuporte(&linha, &coluna); 
                        }
                    }
                }
                lexema[indice] = c[0];
                indice++;
                strcpy(retornaToken.Classe , "NUM");
                strcpy(retornaToken.Lexema , lexema);
                strcpy(retornaToken.Tipo , "real");
                estado = 0;
                x = 0;
                break;

            }else{

                
            }
        
        default:
            x = 0;
            break;
        }
    }
    return retornaToken;
}

void printarToken(token token){
    if (strcmp(token.Classe , "\0") == 0 || strcmp(token.Classe, "Comentário") == 0)  return;

    printf("Classe: %s  Lexema: %s  Tipo: %s\n\n", token.Classe , token.Lexema, token.Tipo);
    return;
}

void printTabelaDeSimbolos(Hash *tabela) {
    if (tabela == NULL) {
        printf("A tabela de símbolos está vazia ou não foi inicializada.\n");
        return;
    }

    printf("\n--- Tabela de Símbolos ---\n");
    for (int i = 0; i < tabela->tamanhoTabela; i++) {
        if (tabela->itens[i] != NULL) { 
            printf("Posição %d:\n", i);
            printf("  Classe: %s\n", tabela->itens[i]->Classe);
            printf("  Lexema: %s\n", tabela->itens[i]->Lexema);
            printf("  Tipo: %s\n", tabela->itens[i]->Tipo);
        }
    }
    printf("--- Fim da Tabela de Símbolos ---\n");
}

const int TAMANHODATABELA = 103;

void chamandoLexico(){
    Hash* tabela = criarTabela(TAMANHODATABELA);


    token t1 = {"inicio", "inicio", "inicio"};
    insereTokenTabela(tabela , t1);
    token t2 = {"varinicio", "varinicio", "varinicio"};
    insereTokenTabela(tabela , t2);
    token t3 = {"varfim", "varfim", "varfim"};
    insereTokenTabela(tabela , t3);
    token t4 = {"escreva", "escreva", "escreva"};
    insereTokenTabela(tabela , t4);
    token t5 = {"leia", "leia", "leia"};
    insereTokenTabela(tabela , t5);
    token t6 = {"se", "se", "se"};
    insereTokenTabela(tabela , t6);
    token t7 = {"entao", "entao", "entao"};
    insereTokenTabela(tabela , t7);
    token t8 = {"fimse", "fimse", "fimse"};
    insereTokenTabela(tabela , t8);
    token t9 = {"faca-ate", "faca-ate", "faca-ate"};
    insereTokenTabela(tabela , t9);
    token t10 = {"fimfaca", "fimfaca", "fimfaca"};
    insereTokenTabela(tabela , t10);
    token t11 = {"fim", "fim", "fim"};
    insereTokenTabela(tabela , t11);
    token t12 = {"inteiro", "inteiro", "inteiro"};
    insereTokenTabela(tabela , t12);
    token t13 = {"literal", "literal", "literal"};
    insereTokenTabela(tabela , t13);
    token t14 = {"real", "real", "real"};
    insereTokenTabela(tabela , t14);

    printf("\n\n");

     Arquivo = fopen("FONTE.ALG.txt", "r");
    if (Arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    loadBuffer();

    int condicao = 1;

    while (condicao == 1) {
        int existe = 0;
            token retornaToken = Scanner();

            if(strcmp(retornaToken.Classe , "ID") == 0) {

                if(strcmp(retornaToken.Lexema , t1.Lexema) == 0){

                    strcpy(retornaToken.Classe , t1.Classe);
                    strcpy(retornaToken.Lexema , t1.Lexema);
                    strcpy(retornaToken.Tipo , t1.Tipo);

                }else if(strcmp(retornaToken.Lexema , t2.Lexema) == 0){

                    strcpy(retornaToken.Classe , t2.Classe);
                    strcpy(retornaToken.Lexema , t2.Lexema);
                    strcpy(retornaToken.Tipo , t2.Tipo);

                }else if(strcmp(retornaToken.Lexema , t3.Lexema) == 0){

                    strcpy(retornaToken.Classe , t3.Classe);
                    strcpy(retornaToken.Lexema , t3.Lexema);
                    strcpy(retornaToken.Tipo , t3.Tipo);

                }else if(strcmp(retornaToken.Lexema , t4.Lexema) == 0){

                    strcpy(retornaToken.Classe , t4.Classe);
                    strcpy(retornaToken.Lexema , t4.Lexema);
                    strcpy(retornaToken.Tipo , t4.Tipo);

                }else if(strcmp(retornaToken.Lexema , t5.Lexema) == 0){

                    strcpy(retornaToken.Classe , t5.Classe);
                    strcpy(retornaToken.Lexema , t5.Lexema);
                    strcpy(retornaToken.Tipo , t5.Tipo);

                }else if(strcmp(retornaToken.Lexema , t6.Lexema) == 0){

                    strcpy(retornaToken.Classe , t6.Classe);
                    strcpy(retornaToken.Lexema , t6.Lexema);
                    strcpy(retornaToken.Tipo , t6.Tipo);

                }else if(strcmp(retornaToken.Lexema , t7.Lexema) == 0){

                    strcpy(retornaToken.Classe , t7.Classe);
                    strcpy(retornaToken.Lexema , t7.Lexema);
                    strcpy(retornaToken.Tipo , t7.Tipo);

                }else if(strcmp(retornaToken.Lexema , t8.Lexema) == 0){

                    strcpy(retornaToken.Classe , t8.Classe);
                    strcpy(retornaToken.Lexema , t8.Lexema);
                    strcpy(retornaToken.Tipo , t8.Tipo);

                }else if(strcmp(retornaToken.Lexema , t9.Lexema) == 0){

                    strcpy(retornaToken.Classe , t9.Classe);
                    strcpy(retornaToken.Lexema , t9.Lexema);
                    strcpy(retornaToken.Tipo , t9.Tipo);

                }else if(strcmp(retornaToken.Lexema , t10.Lexema) == 0){

                    strcpy(retornaToken.Classe , t10.Classe);
                    strcpy(retornaToken.Lexema , t10.Lexema);
                    strcpy(retornaToken.Tipo , t10.Tipo);

                }else if(strcmp(retornaToken.Lexema , t11.Lexema) == 0){

                    strcpy(retornaToken.Classe , t11.Classe);
                    strcpy(retornaToken.Lexema , t11.Lexema);
                    strcpy(retornaToken.Tipo , t11.Tipo);

                }else if(strcmp(retornaToken.Lexema , t12.Lexema) == 0){

                    strcpy(retornaToken.Classe , t12.Classe);
                    strcpy(retornaToken.Lexema , t12.Lexema);
                    strcpy(retornaToken.Tipo , t12.Tipo);

                }else if(strcmp(retornaToken.Lexema , t13.Lexema) == 0){

                    strcpy(retornaToken.Classe , t13.Classe);
                    strcpy(retornaToken.Lexema , t13.Lexema);
                    strcpy(retornaToken.Tipo , t13.Tipo);

                }else if(strcmp(retornaToken.Lexema , t14.Lexema) == 0){

                    strcpy(retornaToken.Classe , t14.Classe);
                    strcpy(retornaToken.Lexema , t14.Lexema);
                    strcpy(retornaToken.Tipo , t14.Tipo);

                }else{
                    existe = buscaTokenTabela(tabela, retornaToken.Lexema);
                    if(existe == 0){
                        insereTokenTabela(tabela , retornaToken);
                    }
                }
            }

            if(existe == 0) printarToken(retornaToken);

            Error(retornaToken);


        if((strcmp(retornaToken.Classe, "EOF") == 0)){
            condicao = 0;
            
            printf("\n\nFim da leitura do arquivo\n\n");
            printTabelaDeSimbolos(tabela);
        } 
    }

}

//-------------------------------------------------------------
//-------------------------------------------------------------

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
    // Consome o PT_V (ou EOF) para não ficar preso no mesmo token
    if (strcmp(lookahead->Classe, "PT_V") == 0) {
        *lookahead = Scanner();
    }
    printf("Recuperação modo pânico: análise retomada após o token '%s'\n", lookahead->Lexema);
}


int main() {
    // Inicializações necessárias do léxico
    Arquivo = fopen("FONTE.ALG.txt", "r");
    if (Arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }
    loadBuffer();

    const int TAMANHODATABELA = 103;
    Hash* tabela = criarTabela(TAMANHODATABELA);
    // Insira as palavras reservadas, se necessário

    // ... resto do código do analisador sintático ...

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
        if (strcmp(lookahead.Classe, "EOF") == 0) {
            printf("Fim do arquivo atingido durante recuperação de erro.\n");
            break;
        }
        continue;
    }
    Ação act = Acao[estado][indice];

    if (act.tipoAção == SHIFT) {
        push(act.valor, lookahead);
        lookahead = Scanner();

    } else if (act.tipoAção == REDUCE) {
        int producao = act.valor;
        int producao_quantidade = 1; // Ajuste conforme sua gramática
        pop(producao_quantidade);

        int estado2 = RestornaEstadoTopoPilha();
        int naoTerminal = 0; // Ajuste conforme sua tabela de produções
        int novoEstado = Desvio[estado2][naoTerminal];
        push(novoEstado, lookahead);

        mostrarReducao(producao);

    } else if (act.tipoAção == ACCEPT) {
        printf("Análise sintática concluída com sucesso!\n");
        break;
    } else { 
        tratamentoErroModoPanico(&lookahead);
        if (strcmp(lookahead.Classe, "EOF") == 0) {
            printf("Fim do arquivo atingido durante recuperação de erro.\n");
            break;
        }
    }
    
    }
    return 0;

}