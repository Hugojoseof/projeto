#ifndef IMAGEM_PBM_H
#define IMAGEM_PBM_H

#define PIXEL_BRANCO 0
#define PIXEL_PRETO 1

typedef struct {
    int largura;
    int altura;
    int **pixels;
} ImagemPBM;

ImagemPBM *criarImagemPBM(int largura, int altura);
void liberarImagemPBM(ImagemPBM *imagem);
int salvarImagemPBM(const char *nomeArquivo, ImagemPBM *imagem);
ImagemPBM *carregarImagemPBM(const char *nomeArquivo); // Declaração correta

#endif
