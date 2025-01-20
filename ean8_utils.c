#include "ean8_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char *L_CODE[] = {
    "0001101", "0011001", "0010011", "0111101", "0100011",
    "0110001", "0101111", "0111011", "0110111", "0001011"
};

const char *R_CODE[] = {
    "1110010", "1100110", "1101100", "1000010", "1011100",
    "1001110", "1010000", "1000100", "1001000", "1110100"
};

int validarIdentificadorEAN8(const char *identificador) {
    if (strlen(identificador) != 8) return 0;
    for (int i = 0; i < 8; i++) {
        if (!isdigit(identificador[i])) return 0;
    }
    int soma = 0;
    for (int i = 0; i < 7; i++) {
        soma += (i % 2 == 0 ? 3 : 1) * (identificador[i] - '0');
    }
    int digitoVerificador = (10 - (soma % 10)) % 10;
    return digitoVerificador == (identificador[7] - '0');
}

void desenharBarrasEAN8(ImagemPBM *imagem, const char *identificador, int espessura) {
    int pos = 0;
    int altura = imagem->altura;

    // Marcador inicial
    for (int i = 0; i < 3; i++) {
        for (int e = 0; e < espessura; e++) {
            for (int j = 0; j < altura; j++) {
                imagem->pixels[j][pos] = ("101"[i] == '1') ? PIXEL_PRETO : PIXEL_BRANCO;
            }
            pos++;
        }
    }

    // 4 primeiros dígitos (L_CODE)
    for (int i = 0; i < 4; i++) {
        const char *codigo = L_CODE[identificador[i] - '0'];
        for (int k = 0; k < 7; k++) {
            for (int e = 0; e < espessura; e++) {
                for (int j = 0; j < altura; j++) {
                    imagem->pixels[j][pos] = (codigo[k] == '1') ? PIXEL_PRETO : PIXEL_BRANCO;
                }
                pos++;
            }
        }
    }

    // Marcador central
    for (int i = 0; i < 5; i++) {
        for (int e = 0; e < espessura; e++) {
            for (int j = 0; j < altura; j++) {
                imagem->pixels[j][pos] = ("01010"[i] == '1') ? PIXEL_PRETO : PIXEL_BRANCO;
            }
            pos++;
        }
    }

    // 4 últimos dígitos (R_CODE)
    for (int i = 4; i < 8; i++) {
        const char *codigo = R_CODE[identificador[i] - '0'];
        for (int k = 0; k < 7; k++) {
            for (int e = 0; e < espessura; e++) {
                for (int j = 0; j < altura; j++) {
                    imagem->pixels[j][pos] = (codigo[k] == '1') ? PIXEL_PRETO : PIXEL_BRANCO;
                }
                pos++;
            }
        }
    }

    // Marcador final
    for (int i = 0; i < 3; i++) {
        for (int e = 0; e < espessura; e++) {
            for (int j = 0; j < altura; j++) {
                imagem->pixels[j][pos] = ("101"[i] == '1') ? PIXEL_PRETO : PIXEL_BRANCO;
            }
            pos++;
        }
    }
}
