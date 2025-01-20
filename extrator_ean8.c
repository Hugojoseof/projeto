// extrator_ean8.c
#include "imagem_pbm.h"
#include "ean8_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LARGURA_TOTAL 67


// Função para remover margens (zeros antes e depois das barras)
void removerMargens(char *codigo) {
    int inicio = 0, fim = strlen(codigo) - 1;
    while (codigo[inicio] == '0') inicio++;
    while (codigo[fim] == '0') fim--;
    memmove(codigo, &codigo[inicio], fim - inicio + 1);
    codigo[fim - inicio + 1] = '\0';
}

// Função para normalizar barras de código
char *normalizarBarras(char *codigo, int escala) {
    int tamanho_normalizado = LARGURA_TOTAL;
    char *barras = malloc((tamanho_normalizado + 1) * sizeof(char));
    if (!barras) {
        fprintf(stderr, "Erro na alocação de memória para normalizar barras.\n");
        return NULL;
    }

    for (int i = 0; i < tamanho_normalizado; i++) {
        int soma = 0;
        for (int j = 0; j < escala; j++) {
            soma += (codigo[i * escala + j] - '0');
        }
        barras[i] = (soma > (escala / 2)) ? '1' : '0';
    }

    barras[tamanho_normalizado] = '\0';
    return barras;
}

// Função para extrair o identificador de um código de barras
int extrairIdentificador(char *barras, char *identificador) {
    // Verificar marcadores iniciais e finais
    if (strncmp(barras, "101", 3) != 0 || strncmp(&barras[LARGURA_TOTAL - 3], "101", 3) != 0) {
        fprintf(stderr, "Marcador inicial ou final inválido.\n");
        return 0;
    }

    int pos = 3; // Após o marcador inicial

    // Decodificar os 4 primeiros dígitos (L_CODE)
    for (int i = 0; i < 4; i++) {
        char segmento[8] = {0};
        strncpy(segmento, &barras[pos], 7);
        pos += 7;
        for (int j = 0; j < 10; j++) {
            if (strcmp(segmento, L_CODE[j]) == 0) {
                identificador[i] = '0' + j;
                break;
            }
        }
    }

    // Pular o marcador central
    pos += 5;

    // Decodificar os 4 últimos dígitos (R_CODE)
    for (int i = 4; i < 8; i++) {
        char segmento[8] = {0};
        strncpy(segmento, &barras[pos], 7);
        pos += 7;
        for (int j = 0; j < 10; j++) {
            if (strcmp(segmento, R_CODE[j]) == 0) {
                identificador[i] = '0' + j;
                break;
            }
        }
    }

    identificador[8] = '\0';
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo PBM>\n", argv[0]);
        return 1;
    }

    const char *nomeArquivo = argv[1];

    // Carregar a imagem PBM
    ImagemPBM *imagem = carregarImagemPBM(nomeArquivo);
    if (!imagem) {
        return 1;
    }

    // Determinar a escala (pixels por área)
    int escala = imagem->largura / LARGURA_TOTAL;
    if (escala < 1) {
        fprintf(stderr, "Imagem muito pequena para conter um código de barras válido.\n");
        liberarImagemPBM(imagem);
        return 1;
    }

    // Converter a linha do meio para uma string
    char *linha = malloc((imagem->largura + 1) * sizeof(char));
    if (!linha) {
        fprintf(stderr, "Erro ao alocar memória para linha da imagem.\n");
        liberarImagemPBM(imagem);
        return 1;
    }

    for (int i = 0; i < imagem->largura; i++) {
        linha[i] = imagem->pixels[imagem->altura / 2][i] ? '1' : '0';
    }
    linha[imagem->largura] = '\0';

    // Remover margens
    removerMargens(linha);

    // Normalizar barras
    char *barras = normalizarBarras(linha, escala);
    free(linha);
    if (!barras) {
        liberarImagemPBM(imagem);
        return 1;
    }

    // Extrair o identificador
    char identificador[9];
    if (!extrairIdentificador(barras, identificador)) {
        fprintf(stderr, "Falha ao extrair o identificador do código de barras.\n");
        free(barras);
        liberarImagemPBM(imagem);
        return 1;
    }

    printf("Identificador extraído: %s\n", identificador);

    free(barras);
    liberarImagemPBM(imagem);
    return 0;
}
