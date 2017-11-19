#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include "string.h"
#define RED 0.3
#define GREEN 0.59
#define BLUE 0.11
#define LARGURA 488
#define ALTURA 512
#define true 1
#define false 0

int matrizPixel[ALTURA][LARGURA];

#pragma pack(push, 1)
typedef struct bitmap {
	unsigned char assinatura1; //B
	unsigned char assinatura2; //M
	unsigned int tamanhoArquivo;
	unsigned short campoReservado1;
	unsigned short campoReservado2;
	unsigned int deslocamento;
	unsigned int tamanhoCabecalho;
	unsigned int larguraImagem;
	unsigned int alturaImagem;
	unsigned short numeroPlanos;
	unsigned short quantidadeBitsPixel;
	unsigned int comprensao;
	unsigned int tamanhoArq;
	unsigned int resolucaoHorizontal;
	unsigned int resolucaoVertical;
	unsigned int numeroCores;
	unsigned int numeroCoresImportantes;
} BITMAP;

typedef struct pixel {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} PIXEL;
#pragma pack(pop)

int verificaBMP(char b, char m) {
	if (b == 'B' && m == 'M') {
		return 1;
	}
	return 0;
}

int procurador(int t, int j, int operacao, int *alturaFim, char *linhaDeAnalise) {
	int pilha[ALTURA * LARGURA * 2];
	int alturaFim2, alturaInicio2;
	int linha, coluna, contadorNo = 0;
	int achouNo = false, contadorPixPilha = 0, contadorPilha = 0;
	int alturaInicio = 0;
	short yCima = 0, yBaixo = 0, xEsquerdo = 0, xDireito = 0, xMedio = 0,
			yMedio = 0;
	char posicaoNo[100] = " ";
	char numm[10];

	do {
		if (matrizPixel[t][j] == 1) {
			matrizPixel[t][j] = 8;
			linha = t;
			coluna = j;
			yCima = t, yBaixo = t, xEsquerdo = j, xDireito = j;
			while (1) {
				if (contadorPilha < 0){
					break;
				}
				if (matrizPixel[t + 1][j] == 1) { //para baixo
					if (operacao == 1 && (t + 1) > alturaInicio) {
						alturaInicio = t + 1;
					}
					matrizPixel[t + 1][j] = 4;
					pilha[contadorPilha] = t + 1;
					pilha[contadorPilha + 1] = j;
					contadorPilha += 2;
					++contadorPixPilha;
					if (yBaixo < t + 1) {
						yBaixo = t + 1;
					}
				}
				if (matrizPixel[t][j - 1] == 1 && (j-1) > 0) { //para esquerda
					matrizPixel[t][j - 1] = 2;
					pilha[contadorPilha] = t;
					pilha[contadorPilha + 1] = j - 1;
					contadorPilha += 2;
					++contadorPixPilha;
					if (xEsquerdo > j - 1) {
						xEsquerdo = j - 1;
					}
				}
				if (matrizPixel[t][j + 1] == 1 && (j + 1) < LARGURA) { //para direita
					matrizPixel[t][j + 1] = 2;
					pilha[contadorPilha] = t;
					pilha[contadorPilha + 1] = j + 1;
					contadorPilha += 2;
					++contadorPixPilha;
					if (xDireito < j + 1) {
						xDireito = j + 1;
					}
				}
				if (matrizPixel[t - 1][j] == 1) { //para cima
					if (operacao == 2 && (t - 1) < *alturaFim) {
						*alturaFim = t - 1;
					}
					matrizPixel[t - 1][j] = 2;
					pilha[contadorPilha] = t - 1;
					pilha[contadorPilha + 1] = j;
					contadorPilha += 2;
					++contadorPixPilha;
					if (yCima > t - 1) {
						yCima = t - 1;
					}
				}
				if (contadorPilha >= 0) {
					if (contadorPixPilha > 35 && achouNo == false && operacao == 3) {
						achouNo = true;
					}
					t = pilha[contadorPilha - 2];
					j = pilha[contadorPilha - 1];
					if (t > LARGURA || j < 0){
						//printf("Erro: Estouro de matriz.");
						break;
					}
					contadorPilha -= 2;
				}
			}//fim while

			if (achouNo == true) {
				contadorNo++;
				//contadorPilha=0;
				achouNo = false;
				yMedio = ((yBaixo + yCima) / 2);
				yMedio = ALTURA - yMedio;
				xMedio = ((xEsquerdo + xDireito) / 2) + 1;
				sprintf(numm, "%i", xMedio);
				strcat(posicaoNo, numm);
				strcat(posicaoNo, " ");
				sprintf(numm, "%i", yMedio);
				strcat(posicaoNo, numm);
				strcat(posicaoNo, " ");
			}
			contadorPixPilha = 0;
			contadorPilha=0;

			if (operacao == 3) {
				t = linha;
				j = coluna;
			}
		}// fim if (matrizPixel[t][j] == 1)

		++j;
		if (j == (LARGURA - 1)) {
			t++;
			j = 0;
		}
	} while (operacao == 3 && t < *alturaFim);

	if (operacao == 1) {
		alturaInicio2 = ALTURA - alturaInicio;
		return alturaInicio2;
	} else if (operacao == 2) {
		alturaFim2 = ALTURA - *alturaFim;
		sprintf(numm, "%i", alturaFim2);
		strcat(linhaDeAnalise, numm);
		strcat(linhaDeAnalise, " ");
		return alturaFim2;
	} else if (operacao == 3) {
		if (contadorNo > 0 && operacao == 3) {
			sprintf(numm, "%i", contadorNo);
			strcat(linhaDeAnalise, numm);
			strcat(linhaDeAnalise, posicaoNo);
		} else if (operacao == 3) {
			sprintf(numm, "%i", contadorNo);
			strcat(linhaDeAnalise, numm);
		}
		return contadorNo;
	} else {
		return 0;
	}
}

