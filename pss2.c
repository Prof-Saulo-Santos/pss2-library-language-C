/**
 * PSS Library for C - Refactored (pss2)
 *
 * What has changed in pss2.c:
 * - Removal of do...while loops: All iterative functions were changed to a linear flow (try reading -> parse -> return).
 * - Empty string handling: input_s now simply returns the content captured by fgets (even if the user just presses "Enter"), without blocking the terminal.
 * - Character handling (input_c): If the user injects an EOF (like in a pipe or end of file), it now immediately returns \0, instead of breaking the terminal and locking the container's CPU.
 * - Numeric Reading (input_d, input_f, etc.): Conversion is done in a Single-Shot attempt. If the input is garbage (errno == ERANGE or a complete parse failure where endptr == aux), the function just returns the neutral value (0), allowing the application to proceed and handle the failure (even with blind returns, since we haven't solved Point 3 yet).
 * - Buffer Externalization (Step 2): Functions no longer allocate fixed buffers ("magic numbers" like `char aux[5120]`) on the stack. They now receive the buffer injected by the Caller (`buffer` and `buffer_size`), mitigating Stack Overflows.
 * - They use the dependency injection pattern, requiring the caller to provide the local buffer and buffer_size.
 * - Error Status Codes (Step 3): Functions now return a PSS_STATUS indicating the operation result (OK, EOF, EMPTY, PARSE, RANGE). The requested value is now injected via pointers (`*saida` / `*output`). This ensures the application doesn't confuse a conversion error with a legitimate "0" input from the user.
 * - This new version of the library (pss2) now adheres to the Single Responsibility Principle (SRP): it only handles capturing and converting the string to the correct type. The responsibility of what to do upon failure (try again, abort, log) has been returned to the caller!
 */

 /**
 * PSS Library for C - Refactored (pss2)
 *
 * O que foi alterado no pss2.c:
 * - Remoção das estruturas do...while: Todas as funções iterativas foram alteradas para um fluxo linear (tentar ler -> fazer o parse -> retornar).
 * - Tratamento de strings vazias: Agora o input_s apenas retorna o conteúdo que capturou por fgets (mesmo se o usuário apenas der "Enter"), sem prender o terminal.
 * - Tratamento de caracteres (input_c): Se o usuário injetar um EOF (como no caso de um pipe ou fim de arquivo), ele agora retorna imediatamente \0, ao invés de quebrar o terminal travando a CPU do container.
 * - Leitura Numérica (input_d, input_f, etc): Fazem a conversão em uma única tentativa (Single-Shot). Caso o input seja lixo (errno == ERANGE ou falha completa no parse onde endptr == aux), a função apenas retorna o valor neutro (0), permitindo que a aplicação prossiga e lide com a falha (mesmo que com retornos cegos, afinal ainda não resolvemos o Ponto 3).
 * - Externalização de Buffers (Passo 2): As funções não alocam mais buffers fixos ("magic numbers" como `char aux[5120]`) na stack. Agora elas recebem o buffer alocado de forma injetada pelo Caller (`buffer` e `buffer_size`), mitigando Stack Overflows.
 * - Elas usam o padrão da injeção de dependência exigindo que o chamador forneça o buffer local e o buffer_size.
 * - Error Status Codes (Passo 3): As funções agora retornam um PSS_STATUS informando o resultado da operação (OK, EOF, EMPTY, PARSE, RANGE). O valor requisitado passou a ser injetado através de ponteiros (`*saida`). Isso garante que a aplicação não confunda um erro de conversão com uma entrada "0" legítima do usuário.
 * - Essa nova versão da biblioteca (pss2) agora respeita o Princípio da Responsabilidade Única (SRP): ela lida apenas com a captura e conversão da String para o tipo correto. A responsabilidade de o que fazer com a falha (tentar de novo, abortar, logar) foi devolvida a quem chama a função!
 */
#include "pss2.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <inttypes.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <ctype.h>

/* função privada do módulo */
static void limpa_buffer() {
    int lixo;
    while ((lixo = getchar()) != '\n' && lixo != '\r' && lixo != EOF);
}

PSS_STATUS input_c(const char *mensagem, char *saida) {
    int x;
    if (!saida) return PSS_ERR_INVALID_BUFFER;
    if (mensagem) {
        printf("%s", mensagem);
    }
    x = fgetc(stdin); 
    if (x == EOF) return PSS_ERR_EOF;
    if (x != '\n') {
        limpa_buffer();
    }
    *saida = (char)x;
    return PSS_OK;
}

