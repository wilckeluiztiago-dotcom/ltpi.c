#include "ltpi2.h"
#include "grandes_numeros.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Constantes de Chudnovsky
#define C 640320
#define C3_OVER_24 10939058860032000L

typedef struct {
    GrandeNumero *P;
    GrandeNumero *Q;
    GrandeNumero *T;
} TermosBS;

void liberar_termos(TermosBS *t) {
    liberar_grande_numero(t->P);
    liberar_grande_numero(t->Q);
    liberar_grande_numero(t->T);
}

// Binary Splitting
void binary_splitting(int a, int b, TermosBS *res) {
    if (b - a == 1) {
        // Caso base: k = a
        // P(a, a+1) = -(6a - 5)(2a - 1)(6a - 1)
        // Q(a, a+1) = 10939058860032000 * a^3
        // T(a, a+1) = P(a, a+1) * (13591409 + 545140134 * a)
        
        res->P = criar_grande_numero(1);
        res->Q = criar_grande_numero(1);
        res->T = criar_grande_numero(1);

        if (a == 0) {
            definir_valor_inteiro(res->P, 1);
            definir_valor_inteiro(res->Q, 1);
            
            // T = 1 * (13591409)
            definir_valor_inteiro(res->T, 13591409);
        } else {
            // P = -(6a-5)(2a-1)(6a-1)
            int64_t val_p = -((int64_t)(6*a - 5) * (2*a - 1) * (6*a - 1));
            // Como val_p é negativo, vamos usar definir_valor_inteiro com abs e setar sinal
            definir_valor_inteiro(res->P, (val_p < 0 ? -val_p : val_p));
            res->P->sinal = (val_p < 0 ? -1 : 1);

            // Q = C3_OVER_24 * a^3
            // Cuidado com overflow de int64 se a for grande (7000^3 * 10^16 excede 2^63)
            // 7000^3 ~ 3.4e11. 3.4e11 * 1e16 = 3.4e27. Excede muito int64.
            // Precisamos usar GrandeNumero para calcular Q.
            
            GrandeNumero *temp = criar_grande_numero(1);
            definir_valor_inteiro(res->Q, a);
            multiplicar_por_inteiro(res->Q, a, res->Q); // a^2
            multiplicar_por_inteiro(res->Q, a, res->Q); // a^3
            
            // Multiplicar pela constante enorme C3_OVER_24
            // Como C3_OVER_24 cabe em int64 mas não em int32, precisamos de cuidado.
            // Minha função multiplicar_por_inteiro aceita int (32 bits geralmente).
            // Vou precisar criar um GrandeNumero para a constante.
            // 10939058860032000L
            // Quebrando em partes: 10939058 * 10^9 + 860032000
            
            definir_valor_inteiro(temp, 10939058);
            multiplicar_por_inteiro(temp, 1000000000, temp);
            GrandeNumero *parte_baixa = criar_grande_numero(1);
            definir_valor_inteiro(parte_baixa, 860032000);
            somar_grandes_numeros(temp, parte_baixa, temp);
            
            multiplicar_grandes_numeros(res->Q, temp, res->Q);
            
            // T = P * (13591409 + 545140134 * a)
            definir_valor_inteiro(parte_baixa, 545140134);
            multiplicar_por_inteiro(parte_baixa, a, parte_baixa);
            GrandeNumero *const_t = criar_grande_numero(1);
            definir_valor_inteiro(const_t, 13591409);
            somar_grandes_numeros(parte_baixa, const_t, parte_baixa);
            
            multiplicar_grandes_numeros(res->P, parte_baixa, res->T);
            
            liberar_grande_numero(temp);
            liberar_grande_numero(parte_baixa);
            liberar_grande_numero(const_t);
        }
        return;
    }

    int m = (a + b) / 2;
    TermosBS left, right;
    
    binary_splitting(a, m, &left);
    binary_splitting(m, b, &right);

    // Merge
    // P = P_left * P_right
    res->P = criar_grande_numero(1);
    multiplicar_grandes_numeros(left.P, right.P, res->P);

    // Q = Q_left * Q_right
    res->Q = criar_grande_numero(1);
    multiplicar_grandes_numeros(left.Q, right.Q, res->Q);

    // T = T_left * Q_right + P_left * T_right
    GrandeNumero *t1 = criar_grande_numero(1);
    multiplicar_grandes_numeros(left.T, right.Q, t1);
    
    GrandeNumero *t2 = criar_grande_numero(1);
    multiplicar_grandes_numeros(left.P, right.T, t2);
    
    res->T = criar_grande_numero(1);
    somar_grandes_numeros(t1, t2, res->T);

    liberar_grande_numero(t1);
    liberar_grande_numero(t2);
    liberar_termos(&left);
    liberar_termos(&right);
}

