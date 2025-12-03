#include "grandes_numeros.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// --- Gerenciamento de Memória ---

GrandeNumero* criar_grande_numero(int capacidade_inicial) {
    GrandeNumero *numero = (GrandeNumero*)malloc(sizeof(GrandeNumero));
    if (numero == NULL) return NULL;

    if (capacidade_inicial < 1) capacidade_inicial = 1;

    numero->blocos = (uint32_t*)calloc(capacidade_inicial, sizeof(uint32_t));
    if (numero->blocos == NULL) {
        free(numero);
        return NULL;
    }

    numero->quantidade_blocos = 1;
    numero->capacidade = capacidade_inicial;
    numero->sinal = 1;

    return numero;
}

void liberar_grande_numero(GrandeNumero *numero) {
    if (numero != NULL) {
        if (numero->blocos != NULL) {
            free(numero->blocos);
        }
        free(numero);
    }
}

void garantir_capacidade(GrandeNumero *numero, int capacidade_necessaria) {
    if (numero->capacidade < capacidade_necessaria) {
        int nova_capacidade = numero->capacidade * 2;
        if (nova_capacidade < capacidade_necessaria) nova_capacidade = capacidade_necessaria;

        uint32_t *novos_blocos = (uint32_t*)realloc(numero->blocos, nova_capacidade * sizeof(uint32_t));
        if (novos_blocos != NULL) {
            numero->blocos = novos_blocos;
            memset(numero->blocos + numero->capacidade, 0, (nova_capacidade - numero->capacidade) * sizeof(uint32_t));
            numero->capacidade = nova_capacidade;
        }
    }
}

void copiar_grande_numero(GrandeNumero *destino, const GrandeNumero *origem) {
    garantir_capacidade(destino, origem->quantidade_blocos);
    memcpy(destino->blocos, origem->blocos, origem->quantidade_blocos * sizeof(uint32_t));
    if (destino->capacidade > origem->quantidade_blocos) {
        memset(destino->blocos + origem->quantidade_blocos, 0, (destino->capacidade - origem->quantidade_blocos) * sizeof(uint32_t));
    }
    destino->quantidade_blocos = origem->quantidade_blocos;
    destino->sinal = origem->sinal;
}

void definir_valor_inteiro(GrandeNumero *numero, int valor) {
    if (valor < 0) {
        numero->sinal = -1;
        valor = -valor;
    } else {
        numero->sinal = 1;
    }

    memset(numero->blocos, 0, numero->capacidade * sizeof(uint32_t));
    
    numero->quantidade_blocos = 0;
    if (valor == 0) {
        numero->quantidade_blocos = 1;
        return;
    }

    int i = 0;
    while (valor > 0) {
        garantir_capacidade(numero, i + 1);
        numero->blocos[i] = valor % BASE_NUMERICA;
        valor /= BASE_NUMERICA;
        i++;
    }
    numero->quantidade_blocos = i;
}

// --- Funções Auxiliares ---

int eh_zero(const GrandeNumero *numero) {
    return (numero->quantidade_blocos == 1 && numero->blocos[0] == 0);
}

int comparar_magnitude(const GrandeNumero *a, const GrandeNumero *b) {
    if (a->quantidade_blocos > b->quantidade_blocos) return 1;
    if (a->quantidade_blocos < b->quantidade_blocos) return -1;

    for (int i = a->quantidade_blocos - 1; i >= 0; i--) {
        if (a->blocos[i] > b->blocos[i]) return 1;
        if (a->blocos[i] < b->blocos[i]) return -1;
    }
    return 0;
}

int comparar_grandes_numeros(const GrandeNumero *a, const GrandeNumero *b) {
    if (a->sinal != b->sinal) return (a->sinal > b->sinal) ? 1 : -1;
    int mag = comparar_magnitude(a, b);
    return (a->sinal == 1) ? mag : -mag;
}

void remover_zeros_a_esquerda(GrandeNumero *numero) {
    while (numero->quantidade_blocos > 1 && numero->blocos[numero->quantidade_blocos - 1] == 0) {
        numero->quantidade_blocos--;
    }
}

void imprimir_grande_numero(const GrandeNumero *numero) {
    if (numero->sinal == -1 && !eh_zero(numero)) printf("-");
    
    if (numero->quantidade_blocos == 0) {
        printf("0");
        return;
    }

    printf("%u", numero->blocos[numero->quantidade_blocos - 1]);
    for (int i = numero->quantidade_blocos - 2; i >= 0; i--) {
        printf("%09u", numero->blocos[i]);
    }
    printf("\n");
}

