#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <fenv.h>
#include <math.h>
#include <stdint.h>

#include "utils.h"

#define UNROLL 6

/////////////////////////////////////////////////////////////////////////////////////
//   AJUSTE DE CURVAS
/////////////////////////////////////////////////////////////////////////////////////


/*
 * 1 mudanca: retira a calculo i+j do loop
 * 2 mudanca: funde os dois loops de i ate n-1
 */
void montaSL(double **A, double *b, int n, long long int p, double *x, double *y) {
  int somaIndices;
  double power;
  for (int i = 0; i < n; ++i){
    for (int j = 0; j < n-(n%UNROLL); j += UNROLL) {
      A[i][j] = 0.0; 
      A[i][j+1] = 0.0; 
      A[i][j+2] = 0.0; 
      A[i][j+3] = 0.0; 
      A[i][j+4] = 0.0; 
      A[i][j+5] = 0.0; 
      
      somaIndices = i + j; 
      for (long long int k = 0; k < p; ++k) {
        power = pow(x[k], somaIndices);
        A[i][j] += power;
        power *= x[k];
        A[i][j+1] += power;
        power *= x[k];
        A[i][j+2] += power;
        power *= x[k];
        A[i][j+3] += power;
        power *= x[k];
        A[i][j+4] += power;
        power *= x[k];
        A[i][j+5] += power;
      } 
    }

    for (int j = n-(n%UNROLL); j < n; ++j){
      A[i][j] = 0.0; 
      somaIndices = i + j;  
      for (long long int k = 0; k < p; ++k)
        A[i][j] += pow(x[k], somaIndices);
    }

    b[i] = 0.0;
    for (long long int k = 0; k < p; ++k)
      b[i] += pow(x[k],i) * y[k];
  }
}

void eliminacaoGauss(double **A, double *b, int n) {
  for (int i = 0; i < n; ++i) {
    int iMax = i;
    for (int k = i+1; k < n; ++k)
      if (A[k][i] > A[iMax][i])
        iMax = k;
    if (iMax != i) {
      double *tmp, aux;
      tmp = A[i];
      A[i] = A[iMax];
      A[iMax] = tmp;

      aux = b[i];
      b[i] = b[iMax];
      b[iMax] = aux;
    }

    for (int k = i+1; k < n; ++k) {
      double m = A[k][i] / A[i][i];
      A[k][i]  = 0.0;
      for (int j = i+1; j < n; ++j)
        A[k][j] -= A[i][j]*m;
      b[k] -= b[i]*m;
    }
  }
}

void retrossubs(double **A, double *b, double *x, int n) {
  for (int i = n-1; i >= 0; --i) {
    x[i] = b[i];
    for (int j = i+1; j < n; ++j)
      x[i] -= A[i][j]*x[j];
    x[i] /= A[i][i];
  }
}


/* 
 * 1 mudanca: retira o use de pow() e guarda valor em uma variavel para 
 * progressivo da potencia
 */
double P(double x, int N, double *alpha) {
  double powerx = x, Px = alpha[0];
  for (int i = 1; i <= N; ++i){
    Px += alpha[i]*powerx;
    powerx *= x;
  }  

  return Px;
}

int main() {

  int N, n;
  long long int K, p;

  scanf("%d %lld", &N, &K);
  p = K;   // quantidade de pontos
  n = N+1; // tamanho do SL (grau N + 1)

  double *x = (double *) malloc(sizeof(double)*p);
  double *y = (double *) malloc(sizeof(double)*p);

  // ler numeros
  for (long long int i = 0; i < p; ++i)
    scanf("%lf %lf", x+i, y+i);

  double **A = (double **) malloc(sizeof(double *)*n);
  for (int i = 0; i < n; ++i)
    A[i] = (double *) malloc(sizeof(double)*n);
  
  double *b = (double *) malloc(sizeof(double)*n);
  double *alpha = (double *) malloc(sizeof(double)*n); // coeficientes ajuste

  // (A) Gera SL
  double tSL = timestamp();
  montaSL(A, b, n, p, x, y);
  tSL = timestamp() - tSL;

  // (B) Resolve SL
  double tEG = timestamp();
  eliminacaoGauss(A, b, n); 
  retrossubs(A, b, alpha, n); 
  tEG = timestamp() - tEG;

  // Imprime coeficientes
  for (int i = 0; i < n; ++i)
    printf("%1.15e ", alpha[i]);
  puts("");

  // Imprime resíduos
  for (long long int i = 0; i < p; ++i)
    printf("%1.15e ", fabs(y[i] - P(x[i],N,alpha)) );
  puts("");

  // Imprime os tempos
  printf("%lld %1.10e %1.10e\n", K, tSL, tEG);

  return 0;
}
