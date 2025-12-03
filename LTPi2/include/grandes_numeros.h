#ifndef GRANDES_NUMEROS_H
#define GRANDES_NUMEROS_H

#include <stdint.h>

// Definição da base para os cálculos (10^9 permite caber em uint32_t e facilita impressão)
#define BASE_NUMERICA 1000000000
#define DIGITOS_POR_BLOCO 9

// Estrutura para representar um número grande
typedef struct {
    uint32_t *blocos;       // Array dinâmico de blocos numéricos
    int quantidade_blocos;  // Quantidade de blocos em uso
    int capacidade;         // Capacidade total alocada
    int sinal;              // 1 para positivo, -1 para negativo
} GrandeNumero;

// Funções de gerenciamento de memória
GrandeNumero* criar_grande_numero(int capacidade_inicial);
void liberar_grande_numero(GrandeNumero *numero);
void copiar_grande_numero(GrandeNumero *destino, const GrandeNumero *origem);
void definir_valor_inteiro(GrandeNumero *numero, int valor);
void definir_valor_string(GrandeNumero *numero, const char *valor_str);

// Operações matemáticas básicas
void somar_grandes_numeros(const GrandeNumero *a, const GrandeNumero *b, GrandeNumero *resultado);
void subtrair_grandes_numeros(const GrandeNumero *a, const GrandeNumero *b, GrandeNumero *resultado);
void multiplicar_grandes_numeros(const GrandeNumero *a, const GrandeNumero *b, GrandeNumero *resultado);
void multiplicar_por_inteiro(const GrandeNumero *a, int b, GrandeNumero *resultado);
void dividir_grande_numero(const GrandeNumero *dividendo, const GrandeNumero *divisor, GrandeNumero *quociente);
void dividir_por_inteiro(const GrandeNumero *dividendo, int divisor, GrandeNumero *quociente);
void raiz_quadrada(const GrandeNumero *numero, GrandeNumero *resultado);

// Funções auxiliares e de exibição
void imprimir_grande_numero(const GrandeNumero *numero);
int comparar_grandes_numeros(const GrandeNumero *a, const GrandeNumero *b);
int eh_zero(const GrandeNumero *numero);

#endif // GRANDES_NUMEROS_H