char* calcular_pi(int digitos) {
    // Chudnovsky: Pi = (Q * 426880 * sqrt(10005)) / T
    // Precisamos de precisão extra para evitar erro de arredondamento
    int termos = digitos / 14 + 1;
    
    TermosBS final;
    binary_splitting(0, termos, &final);

    // Numerador = Q * 426880 * sqrt(10005)
    // Mas precisamos escalar para ter os dígitos inteiros.
    // Pi * 10^k = ...
    // Vamos calcular (Q * 426880 * sqrt(10005 * 10^(2k))) / T
    
    // 1. Calcular sqrt(10005 * 10^(2*digitos))
    GrandeNumero *raiz = criar_grande_numero(1);
    GrandeNumero *base_raiz = criar_grande_numero(1);
    definir_valor_inteiro(base_raiz, 10005);
    
    // Escalar base_raiz por 10^(2*digitos)
    // Isso é muito lento multiplicando por 10 repetidamente.
    // Melhor: criar número com blocos certos.
    // 10^9 por bloco.
    // 2*digitos / 9 blocos.
    // int blocos_extra = (2 * digitos) / 9 + 1;
    // Maneira rápida: criar array de blocos e setar.
    // Mas minha API é limitada.
    // Vou usar multiplicação por potências de 10 é lento?
    // Não, vou hackear a estrutura ou fazer loop eficiente.
    // Loop de multiplicação por 10^9 (BASE_NUMERICA) é apenas shift de blocos.
    // Vou adicionar uma função 'shift_blocos' no grandes_numeros.c se pudesse, mas não posso editar agora facilmente sem reescrever.
    // Vou usar o fato de que multiplicar por BASE_NUMERICA é adicionar um bloco zero.
    // Mas não tenho essa função exposta.
    // Vou usar 'multiplicar_por_inteiro' com BASE_NUMERICA repetidamente?
    // Não, isso é O(N^2) no total.
    // Melhor: criar um GrandeNumero que representa 10^(2*digitos) e multiplicar.
    // Criar 10^(2*digitos) é fácil: 1 seguido de zeros.
    // Mas como setar isso eficientemente?
    // Vou fazer um loop de 'multiplicar_por_inteiro(..., BASE_NUMERICA, ...)'?
    // Sim, é o que tenho.
    
    // Espera, 100k digitos. 200k zeros. ~22k blocos.
    // Multiplicar por BASE_NUMERICA 22k vezes é muito lento se for O(N).
    // Total O(N^2).
    // Preciso de um jeito mais rápido de shiftar.
    // Vou assumir que posso acessar a struct (está no header).
    // Sim, a struct está no header! Posso manipular diretamente.
    
    // Escalar base_raiz por 10^(2*digitos)
    // Fazemos isso multiplicando por BASE_NUMERICA (10^9) repetidamente
    // e depois multiplicando pelo resto da potência de 10
    
    int num_blocos_shift = (2 * digitos) / 9;
    int zeros_restantes = (2 * digitos) % 9;
    
    // Multiplicar por 10^(zeros_restantes) primeiro (número pequeno)
    if (zeros_restantes > 0) {
        int pot10 = 1;
        for(int k = 0; k < zeros_restantes; k++) pot10 *= 10;
        multiplicar_por_inteiro(base_raiz, pot10, base_raiz);
    }
    
    // Agora multiplicar por BASE_NUMERICA num_blocos_shift vezes
    // Isso é equivalente a shiftar blocos, mas usa a API pública
    for (int i = 0; i < num_blocos_shift; i++) {
        multiplicar_por_inteiro(base_raiz, BASE_NUMERICA, base_raiz);
    }
    
    raiz_quadrada(base_raiz, raiz);
    
    // Numerador = Q * 426880 * raiz
    GrandeNumero *numerador = criar_grande_numero(1);
    multiplicar_por_inteiro(final.Q, 426880, numerador);
    multiplicar_grandes_numeros(numerador, raiz, numerador);
    
    // Divisão final: Pi = Numerador / T
    GrandeNumero *pi = criar_grande_numero(1);
    dividir_grande_numero(numerador, final.T, pi);
    
    // Converter para string
    // Cada bloco tem no máximo 9 dígitos
    // Precisamos alocar espaço suficiente para todos os blocos + ponto decimal + null terminator
    int tamanho_buffer = pi->quantidade_blocos * 9 + 10;  // 10 extra para segurança e ponto decimal
    char *buffer = (char*)malloc(tamanho_buffer);
    if (buffer == NULL) {
        printf("ERRO: Falha ao alocar memória para o resultado!\n");
        return NULL;
    }
    char *ptr = buffer;
    
    // O primeiro bloco (mais significativo) pode ter menos de 9 dígitos
    // Mas Pi é 3.14...
    // O resultado 'pi' é um inteiro gigante que representa 314159...
    // O primeiro dígito deve ser 3.
    
    if (pi->quantidade_blocos == 0) {
        sprintf(buffer, "0");
    } else {
        ptr += sprintf(ptr, "%u", pi->blocos[pi->quantidade_blocos - 1]);
        for (int i = pi->quantidade_blocos - 2; i >= 0; i--) {
            ptr += sprintf(ptr, "%09u", pi->blocos[i]);
        }
    }
    
    // Inserir o ponto decimal após o primeiro dígito '3'
    // O buffer tem "314159..."
    // Queremos "3.14159..."
    // Vamos mover tudo um para a direita
    int len = strlen(buffer);
    if (len > 1) {
        for(int i = len; i >= 1; i--) {
            buffer[i+1] = buffer[i];
        }
        buffer[1] = '.';
    }
    
    // Truncar para o número de dígitos pedido
    if ((int)strlen(buffer) > digitos + 2) { // +2 conta '3.'
        buffer[digitos + 2] = '\0';
    }

    liberar_grande_numero(raiz);
    liberar_grande_numero(base_raiz);
    liberar_grande_numero(numerador);
    liberar_grande_numero(pi);
    liberar_termos(&final);
    
    return buffer;
}
