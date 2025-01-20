#include "imagem_pbm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função para criar uma nova imagem PBM
ImagemPBM *criarImagemPBM(int largura, int altura) {
    ImagemPBM *imagem = (ImagemPBM *)malloc(sizeof(ImagemPBM));
    if (!imagem) {
        fprintf(stderr, "Erro ao alocar memória para a imagem.\n");
        return NULL;
    }

    imagem->largura = largura;
    imagem->altura = altura;

    imagem->pixels = (int **)malloc(altura * sizeof(int *));
    if (!imagem->pixels) {
        free(imagem);
        fprintf(stderr, "Erro ao alocar memória para os pixels.\n");
        return NULL;
    }

    for (int i = 0; i < altura; i++) {
        imagem->pixels[i] = (int *)calloc(largura, sizeof(int));
        if (!imagem->pixels[i]) {
            for (int j = 0; j < i; j++) {
                free(imagem->pixels[j]);
            }
            free(imagem->pixels);
            free(imagem);
            fprintf(stderr, "Erro ao alocar memória para a linha de pixels.\n");
            return NULL;
        }
    }

    return imagem;
}

// Função para liberar a memória da imagem PBM
void liberarImagemPBM(ImagemPBM *imagem) {
    if (!imagem) return;

    for (int i = 0; i < imagem->altura; i++) {
        free(imagem->pixels[i]);
    }
    free(imagem->pixels);
    free(imagem);
}

// Função para salvar uma imagem PBM em um arquivo
int salvarImagemPBM(const char *nomeArquivo, ImagemPBM *imagem) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo %s para escrita.\n", nomeArquivo);
        return 0;
    }

    fprintf(arquivo, "P1\n%d %d\n", imagem->largura, imagem->altura);
    for (int i = 0; i < imagem->altura; i++) {
        for (int j = 0; j < imagem->largura; j++) {
            fprintf(arquivo, "%d ", imagem->pixels[i][j]);
        }
        fprintf(arquivo, "\n");
    }

    fclose(arquivo);
    return 1;
}

// Função para carregar uma imagem PBM de um arquivo
ImagemPBM *carregarImagemPBM(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo %s para leitura.\n", nomeArquivo);
        return NULL;
    }

    char formato[3];
    if (!fgets(formato, sizeof(formato), arquivo)) {
        fprintf(stderr, "Erro ao ler o cabeçalho do arquivo.\n");
        fclose(arquivo);
        return NULL;
    }

    // Verificar se o formato começa com "P1"
    if (strncmp(formato, "P1", 2) != 0) {
        fprintf(stderr, "Arquivo não está no formato PBM.\n");
        fclose(arquivo);
        return NULL;
    }

    int largura, altura;
    if (fscanf(arquivo, "%d %d", &largura, &altura) != 2) {
        fprintf(stderr, "Dimensões da imagem inválidas.\n");
        fclose(arquivo);
        return NULL;
    }

    ImagemPBM *imagem = criarImagemPBM(largura, altura);
    if (!imagem) {
        fclose(arquivo);
        return NULL;
    }

    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            int pixel;
            if (fscanf(arquivo, "%d", &pixel) != 1) {
                fprintf(stderr, "Erro ao ler os pixels da imagem.\n");
                liberarImagemPBM(imagem);
                fclose(arquivo);
                return NULL;
            }
            imagem->pixels[i][j] = pixel;
        }
    }

    fclose(arquivo);
    return imagem;
}
