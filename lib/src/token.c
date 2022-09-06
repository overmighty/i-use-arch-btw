// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: GPL-3.0-only

#include "iuab/token.h"

const char *iuab_token_type_name(enum iuab_token_type type) {
    switch (type) {
    case IUAB_TOKEN_EOF: return "EOF";
    case IUAB_TOKEN_I: return "i";
    case IUAB_TOKEN_USE: return "use";
    case IUAB_TOKEN_ARCH: return "arch";
    case IUAB_TOKEN_LINUX: return "linux";
    case IUAB_TOKEN_BTW: return "btw";
    case IUAB_TOKEN_BY: return "by";
    case IUAB_TOKEN_THE: return "the";
    case IUAB_TOKEN_WAY: return "way";
    case IUAB_TOKEN_GENTOO: return "gentoo";
    default: return "???";
    }
}
