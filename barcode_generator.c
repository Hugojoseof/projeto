#include "barcode_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage() {
    printf("Uso: ./barcode_generator <identificador> [espaçamento lateral] [pixels por área] [altura] [nome do arquivo]\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage();
        return 1;
    }

    char *identifier = argv[1];
    int lateral_spacing = (argc > 2) ? atoi(argv[2]) : 4;
    int pixels_per_area = (argc > 3) ? atoi(argv[3]) : 3;
    int height = (argc > 4) ? atoi(argv[4]) : 50;
    char *output_file = (argc > 5) ? argv[5] : "barcode.pbm";

    if (!validate_identifier(identifier)) {
        printf("Identificador inválido. Certifique-se de que possui 8 dígitos numéricos e um dígito verificador válido.\n");
        return 1;
    }

    if (file_exists(output_file)) {
        printf("O arquivo '%s' já existe. Deseja sobrescrevê-lo? (s/n): ", output_file);
        char response;
        scanf(" %c", &response);
        if (response != 's' && response != 'S') {
            printf("Operação cancelada.\n");
            return 1;
        }
    }

    if (generate_barcode_image(identifier, lateral_spacing, pixels_per_area, height, output_file)) {
        printf("Código de barras gerado com sucesso em '%s'.\n", output_file);
    } else {
        printf("Falha ao gerar a imagem do código de barras.\n");
        return 1;
    }

    return 0;
}
