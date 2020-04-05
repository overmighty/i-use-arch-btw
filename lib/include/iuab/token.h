#ifndef IUAB_TOKEN_H
#define IUAB_TOKEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/* Types of "I use Arch btw" source code tokens. */
enum iuab_token_type {
    IUAB_TOKEN_EOF = -1, /* End of file. */

    IUAB_TOKEN_I = 0,
    IUAB_TOKEN_USE,
    IUAB_TOKEN_ARCH,
    IUAB_TOKEN_LINUX,
    IUAB_TOKEN_BTW,
    IUAB_TOKEN_BY,
    IUAB_TOKEN_THE,
    IUAB_TOKEN_WAY,
    IUAB_TOKEN_GENTOO,

    IUAB_TOKEN_INVAL /* Invalid token. */
};

/* An "I use Arch btw" source code token. */
struct iuab_token {
    enum iuab_token_type type;
    size_t line;
    size_t col; /* Column. */
};

/* Returns the name of the given token type as a string. */
char *iuab_token_type_name(enum iuab_token_type tok_type);

#ifdef __cplusplus
}
#endif

#endif /* IUAB_TOKEN_H */
