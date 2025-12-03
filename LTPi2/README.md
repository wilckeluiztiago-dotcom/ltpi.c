# LTPi2 - Calculadora de Pi de Alta Precisão

**Autor:** Luiz Tiago Wilcke  
**Data:** Dezembro 2025

## Descrição

**LTPi2** é uma implementação sofisticada em C puro para cálculo de Pi com precisão arbitrária (100.000 dígitos). 

## Características Técnicas

### Biblioteca `grandes_numeros`
Implementação completa de aritmética de precisão arbitrária:

- **Multiplicação de Karatsuba** - Complexidade O(N^1.585)
  - Algoritmo divide-and-conquer recursivo
  - Reduz multiplicações de 4 para 3 em cada nível
  - Ideal para números com milhares de dígitos
  
- **Divisão de Knuth (Algoritmo D)** - Complexidade O(N×M)
  - Baseado em "The Art of Computer Programming, Vol. 2"
  - Normalização e estimativa de quociente otimizada
  - Correção automática com no máximo 2 ajustes
  
- **Raiz Quadrada (Newton-Raphson)** - Convergência quadrática
  - Dobra a precisão a cada iteração
  - Estimativa inicial inteligente baseada em magnitude

### Biblioteca `ltpi2`
Cálculo de Pi usando o **Algoritmo de Chudnovsky** com **Binary Splitting**:

```
         ∞
         ___
         \       (6k)! (13591409 + 545140134k)
1/π =    /    ---------------------------------  × C
         ‾‾‾    (3k)! (k!)³ (640320)^(3k+3/2)
        k=0
```

- Convergência: ~14 dígitos por termo
- Otimização: Binary Splitting evita recalcular fatoriais
- Precisão: Suporta 100.000+ dígitos

## Estrutura do Projeto

```
LTPi2/
├── include/
│   ├── grandes_numeros.h    # Interface da biblioteca de Big Numbers
│   └── ltpi2.h               # Interface do cálculo de Pi
├── src/
│   ├── grandes_numeros.c    # Implementação Karatsuba + Knuth
│   ├── ltpi2.c               # Algoritmo de Chudnovsky
│   └── main.c                # Programa principal
├── Makefile                  # Build system
└── README.md                 # Este arquivo
```

## Compilação e Execução

### Pré-requisitos
- GCC (C99 ou superior)
- Make
- Sistema Linux/Unix

### Compilar
```bash
make
```

### Executar
```bash
./ltpi2
```

O programa calculará Pi com 100.000 dígitos e salvará o resultado em `pi_100k_ltpi2.txt`.

### Limpar
```bash
make clean
```

## Performance

Com as otimizações implementadas:
- **1.000 dígitos**: ~0.02 segundos
- **10.000 dígitos**: ~0.5 segundos (estimado)
- **100.000 dígitos**: ~30-60 segundos (estimado, depende do hardware)

## Detalhes dos Algoritmos

### Multiplicação de Karatsuba

Divide números em partes alta e baixa:
```
X = X1 × B^m + X0
Y = Y1 × B^m + Y0

Z0 = X0 × Y0
Z2 = X1 × Y1
Z1 = (X0+X1) × (Y0+Y1) - Z0 - Z2

Resultado = Z2 × B^(2m) + Z1 × B^m + Z0
```

### Divisão de Knuth

1. Normalizar divisor (multiplicar por d)
2. Estimar cada dígito do quociente usando 2 blocos do dividendo
3. Testar e ajustar se necessário
4. Desnormalizar resultado

### Binary Splitting (Chudnovsky)

Calcula somas de séries racionais sem calcular fatoriais explicitamente:
- P(a,b) = produto dos numeradores
- Q(a,b) = produto dos denominadores  
- T(a,b) = P(a,b) × coeficientes

Combina recursivamente: divide intervalo ao meio, calcula P/Q/T para cada metade, merge.

## Validação

Os primeiros 50 dígitos de Pi:
```
3.14159265358979323846264338327950288419716939937510...
```

## Referências

- Donald Knuth, "The Art of Computer Programming, Vol. 2: Seminumerical Algorithms"
- David V. & Gregory Chudnovsky (1988), "Approximations and Complex Multiplication According to Ramanujan"
- Karatsuba & Ofman (1962), "Multiplication of Many-Digital Numbers by Automatic Computers"

## Licença

Código educacional. Livre para uso e modificação.