PSS_STATUS input_s(const char *mensagem, char *buffer, size_t tamanho) {	
    if (!buffer || tamanho == 0) return PSS_ERR_INVALID_BUFFER;
    if (mensagem) {
        printf("%s", mensagem);
    }
    if (fgets(buffer, (int)tamanho, stdin) == NULL) {
        buffer[0] = '\0';
        return PSS_ERR_EOF;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strlen(buffer) == 0) return PSS_ERR_EMPTY;
    if (strlen(buffer) == tamanho - 1) {
        limpa_buffer();
    }
    return PSS_OK;
}

PSS_STATUS input_d(const char *mensagem, char *buffer, size_t buffer_size, int *saida) {
    char *endptr;
    long temp;
    PSS_STATUS status;
    
    if (!saida) return PSS_ERR_INVALID_BUFFER;
    
    status = input_s(mensagem, buffer, buffer_size);
    if (status != PSS_OK) return status;
    
    errno = 0;
    temp = strtol(buffer, &endptr, 10);
    while (isspace((unsigned char)*endptr)) endptr++;
    
    if (endptr == buffer) return PSS_ERR_PARSE;
    
    if (errno == ERANGE || temp < INT_MIN || temp > INT_MAX) {
        *saida = (temp < 0) ? INT_MIN : INT_MAX;
        return PSS_ERR_RANGE;
    }
    
    *saida = (int)temp;
    return PSS_OK;
}

PSS_STATUS input_f(const char *mensagem, char *buffer, size_t buffer_size, float *saida) {
    char *endptr;
    float x;
    PSS_STATUS status;
    
    if (!saida) return PSS_ERR_INVALID_BUFFER;
    
    status = input_s(mensagem, buffer, buffer_size);
    if (status != PSS_OK) return status;
    
    errno = 0;
    x = strtof(buffer, &endptr);
    while (isspace((unsigned char)*endptr)) endptr++;
    
    if (endptr == buffer) return PSS_ERR_PARSE;
    
    if (errno == ERANGE) {
        *saida = (x > 0) ? FLT_MAX : -FLT_MAX;
        return PSS_ERR_RANGE;
    }
    
    *saida = x;
    return PSS_OK;
}

PSS_STATUS input_ld(const char *mensagem, char *buffer, size_t buffer_size, long int *saida) {
    char *endptr;
    long int x;
    PSS_STATUS status;
    
    if (!saida) return PSS_ERR_INVALID_BUFFER;
    
    status = input_s(mensagem, buffer, buffer_size);
    if (status != PSS_OK) return status;
    
    errno = 0;
    x = strtol(buffer, &endptr, 10);
    while (isspace((unsigned char)*endptr)) endptr++;
    
    if (endptr == buffer) return PSS_ERR_PARSE;
    
    if (errno == ERANGE) {
        *saida = (x > 0) ? LONG_MAX : LONG_MIN;
        return PSS_ERR_RANGE;
    }
    
    *saida = x;
    return PSS_OK;
}

PSS_STATUS input_lf(const char *mensagem, char *buffer, size_t buffer_size, double *saida) {
    char *endptr;
    double x;
    PSS_STATUS status;
    
    if (!saida) return PSS_ERR_INVALID_BUFFER;
    
    status = input_s(mensagem, buffer, buffer_size);
    if (status != PSS_OK) return status;
    
    errno = 0;
    x = strtod(buffer, &endptr); 
    while (isspace((unsigned char)*endptr)) endptr++;
    
    if (endptr == buffer) return PSS_ERR_PARSE;
    
    if (errno == ERANGE) {
        *saida = (x > 0) ? DBL_MAX : -DBL_MAX;
        return PSS_ERR_RANGE;
    }
    
    *saida = x;
    return PSS_OK; 
}

PSS_STATUS input_Lf(const char *mensagem, char *buffer, size_t buffer_size, long double *saida) {
    char *endptr;
    long double x;
    PSS_STATUS status;
    
    if (!saida) return PSS_ERR_INVALID_BUFFER;
    
    status = input_s(mensagem, buffer, buffer_size);
    if (status != PSS_OK) return status;
    
    errno = 0;
    x = strtold(buffer, &endptr); 
    while (isspace((unsigned char)*endptr)) endptr++;
    
    if (endptr == buffer) return PSS_ERR_PARSE;
    
    if (errno == ERANGE) {
        *saida = (x > 0) ? LDBL_MAX : -LDBL_MAX;
        return PSS_ERR_RANGE;
    }
    
    *saida = x;
    return PSS_OK; 
}