// --- Operações de Adição e Subtração ---

void somar_magnitudes(const GrandeNumero *a, const GrandeNumero *b, GrandeNumero *resultado) {
    int max_blocos = (a->quantidade_blocos > b->quantidade_blocos) ? a->quantidade_blocos : b->quantidade_blocos;
    garantir_capacidade(resultado, max_blocos + 1);

    uint64_t carry = 0;
    for (int i = 0; i < max_blocos || carry; i++) {
        uint64_t soma = carry;
        if (i < a->quantidade_blocos) soma += a->blocos[i];
        if (i < b->quantidade_blocos) soma += b->blocos[i];

        resultado->blocos[i] = soma % BASE_NUMERICA;
        carry = soma / BASE_NUMERICA;
        if (i >= resultado->quantidade_blocos) resultado->quantidade_blocos = i + 1;
    }
    resultado->quantidade_blocos = (resultado->quantidade_blocos < max_blocos) ? max_blocos : resultado->quantidade_blocos;
    if (carry) {
        resultado->blocos[resultado->quantidade_blocos] = carry;
        resultado->quantidade_blocos++;
    }
    remover_zeros_a_esquerda(resultado);
}

void subtrair_magnitudes(const GrandeNumero *a, const GrandeNumero *b, GrandeNumero *resultado) {
    garantir_capacidade(resultado, a->quantidade_blocos);
    
    int64_t borrow = 0;
    for (int i = 0; i < a->quantidade_blocos; i++) {
        int64_t diff = a->blocos[i] - borrow;
        if (i < b->quantidade_blocos) diff -= b->blocos[i];

        if (diff < 0) {
            diff += BASE_NUMERICA;
            borrow = 1;
        } else {
            borrow = 0;
        }
        resultado->blocos[i] = diff;
    }
    resultado->quantidade_blocos = a->quantidade_blocos;
    remover_zeros_a_esquerda(resultado);
}

void somar_grandes_numeros(const GrandeNumero *a, const GrandeNumero *b, GrandeNumero *resultado) {
    if (a->sinal == b->sinal) {
        somar_magnitudes(a, b, resultado);
        resultado->sinal = a->sinal;
    } else {
        int cmp = comparar_magnitude(a, b);
        if (cmp >= 0) {
            subtrair_magnitudes(a, b, resultado);
            resultado->sinal = a->sinal;
        } else {
            subtrair_magnitudes(b, a, resultado);
            resultado->sinal = b->sinal;
        }
    }
    if (eh_zero(resultado)) resultado->sinal = 1;
}

void subtrair_grandes_numeros(const GrandeNumero *a, const GrandeNumero *b, GrandeNumero *resultado) {
    GrandeNumero temp_b = *b;
    temp_b.sinal = -b->sinal;
    somar_grandes_numeros(a, &temp_b, resultado);
}

// --- Multiplicação por Inteiro (para casos pequenos) ---

void multiplicar_por_inteiro(const GrandeNumero *a, int b, GrandeNumero *resultado) {
    if (b == 0 || eh_zero(a)) {
        definir_valor_inteiro(resultado, 0);
        return;
    }
    
    int b_abs = (b < 0) ? -b : b;
    garantir_capacidade(resultado, a->quantidade_blocos + 2);

    uint64_t carry = 0;
    for (int i = 0; i < a->quantidade_blocos; i++) {
        uint64_t prod = (uint64_t)a->blocos[i] * b_abs + carry;
        resultado->blocos[i] = prod % BASE_NUMERICA;
        carry = prod / BASE_NUMERICA;
    }
    
    resultado->quantidade_blocos = a->quantidade_blocos;
    while (carry > 0) {
        resultado->blocos[resultado->quantidade_blocos] = carry % BASE_NUMERICA;
        carry /= BASE_NUMERICA;
        resultado->quantidade_blocos++;
    }
    
    resultado->sinal = (a->sinal == ((b < 0) ? -1 : 1)) ? 1 : -1;
}

