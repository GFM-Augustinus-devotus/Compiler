#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 77      // Ajuste conforme o número de linhas do seu CSV
#define COLS 19       // Ajuste conforme o número de colunas (no seu CSV são 27)
#define MAX_LEN 50    // Tamanho máximo de cada string

// Função para dividir a linha em tokens, preservando campos vazios
void split_csv_preserve_empty(char *linha, char matriz[COLS][MAX_LEN]) {
    int i = 0;
    char *ptr = linha;
    char *start = ptr;

    while (*ptr != '\0' && i < COLS) {
        if (*ptr == ',' || *ptr == '\n') {
            int len = ptr - start;
            if (len > 0) {
                strncpy(matriz[i], start, len);
                matriz[i][len] = '\0';
            } else {
                matriz[i][0] = '\0';  // Campo vazio
            }
            i++;
            start = ptr + 1;
        }
        ptr++;
    }

    // Para o último campo, se a linha não terminar com \n
    if (i < COLS && start != ptr) {
        strncpy(matriz[i], start, ptr - start);
        matriz[i][ptr - start] = '\0';
    }
}

int main() {
    FILE *file = fopen("SLR_Goto.csv", "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    char matriz[ROWS][COLS][MAX_LEN];  // Matriz final de strings
    char linha[1024];
    int row = 0;

    while (fgets(linha, sizeof(linha), file) && row < ROWS) {
        split_csv_preserve_empty(linha, matriz[row]);
        row++;
    }

    fclose(file);

    // Exibir a matriz lida
    printf("Matriz carregada:\n\n");
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < COLS; j++) {
            if (strlen(matriz[i][j]) == 0)
                printf("--\t");
            else
                printf("%s\t", matriz[i][j]);
        }
        printf("\n");
    }

    return 0;
}
