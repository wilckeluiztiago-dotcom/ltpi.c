#include <stdio.h>
#include "grandes_numeros.h"

int main() {
    printf("Teste da biblioteca grandes_numeros\\n");
    
    // Teste 1: Multiplicação Karatsuba
    printf("\\nTeste 1: Multiplicação 123456789 * 987654321\\n");
    GrandeNumero *a = criar_grande_numero(1);
    GrandeNumero *b = criar_grande_numero(1);
    GrandeNumero *result = criar_grande_numero(1);
    
    definir_valor_inteiro(a, 123456789);
    definir_valor_inteiro(b, 987654321);
    
    printf("a = "); imprimir_grande_numero(a);
    printf("b = ");  imprimir_grande_numero(b);
    
    multiplicar_grandes_numeros(a, b, result);
    printf("a * b = "); imprimir_grande_numero(result);
    
    // Teste 2: Divisão
    printf("\\nTeste 2: Divisão 1000000 / 7\\n");
    definir_valor_inteiro(a, 1000000);
    definir_valor_inteiro(b, 7);
    dividir_grande_numero(a, b, result);
    printf("1000000 / 7 = "); imprimir_grande_numero(result);
    
    // Teste 3: Raiz Quadrada
    printf("\\nTeste 3: Raiz quadrada de 10000\\n");
    definir_valor_inteiro(a, 10000);
    raiz_quadrada(a, result);
    printf("sqrt(10000) = "); imprimir_grande_numero(result);
    
    liberar_grande_numero(a);
    liberar_grande_numero(b);
    liberar_grande_numero(result);
    
    printf("\\nTodos os testes concluídos com sucesso!\\n");
    return 0;
}