// --- MULTIPLICAÇÃO DE KARATSUBA ---
/*
 * Algoritmo de Karatsuba para multiplicação rápida de números grandes.
 * 
 * Teoria:
 * Para multiplicar dois números X e Y de n dígitos, podemos dividir cada um ao meio:
 *   X = X1 * B^m + X0
 *   Y = Y1 * B^m + Y0
 * onde m = n/2 e B é a base (no nosso caso, BASE_NUMERICA).
 * 
 * A multiplicação tradicional requer 4 multiplicações:
 *   X * Y = X1*Y1 * B^(2m) + (X1*Y0 + X0*Y1) * B^m + X0*Y0
 * 
 * Karatsuba reduz para 3 multiplicações usando o truque:
 *   Z0 = X0 * Y0
 *   Z2 = X1 * Y1
 *   Z1 = (X0 + X1) * (Y0 + Y1) - Z0 - Z2
 * 
 * Então: X * Y = Z2 * B^(2m) + Z1 * B^m + Z0
 * 
 * Complexidade: O(N^log2(3)) ≈ O(N^1.585) vs O(N^2) tradicional
 */

// Limiar para usar multiplicação tradicional vs Karatsuba
#define KARATSUBA_LIMIAR 32

// Multiplicação tradicional O(N^2) - usada para números pequenos
static void multiplicar_tradicional(const GrandeNumero *a, const GrandeNumero *b, GrandeNumero *resultado) {
    if (eh_zero(a) || eh_zero(b)) {
        definir_valor_inteiro(resultado, 0);
        return;
    }

    GrandeNumero *temp = criar_grande_numero(a->quantidade_blocos + b->quantidade_blocos);
    definir_valor_inteiro(temp, 0);
    garantir_capacidade(temp, a->quantidade_blocos + b->quantidade_blocos);

    for (int i = 0; i < a->quantidade_blocos; i++) {
        uint64_t carry = 0;
        for (int j = 0; j < b->quantidade_blocos || carry; j++) {
            uint64_t cur = temp->blocos[i + j] + 
                           (uint64_t)a->blocos[i] * (j < b->quantidade_blocos ? b->blocos[j] : 0) + 
                           carry;
            temp->blocos[i + j] = cur % BASE_NUMERICA;
            carry = cur / BASE_NUMERICA;
            if (i + j >= temp->quantidade_blocos) temp->quantidade_blocos = i + j + 1;
        }
    }
    
    temp->sinal = (a->sinal == b->sinal) ? 1 : -1;
    remover_zeros_a_esquerda(temp);
    copiar_grande_numero(resultado, temp);
    liberar_grande_numero(temp);
}

// Função auxiliar para criar um sub-número a partir de um range de blocos
static GrandeNumero* criar_subnumero(const GrandeNumero *num, int inicio, int fim) {
    int tamanho = fim - inicio;
    if (tamanho <= 0) {
        GrandeNumero *zero = criar_grande_numero(1);
        definir_valor_inteiro(zero, 0);
        return zero;
    }
    
    GrandeNumero *sub = criar_grande_numero(tamanho);
    for (int i = 0; i < tamanho && (inicio + i) < num->quantidade_blocos; i++) {
        sub->blocos[i] = num->blocos[inicio + i];
    }
    sub->quantidade_blocos = tamanho;
    sub->sinal = 1;
    remover_zeros_a_esquerda(sub);
    return sub;
}

// Função auxiliar para somar com shift de blocos: resultado = a + (b << shift_blocos)
static void somar_com_shift(GrandeNumero *resultado, const GrandeNumero *valor, int shift_blocos) {
    garantir_capacidade(resultado, shift_blocos + valor->quantidade_blocos);
    
    uint64_t carry = 0;
    for (int i = 0; i < valor->quantidade_blocos || carry; i++) {
        uint64_t soma = carry;
        if (shift_blocos + i < resultado->quantidade_blocos) {
            soma += resultado->blocos[shift_blocos + i];
        }
        if (i < valor->quantidade_blocos) {
            soma += valor->blocos[i];
        }
        
        if (shift_blocos + i >= resultado->capacidade) {
            garantir_capacidade(resultado, shift_blocos + i + 1);
        }
        
        resultado->blocos[shift_blocos + i] = soma % BASE_NUMERICA;
        carry = soma / BASE_NUMERICA;
        
        if (shift_blocos + i >= resultado->quantidade_blocos) {
            resultado->quantidade_blocos = shift_blocos + i + 1;
        }
    }
    remover_zeros_a_esquerda(resultado);
}

