#include "barcode_utils.h"
#include <stdio.h>
#include <stdlib.h>

void usage_extractor() {
    printf("Uso: ./barcode_extractor <arquivo PBM>\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage_extractor();
        return 1;
    }

    char *input_file = argv[1];

    if (!file_exists(input_file)) {
        printf("Arquivo '%s' não encontrado.\n", input_file);
        return 1;
    }

    if (!is_valid_pbm(input_file)) {
        printf("O arquivo '%s' não é um PBM válido.\n", input_file);
        return 1;
    }

    char identifier[9];
    if (!extract_identifier(input_file, identifier)) {
        printf("Não foi possível encontrar um código de barras na imagem.\n");
        return 1;
    }

    printf("Identificador extraído: %s\n", identifier);
    return 0;
}