void analiseImagem(int posicaoFimMadeira, char *linhaDeAnalise) {
	int aux, inicio=0, auxInicio=ALTURA, fim, alt2;
	char numm[10];

	//procurando o fim da Imagem na matriz
	fim = procurador(posicaoFimMadeira, 0, 2, &posicaoFimMadeira, linhaDeAnalise);

	//procurando o inicio da Imagem
	for(aux=0; aux<LARGURA; aux++){
		alt2= ALTURA;
		if (matrizPixel[0][aux] == 1){
			inicio = procurador(0, aux, 1, &alt2, linhaDeAnalise) - 1;
			if (inicio < auxInicio){
				auxInicio = inicio;
			}
		}
	}
	sprintf(numm, "%i", auxInicio);
	strcat(linhaDeAnalise, numm);
	strcat(linhaDeAnalise, " ");

	//procurando a quantidade de nos
	fim= ALTURA - fim - 1;
	inicio= ALTURA - inicio;
	int qtdNos = procurador(inicio == ALTURA?0:inicio, 0, 3, &fim, linhaDeAnalise);
}

int montadoMatriz(FILE *imagem) {
	BITMAP a1;
	PIXEL pix;

	int alt, larg, deslocamentoPixel = 0, tomCinza = 0;
	short fimMadeira = 0, achouFimMadeira = 0, posicaoFimMadeira;
	short inicioMadeira = 0, achouInicioMadeira = 0;

	fseek(imagem, SEEK_SET, 0);
	fread(&a1, sizeof(BITMAP), 1, imagem);

	fseek(imagem, a1.deslocamento, SEEK_SET);
	fread(&pix, sizeof(PIXEL), 1, imagem);
	for (alt = 0; alt < a1.alturaImagem; alt++) {
		for (larg = 0; larg < a1.larguraImagem; larg++) {
			fseek(imagem,
					(a1.deslocamento + (deslocamentoPixel * sizeof(PIXEL))),
					SEEK_SET);
			fread(&pix, sizeof(PIXEL), 1, imagem);

			if (achouFimMadeira == true) {
				matrizPixel[alt][larg] = 2;
			} else {
				tomCinza = (pix.red * RED) + (pix.green * GREEN)
						+ (pix.blue * BLUE);

				if (tomCinza <= 80) {
					matrizPixel[alt][larg] = 1;
				} else {
					matrizPixel[alt][larg] = 0;
				}
			}

			if (matrizPixel[alt][larg] == 0 && achouInicioMadeira == false) {
				++inicioMadeira;
				if (inicioMadeira == 35) {
					achouInicioMadeira = true;
				}
			}

			if (matrizPixel[alt][larg] == 1 && achouInicioMadeira == true) {
				++fimMadeira;
				if (fimMadeira == 488) {
					posicaoFimMadeira = alt;
					achouFimMadeira = true;
				}
			}
			deslocamentoPixel++;
		}
		if (achouInicioMadeira == false) {
			inicioMadeira = 0;
		}
		if (achouFimMadeira == false) {
			fimMadeira = 0;
		}
	}
	return posicaoFimMadeira;
}

int main(void) {
	setbuf(stdout, NULL);
	DIR *diretorio;
	BITMAP a2;
	FILE *imagem;
	FILE *arqSaida = fopen("Analise.txt", "w+");
	struct dirent *lsdir;

	printf("Digite o arquivo que deseja analisar: ");
	char *nomeArquivo = malloc(50);
	char *auxNomeArquivo = malloc(100);
	scanf("%s", nomeArquivo);
	strcpy(auxNomeArquivo, nomeArquivo);
	fflush(stdin);
	clock_t end, start;
	start = clock();

	diretorio = opendir(nomeArquivo);

	while ((lsdir = readdir(diretorio)) != NULL) {
		strcat(auxNomeArquivo, lsdir->d_name);
		imagem = fopen(auxNomeArquivo, "rb+");
		fseek(imagem, SEEK_SET, 0);
		fread(&a2, sizeof(BITMAP), 1, imagem);

		if (verificaBMP(a2.assinatura1, a2.assinatura2) == 1) {
			char linhaDeAnalise[200] = "";
			strcat(linhaDeAnalise, lsdir->d_name);
			strcat(linhaDeAnalise, " ");
			int posicaoFimMadeira = montadoMatriz(imagem);
			analiseImagem(posicaoFimMadeira, linhaDeAnalise);
			fprintf(arqSaida, "%s\n", linhaDeAnalise);
		}
		strcpy(auxNomeArquivo, nomeArquivo);
		fclose(imagem);
	}

	fclose(arqSaida);
	end = clock();
	printf("Tempo gasto: %4.0f ms\n\n", 1000 * (double) (end - start) / (double) (CLOCKS_PER_SEC));

	//teste
//	FILE *w = fopen("complementoMatriz.txt", "w+");
//	FILE *we= fopen("complementoImagem.txt", "w+");
//	short k, l;
//	for (k = 0; k < 512; k++) {
//		for (l = 0; l < 488; l++) {
//			fprintf(w, "%hd", matrizPixel[k][l]);
//		}
//		fprintf(w, "%c", '\n');
//	}
//	for (k = 511; k >= 0; k--) {
//		for (l = 0; l < 488; l++) {
//			fprintf(we, "%hd", matrizPixel[k][l]);
//		}
//		fprintf(we, "%c", '\n');
//	}
//
//	fclose(w);
//	fclose(we);

	return EXIT_SUCCESS;
}
