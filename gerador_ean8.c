#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imagem_pbm.h"
#include "ean8_utils.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <identificador EAN-8> [--largura <pixels>] [--altura <pixels>] [--espessura <pixels>] [--arquivo <nome>]\n", argv[0]);
        return 1;
    }

    const char *identificador = argv[1];
    if (!validarIdentificadorEAN8(identificador)) {
        printf("Identificador inválido! Certifique-se de que possui 8 dígitos e um dígito verificador válido.\n");
        return 1;
    }

    // Valores padrão
    int largura = 200, altura = 100, espessura = 2;
    char nomeArquivo[100] = "codigo_barras.pbm";

    // Processar argumentos opcionais
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--largura") == 0 && i + 1 < argc) {
            largura = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--altura") == 0 && i + 1 < argc) {
            altura = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--espessura") == 0 && i + 1 < argc) {
            espessura = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--arquivo") == 0 && i + 1 < argc) {
            strncpy(nomeArquivo, argv[++i], 99);
        }
    }

    // Criar a imagem
    ImagemPBM *imagem = criarImagemPBM(largura, altura);
    if (!imagem) return 1;

    // Gerar o código de barras
    desenharBarrasEAN8(imagem, identificador, espessura);

    // Salvar a imagem
    if (!salvarImagemPBM(nomeArquivo, imagem)) {
        liberarImagemPBM(imagem);
        return 1;
    }

    printf("Imagem gerada com sucesso no arquivo '%s'.\n", nomeArquivo);
    liberarImagemPBM(imagem);
    return 0;
}
