#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <fenv.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "utils.h"

#define UNROLL 6

/////////////////////////////////////////////////////////////////////////////////////
//   AJUSTE DE CURVAS
/////////////////////////////////////////////////////////////////////////////////////

/*
 * 1 - reaproveita dados calculados fazendo com q va de O(n^2) para O(2n)
 * 2 - coloca somaIndice em uma variavel para diminuir o calculo
 */
void montaSL(double *A, double *b, int n, long long int p, double *x, double *y) {
  int somaIndices;
  // calcula a primeira linha inteira
  for(int i = 0; i < n; ++i){
    A[i] = 0.0;
    b[i] = 0.0;
    for (long long int k = 0; k < p; ++k) {                                                                                                                             
      double power = pow(x[k],i);
      A[i] += power;
      b[i] += power * y[k];
    }   
  }     

  for (int i = 1; i < n; ++i){
    // copia n-1 menos elementos da linha anterior
    for (int j = 0; j < n-1; ++j) {
      A[i*n+j] = A[(i-1)*n + j+1];
    }   

    // calcula um unico novo elemento
    A[i*n+n-1] = 0.0;
    somaIndices = i+n-1;
    for (long long int k = 0; k < p; ++k)                                                                                                                             
      A[i*n+n-1] += pow(x[k], somaIndices);

    b[i] = 0.0;
    for (long long int k = 0; k < p; ++k)
      b[i] += pow(x[k],i) * y[k];
  }
}


/*
 * 1 mudanca: retira a calculo i+j do loop
 * 2 mudanca: funde os dois loops de i ate n-1
 */
void OLDmontaSL(double *A, double *b, int n, long long int p, double *x, double *y) {
  int somaIndices;
  double power;
  for (int i = 0; i < n; ++i){

    b[i] = 0.0;
    for (long long int k = 0; k < p; ++k)
      b[i] += pow(x[k],i) * y[k];

    for (int j = 0; j < n-(n%UNROLL); j += UNROLL) {
      
      A[i*n+j] = 0.0; 
      A[i*n+j+1] = 0.0; 
      A[i*n+j+2] = 0.0; 
      A[i*n+j+3] = 0.0; 
      A[i*n+j+4] = 0.0; 
      A[i*n+j+5] = 0.0; 
      
      somaIndices = i + j; 
      for (long long int k = 0; k < p; ++k) {
        A[i*n+j] += pow(x[k], somaIndices);
        A[i*n+j+1] += pow(x[k], somaIndices + 1);
        A[i*n+j+2] += pow(x[k], somaIndices + 2);
        A[i*n+j+3] += pow(x[k], somaIndices + 3);
        A[i*n+j+4] += pow(x[k], somaIndices + 4);
        A[i*n+j+5] += pow(x[k], somaIndices + 5);
      } 
    }

    for (int j = n-(n%UNROLL); j < n; ++j){
      A[i*n+j] = 0.0; 
      somaIndices = i + j;  
      for (long long int k = 0; k < p; ++k)
        A[i*n+j] += pow(x[k], somaIndices);
    }
    
  }
}

void eliminacaoGauss(double **A, double *b, int n) {
  for (int i = 0; i < n; ++i) {
    int iMax = i;
    for (int k = i+1; k < n; ++k)
      if (A[k][i] > A[iMax][i]){
        iMax = k;
      }

    if (iMax != i) {
      double *tmp, aux;
      tmp = A[i];
      A[i] = A[iMax];
      A[iMax] = tmp;

      aux = b[i];
      b[i] = b[iMax];
      b[iMax] = aux;
    }
    
    for (int k = i+1; k < n-(n%UNROLL); k+=UNROLL) {
      double m0 = A[k][i] / A[i][i];
      double m1 = A[k+1][i] / A[i][i];
      double m2 = A[k+2][i] / A[i][i];
      double m3 = A[k+3][i] / A[i][i];
      double m4 = A[k+4][i] / A[i][i];
      double m5 = A[k+5][i] / A[i][i];
      A[k][i]  = 0.0;
      A[k+1][i]  = 0.0;
      A[k+2][i]  = 0.0;
      A[k+3][i]  = 0.0;
      A[k+4][i]  = 0.0;
      A[k+5][i]  = 0.0;
      for (int j = i+1; j < n; ++j){
        A[k][j] -= A[i][j]*m0;
        A[k+1][j] -= A[i][j]*m1;
        A[k+2][j] -= A[i][j]*m2;
        A[k+3][j] -= A[i][j]*m3;
        A[k+4][j] -= A[i][j]*m4;
        A[k+5][j] -= A[i][j]*m5;
      }
      b[k] -= b[i]*m0;
      b[k+1] -= b[i]*m1;
      b[k+2] -= b[i]*m2;
      b[k+3] -= b[i]*m3;
      b[k+4] -= b[i]*m4;
      b[k+5] -= b[i]*m5;
    }
    int max = n-(n%UNROLL) < i+1 ? i+1 : n-(n%UNROLL);
    for (int k = max; k < n; ++k) {
      double m = A[k][i] / A[i][i];
      A[k][i]  = 0.0;
      for (int j = i+1; j < n; ++j){
        A[k][j] -= A[i][j]*m;
      }
      b[k] -= b[i]*m;
    }

  }
}

void retrossubs(double **A, double * b, double * x, int n) {
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

  double *A = (double *) malloc(sizeof(double)*n*n);
  double **B = (double **) malloc(sizeof(double*)*n);

  
  double *b = (double *) malloc(sizeof(double)*n);
  double *alpha = (double *) malloc(sizeof(double)*n); // coeficientes ajuste

  // (A) Gera SL
  double tSL = timestamp();
  montaSL(A, b, n, p, x, y);
  tSL = timestamp() - tSL;

  for (int i = 0; i < n; i++) 
      B[i] = &A[i*n];
  
  // (B) Resolve SL
  double tEG = timestamp();
  eliminacaoGauss(B, b, n); 
  retrossubs(B, b, alpha, n); 
  tEG = timestamp() - tEG;

  for (int i = 0; i < n*n; i++){
    printf("%1.15e\n ", A[i]);
  }
  printf("matrix ^\n");

  // Imprime coeficientes
  for (int i = 0; i < n; ++i)
    printf("%1.15e ", alpha[i]);
  puts("");

  // Imprime resíduos
  for (long long int i = 0; i < p; ++i)
    printf("%1.15e ", fabs(y[i] - P(x[i],N,alpha)) );
  puts("");

  // Imprime os tempos
  printf("%d %lld %1.10e %1.10e\n",n, K, tSL, tEG);

  return 0;
}