// Implementação recursiva de Karatsuba
static void karatsuba_recursivo(const GrandeNumero *x, const GrandeNumero *y, GrandeNumero *resultado) {
    // Caso base: números pequenos usam multiplicação tradicional
    if (x->quantidade_blocos <= KARATSUBA_LIMIAR || y->quantidade_blocos <= KARATSUBA_LIMIAR) {
        multiplicar_tradicional(x, y, resultado);
        return;
    }
    
    // Determinar o ponto de divisão (metade do maior número)
    int m = (x->quantidade_blocos > y->quantidade_blocos ? x->quantidade_blocos : y->quantidade_blocos) / 2;
    
    // Dividir X = X1 * B^m + X0
    GrandeNumero *x0 = criar_subnumero(x, 0, m);
    GrandeNumero *x1 = criar_subnumero(x, m, x->quantidade_blocos);
    
    // Dividir Y = Y1 * B^m + Y0
    GrandeNumero *y0 = criar_subnumero(y, 0, m);
    GrandeNumero *y1 = criar_subnumero(y, m, y->quantidade_blocos);
    
    // Calcular as 3 multiplicações de Karatsuba
    GrandeNumero *z0 = criar_grande_numero(x0->quantidade_blocos + y0->quantidade_blocos);
    GrandeNumero *z2 = criar_grande_numero(x1->quantidade_blocos + y1->quantidade_blocos);
    
    karatsuba_recursivo(x0, y0, z0);  // Z0 = X0 * Y0
    karatsuba_recursivo(x1, y1, z2);  // Z2 = X1 * Y1
    
    // Calcular (X0 + X1) e (Y0 + Y1)
    GrandeNumero *x0_mais_x1 = criar_grande_numero(m + 2);
    GrandeNumero *y0_mais_y1 = criar_grande_numero(m + 2);
    somar_grandes_numeros(x0, x1, x0_mais_x1);
    somar_grandes_numeros(y0, y1, y0_mais_y1);
    
    // Z1 = (X0 + X1) * (Y0 + Y1) - Z0 - Z2
    GrandeNumero *z1 = criar_grande_numero(x0_mais_x1->quantidade_blocos + y0_mais_y1->quantidade_blocos);
    karatsuba_recursivo(x0_mais_x1, y0_mais_y1, z1);
    subtrair_grandes_numeros(z1, z0, z1);
    subtrair_grandes_numeros(z1, z2, z1);
    
    // Montar resultado: Z2 * B^(2m) + Z1 * B^m + Z0
    definir_valor_inteiro(resultado, 0);
    garantir_capacidade(resultado, 2 * m + z2->quantidade_blocos);
    
    copiar_grande_numero(resultado, z0);           // resultado = Z0
    somar_com_shift(resultado, z1, m);             // resultado += Z1 * B^m
    somar_com_shift(resultado, z2, 2 * m);         // resultado += Z2 * B^(2m)
    
    // Limpar memória
    liberar_grande_numero(x0);
    liberar_grande_numero(x1);
    liberar_grande_numero(y0);
    liberar_grande_numero(y1);
    liberar_grande_numero(z0);
    liberar_grande_numero(z1);
    liberar_grande_numero(z2);
    liberar_grande_numero(x0_mais_x1);
    liberar_grande_numero(y0_mais_y1);
}

void multiplicar_grandes_numeros(const GrandeNumero *a, const GrandeNumero *b, GrandeNumero *resultado) {
    if (eh_zero(a) || eh_zero(b)) {
        definir_valor_inteiro(resultado, 0);
        return;
    }
    
    // Usar Karatsuba
    karatsuba_recursivo(a, b, resultado);
    resultado->sinal = (a->sinal == b->sinal) ? 1 : -1;
    remover_zeros_a_esquerda(resultado);
}

// --- DIVISÃO OTIMIZADA (baseada no Algoritmo D de Knuth) ---
/*
 * Algoritmo D de Knuth para divisão de números grandes.
 * 
 * Referência: The Art of Computer Programming, Vol. 2, Seção 4.3.1
 * 
 * Estratégia:
 * 1. Normalizar o divisor para que seu bloco mais significativo seja >= BASE/2
 * 2. Para cada posição do quociente, estimar q usando os 2 blocos superiores do dividendo
 *    e o bloco superior do divisor
 * 3. Ajustar q se necessário (raramente precisa de mais de 2 ajustes)
 * 4. Subtrair q * divisor do dividendo parcial
 * 5. Desnormalizar o resultado
 * 
 * Complexidade: O(N*M) onde N é o tamanho do dividendo e M do divisor
 */

