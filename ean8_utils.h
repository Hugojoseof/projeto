#ifndef EAN8_UTILS_H
#define EAN8_UTILS_H

#include "imagem_pbm.h"

// Tabelas de código declaradas como extern
extern const char *L_CODE[];
extern const char *R_CODE[];

int validarIdentificadorEAN8(const char *identificador);
void desenharBarrasEAN8(ImagemPBM *imagem, const char *identificador, int espessura);

#endif
