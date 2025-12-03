#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ltpi2.h"

int main() {
    printf("LTPi2 - Calculadora de Pi Sofisticada\n");
    printf("Iniciando cálculo de 100.000 dígitos...\n");
    printf("Isso pode levar alguns minutos dependendo da máquina.\n");

    clock_t inicio = clock();
    
    char *pi = calcular_pi(100);
    
    clock_t fim = clock();
    double tempo_gasto = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("\nCálculo concluído em %.2f segundos.\n", tempo_gasto);
    
    // Salvar em arquivo
    FILE *f = fopen("pi_100k_ltpi2.txt", "w");
    if (f) {
        fprintf(f, "%s", pi);
        fclose(f);
        printf("Resultado salvo em 'pi_100k_ltpi2.txt'.\n");
    } else {
        printf("Erro ao salvar arquivo.\n");
    }
    
    // Mostrar primeiros e últimos dígitos
    printf("Primeiros 50 dígitos: %.50s...\n", pi);
    
    free(pi);
    return 0;
}