void dividir_por_inteiro(const GrandeNumero *dividendo, int divisor, GrandeNumero *quociente) {
    if (divisor == 0) return;
    
    int divisor_abs = (divisor < 0) ? -divisor : divisor;
    garantir_capacidade(quociente, dividendo->quantidade_blocos);
    
    uint64_t resto = 0;
    for (int i = dividendo->quantidade_blocos - 1; i >= 0; i--) {
        uint64_t cur = dividendo->blocos[i] + resto * BASE_NUMERICA;
        quociente->blocos[i] = cur / divisor_abs;
        resto = cur % divisor_abs;
    }
    
    quociente->quantidade_blocos = dividendo->quantidade_blocos;
    quociente->sinal = (dividendo->sinal == ((divisor < 0) ? -1 : 1)) ? 1 : -1;
    remover_zeros_a_esquerda(quociente);
}

void dividir_grande_numero(const GrandeNumero *dividendo, const GrandeNumero *divisor, GrandeNumero *quociente) {
    if (eh_zero(divisor)) {
        printf("ERRO: Divisão por zero!\n");
        return;
    }
    
    int cmp = comparar_magnitude(dividendo, divisor);
    if (cmp < 0) {
        definir_valor_inteiro(quociente, 0);
        return;
    }
    
    if (cmp == 0) {
        definir_valor_inteiro(quociente, 1);
        quociente->sinal = (dividendo->sinal == divisor->sinal) ? 1 : -1;
        return;
    }
    
    // Se o divisor cabe em um único bloco, usar divisão por inteiro (muito mais rápida)
    if (divisor->quantidade_blocos == 1) {
        dividir_por_inteiro(dividendo, divisor->blocos[0], quociente);
        quociente->sinal = (dividendo->sinal == divisor->sinal) ? 1 : -1;
        return;
    }
    
    // Implementação do Algoritmo D de Knuth
    
    int n = divisor->quantidade_blocos;
    int m = dividendo->quantidade_blocos - n;
    
    // Copiar valores para trabalhar (usaremos magnitudes)
    GrandeNumero *u = criar_grande_numero(dividendo->quantidade_blocos + 1);
    copiar_grande_numero(u, dividendo);
    u->sinal = 1;
    
    GrandeNumero *v = criar_grande_numero(divisor->quantidade_blocos);
    copiar_grande_numero(v, divisor);
    v->sinal = 1;
    
    // D1: Normalizar - multiplicar ambos por um fator d tal que v[n-1] >= BASE/2
    uint32_t d = BASE_NUMERICA / (v->blocos[n - 1] + 1);
    if (d > 1) {
        multiplicar_por_inteiro(u, d, u);
        multiplicar_por_inteiro(v, d, v);
    }
    
    // Garantir que u tem espaço para um bloco extra
    garantir_capacidade(u, dividendo->quantidade_blocos + 1);
    if (u->quantidade_blocos <= dividendo->quantidade_blocos) {
        u->blocos[dividendo->quantidade_blocos] = 0;
        u->quantidade_blocos = dividendo->quantidade_blocos + 1;
    }
    
    // Preparar quociente
    garantir_capacidade(quociente, m + 1);
    definir_valor_inteiro(quociente, 0);
    quociente->quantidade_blocos = m + 1;
    
    // D2-D7: Loop principal - calcular cada dígito do quociente
    for (int j = m; j >= 0; j--) {
        // D3: Calcular estimativa de q
        uint64_t dividendo_alto = ((uint64_t)u->blocos[j + n] * BASE_NUMERICA + u->blocos[j + n - 1]);
        uint64_t q_hat = dividendo_alto / v->blocos[n - 1];
        uint64_t r_hat = dividendo_alto % v->blocos[n - 1];
        
        // Refinar q_hat
        while (q_hat >= BASE_NUMERICA || 
               (q_hat * v->blocos[n - 2] > BASE_NUMERICA * r_hat + u->blocos[j + n - 2])) {
            q_hat--;
            r_hat += v->blocos[n - 1];
            if (r_hat >= BASE_NUMERICA) break;
        }
        
        // D4: Multiplicar e subtrair: u[j..j+n] -= q_hat * v
        int64_t carry = 0;
        for (int i = 0; i <= n; i++) {
            uint64_t produto = (i < n) ? q_hat * v->blocos[i] : 0;
            int64_t diff = (int64_t)u->blocos[j + i] - (produto % BASE_NUMERICA) - carry;
            carry = (produto / BASE_NUMERICA);
            
            if (diff < 0) {
                diff += BASE_NUMERICA;
                carry++;
            }
            u->blocos[j + i] = diff;
        }
        
        // D5: Testar resto - se negativo, q_hat está 1 a mais
        if (carry > 0) {
            // D6: Adicionar de volta: u[j..j+n] += v
            q_hat--;
            uint64_t carry_back = 0;
            for (int i = 0; i <= n; i++) {
                uint64_t soma = (uint64_t)u->blocos[j + i] + (i < n ? v->blocos[i] : 0) + carry_back;
                u->blocos[j + i] = soma % BASE_NUMERICA;
                carry_back = soma / BASE_NUMERICA;
            }
        }
        
        quociente->blocos[j] = q_hat;
    }
    
    // D8: Desnormalizar (o resto está em u, mas não precisamos dele para esta função)
    remover_zeros_a_esquerda(quociente);
    quociente->sinal = (dividendo->sinal == divisor->sinal) ? 1 : -1;
    
    liberar_grande_numero(u);
    liberar_grande_numero(v);
}

