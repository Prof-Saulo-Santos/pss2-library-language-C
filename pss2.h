#ifndef PSS2_H
#define PSS2_H

#include <stddef.h>

/**
 * Códigos de Status / Error Codes da biblioteca (Passo 3)
 */
typedef enum {
    PSS_OK = 0,
    PSS_ERR_EOF = -1,
    PSS_ERR_EMPTY = -2,
    PSS_ERR_PARSE = -3,
    PSS_ERR_RANGE = -4,
    PSS_ERR_INVALID_BUFFER = -5
} PSS_STATUS;

PSS_STATUS input_c(const char *mensagem, char *saida);
PSS_STATUS input_s(const char *mensagem, char *buffer, size_t tamanho);
PSS_STATUS input_d(const char *mensagem, char *buffer, size_t buffer_size, int *saida);
PSS_STATUS input_f(const char *mensagem, char *buffer, size_t buffer_size, float *saida);
PSS_STATUS input_ld(const char *mensagem, char *buffer, size_t buffer_size, long int *saida);
PSS_STATUS input_lf(const char *mensagem, char *buffer, size_t buffer_size, double *saida);
PSS_STATUS input_Lf(const char *mensagem, char *buffer, size_t buffer_size, long double *saida);

#endif
