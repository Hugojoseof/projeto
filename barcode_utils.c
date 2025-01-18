#include "barcode_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

bool validate_identifier(const char *identifier) {
    if (strlen(identifier) != 8) return false;
    for (int i = 0; i < 7; i++) {
        if (!isdigit(identifier[i])) return false;
    }
    int sum = 0;
    for (int i = 0; i < 7; i++) {
        sum += (i % 2 == 0 ? 3 : 1) * (identifier[i] - '0');
    }
    int check_digit = (10 - (sum % 10)) % 10;
    return check_digit == (identifier[7] - '0');
}

bool file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

bool generate_barcode_image(const char *identifier, int lateral_spacing, int pixels_per_area, int height, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Erro ao criar o arquivo %s.\n", filename);
        return false;
    }

    // Largura do código de barras
    int largura_codigo = (3 + 28 + 5 + 28 + 3) * pixels_per_area;
    int largura_total = largura_codigo + 2 * lateral_spacing;
    int altura_total = height;

    // Escrever o cabeçalho do PBM
    fprintf(file, "P1\n%d %d\n", largura_total, altura_total);

    // Codificação do identificador em barras
    const char *marcador_inicio = "101";
    const char *marcador_central = "01010";
    const char *marcador_fim = "101";

    const char *L_CODE[] = {"0001101", "0011001", "0010011", "0111101", "0100011", "0110001", "0101111", "0111011", "0110111", "0001011"};
    const char *R_CODE[] = {"1110010", "1100110", "1101100", "1000010", "1011100", "1001110", "1010000", "1000100", "1001000", "1110100"};

    // Construir a linha de dados
    char linha_barras[largura_codigo + 1];
    memset(linha_barras, '0', largura_codigo);
    linha_barras[largura_codigo] = '\0';

    // Adicionar o marcador inicial
    int pos = 0;
    strncpy(linha_barras + pos, marcador_inicio, 3);
    pos += 3;

    // Codificar os 4 primeiros dígitos (L_CODE)
    for (int i = 0; i < 4; i++) {
        strncpy(linha_barras + pos, L_CODE[identifier[i] - '0'], 7);
        pos += 7;
    }

    // Adicionar o marcador central
    strncpy(linha_barras + pos, marcador_central, 5);
    pos += 5;

    // Codificar os 4 últimos dígitos (R_CODE)
    for (int i = 4; i < 8; i++) {
        strncpy(linha_barras + pos, R_CODE[identifier[i] - '0'], 7);
        pos += 7;
    }

    // Adicionar o marcador final
    strncpy(linha_barras + pos, marcador_fim, 3);

    // Depuração: Verificar linha gerada
    printf("Linha barras gerada: %.20s...\n", linha_barras);

    // Escrever o código de barras no arquivo PBM
    for (int y = 0; y < altura_total; y++) {
        // Espaçamento lateral
        for (int x = 0; x < lateral_spacing; x++) {
            fprintf(file, "0 ");
        }

        // Escrever a linha de barras
        for (int x = 0; x < largura_codigo; x++) {
            char pixel = linha_barras[x]; // '1' ou '0'
            for (int p = 0; p < pixels_per_area; p++) {
                fprintf(file, "%c ", pixel); // Gravar pixel repetido
            }
        }

        // Espaçamento lateral
        for (int x = 0; x < lateral_spacing; x++) {
            fprintf(file, "0 ");
        }

        fprintf(file, "\n");
    }

    fclose(file);
    printf("Código de barras gerado com sucesso.\n");
    return true;
}


bool is_valid_pbm(const char *filename) {
    // Verificação simplificada do formato PBM
    FILE *file = fopen(filename, "r");
    if (!file) return false;
    char header[3];
    fscanf(file, "%2s", header);
    fclose(file);
    return strcmp(header, "P1") == 0;
}

bool extract_identifier(const char *filename, char *identifier) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo '%s'.\n", filename);
        return false;
    }

    // Validar cabeçalho PBM
    char header[3];
    int largura, altura;
    if (fscanf(file, "%2s", header) != 1 || strcmp(header, "P1") != 0) {
        fclose(file);
        printf("Arquivo '%s' não é um PBM válido.\n", filename);
        return false;
    }

    if (fscanf(file, "%d %d", &largura, &altura) != 2) {
        fclose(file);
        printf("Dimensões inválidas no arquivo PBM.\n");
        return false;
    }

    printf("Cabeçalho validado: largura=%d, altura=%d\n", largura, altura);

    // Ler a matriz
    int matriz[altura][largura];
    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            if (fscanf(file, "%d", &matriz[y][x]) != 1) {
                printf("Erro ao ler o pixel em (%d, %d).\n", y, x);
                fclose(file);
                return false;
            }
        }
    }
    fclose(file);

    // Compactar barras para decodificação
    int pixels_per_area = largura / (3 + 28 + 5 + 28 + 3); // Estimar a escala
    int barras[95] = {0}; // Total de áreas: 3 (início) + 28 + 5 + 28 + 3 (fim)

    for (int i = 0; i < 95; i++) {
        int start = i * pixels_per_area;
        int sum = 0;
        for (int j = 0; j < pixels_per_area; j++) {
            sum += matriz[0][start + j]; // Média dos pixels
        }
        barras[i] = (sum >= pixels_per_area / 2) ? 1 : 0; // Converter para 0 ou 1
    }

    // Verificar marcador inicial
    if (barras[0] != 1 || barras[1] != 0 || barras[2] != 1) {
        printf("Marcador inicial inválido.\n");
        return false;
    }

    // Decodificar dígitos
    const char *L_CODE[] = {"0001101", "0011001", "0010011", "0111101", "0100011", "0110001", "0101111", "0111011", "0110111", "0001011"};
    const char *R_CODE[] = {"1110010", "1100110", "1101100", "1000010", "1011100", "1001110", "1010000", "1000100", "1001000", "1110100"};

    int pos = 3; // Após o marcador inicial
    for (int i = 0; i < 4; i++) {
        char barra[8] = {0};
        for (int j = 0; j < 7; j++) {
            barra[j] = barras[pos++] + '0';
        }
        for (int k = 0; k < 10; k++) {
            if (strcmp(barra, L_CODE[k]) == 0) {
                identifier[i] = '0' + k;
                break;
            }
        }
    }

    pos += 5; // Pular o marcador central

    for (int i = 4; i < 8; i++) {
        char barra[8] = {0};
        for (int j = 0; j < 7; j++) {
            barra[j] = barras[pos++] + '0';
        }
        for (int k = 0; k < 10; k++) {
            if (strcmp(barra, R_CODE[k]) == 0) {
                identifier[i] = '0' + k;
                break;
            }
        }
    }

    identifier[8] = '\0';
    printf("Identificador decodificado: %s\n", identifier);
    return true;
}