// --- RAIZ QUADRADA (Newton-Raphson otimizado) ---
/*
 * Método de Newton-Raphson para calcular raiz quadrada.
 * 
 * Fórmula iterativa: x_{n+1} = (x_n + N/x_n) / 2
 * 
 * Convergência: Quadrática (dobra o número de dígitos corretos a cada iteração)
 * 
 * Estimativa inicial: 10^(metade dos dígitos de N)
 * Com boa estimativa inicial, converge em log(precisão) iterações
 */

void raiz_quadrada(const GrandeNumero *numero, GrandeNumero *resultado) {
    if (numero->sinal < 0) {
        printf("ERRO: Raiz quadrada de número negativo!\n");
        return;
    }
    if (eh_zero(numero)) {
        definir_valor_inteiro(resultado, 0);
        return;
    }

    // Estimativa inicial: 10^(ceil(digitos/2))
    // Em termos de blocos: aproximadamente metade dos blocos
    int shift = (numero->quantidade_blocos + 1) / 2;
    
    GrandeNumero *x = criar_grande_numero(shift + 1);
    definir_valor_inteiro(x, 1);
    
    // Criar número com shift blocos: 1 seguido de (shift) blocos de zeros
    if (shift > 0) {
        garantir_capacidade(x, shift + 1);
        for (int i = shift; i > 0; i--) x->blocos[i] = 0;
        x->blocos[shift] = 1;
        x->quantidade_blocos = shift + 1;
    }

    GrandeNumero *x_anterior = criar_grande_numero(numero->capacidade);
    GrandeNumero *div = criar_grande_numero(numero->capacidade);
    
    // Newton-Raphson: x = (x + n/x) / 2
    // Continuaremos até convergência (x não muda mais)
    int iteracoes_max = 150;  // Suficiente para 100k dígitos
    for (int i = 0; i < iteracoes_max; i++) {
        copiar_grande_numero(x_anterior, x);
        
        // div = numero / x
        dividir_grande_numero(numero, x, div);
        
        // x = (x + div) / 2
        somar_grandes_numeros(x, div, x);
        dividir_por_inteiro(x, 2, x);
        
        // Verificar convergência: se x == x_anterior, terminamos
        if (comparar_magnitude(x, x_anterior) == 0) {
            break;
        }
        
        // Verificar oscilação entre dois valores (comum em raiz inteira)
        // Se abs(x - x_anterior) <= 1, estamos perto o suficiente
        GrandeNumero *diferenca = criar_grande_numero(x->quantidade_blocos);
        if (comparar_magnitude(x, x_anterior) > 0) {
            subtrair_magnitudes(x, x_anterior, diferenca);
        } else {
            subtrair_magnitudes(x_anterior, x, diferenca);
        }
        
        int pequena_diff = (diferenca->quantidade_blocos == 1 && diferenca->blocos[0] <= 1);
        liberar_grande_numero(diferenca);
        
        if (pequena_diff) break;
    }
    
    copiar_grande_numero(resultado, x);
    
    liberar_grande_numero(x);
    liberar_grande_numero(x_anterior);
    liberar_grande_numero(div);
}
