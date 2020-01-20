#include <iuab/compiler.h>

#include <stdlib.h> /* for malloc */

#define EMIT_BYTE(comp, byte) (iuab_buffer_put_byte(comp->output, byte))
#define EMIT_SIZE(comp, size) (iuab_buffer_put_size(comp->output, size))

#define STACK_PUSH(comp, n) (comp->stack[comp->depth++] = (n))
#define STACK_POP(comp)     (comp->stack[--comp->depth])

#define IS_ARITHM_TOKEN(type) ((type) >= IUAB_TOKEN_I && \
                               (type) <= IUAB_TOKEN_LINUX)
#define IS_IO_TOKEN(type)     ((type) == IUAB_TOKEN_BTW || \
                               (type) == IUAB_TOKEN_BY || \
                               (type) == IUAB_TOKEN_GENTOO)

static void iuab_compiler_advance(struct iuab_compiler *comp) {
    comp->token = iuab_lexer_next_token(comp->lexer);
}

enum iuab_error iuab_compiler_init(struct iuab_compiler *comp, FILE *input) {
    comp->lexer = malloc(sizeof(struct iuab_lexer));

    if (!comp->lexer) {
        return IUAB_ERROR_MEMORY;
    }

    iuab_lexer_init(comp->lexer, input);
    iuab_compiler_advance(comp);
    comp->output = malloc(sizeof(struct iuab_buffer));

    if (!comp->output) {
        free(comp->lexer);
        return IUAB_ERROR_MEMORY;
    }

    if (iuab_buffer_init(comp->output) != IUAB_ERROR_SUCCESS) {
        free(comp->output);
        free(comp->lexer);
        return IUAB_ERROR_MEMORY;
    }

    comp->depth = 0;
    comp->error = IUAB_ERROR_SUCCESS;
    return IUAB_ERROR_SUCCESS;
}

static void iuab_compiler_arithm_op(struct iuab_compiler *comp) {
    enum iuab_token_type tok_type = comp->token.type;
    /* Operation's operand: number of consecutive occurrences of the token */
    uint8_t i = 1;
    iuab_compiler_advance(comp);

    while (tok_type == comp->token.type) {
        i++;
        iuab_compiler_advance(comp);
    }

    if (i == 0) {
        /* The only supported arithmetic operations are basically additions and
         * subtractions, which are useless with an addend/subtrahend of 0 */
        return;
    }

    switch (tok_type) {
    case IUAB_TOKEN_I:     EMIT_BYTE(comp, IUAB_OP_INCP); break;
    case IUAB_TOKEN_USE:   EMIT_BYTE(comp, IUAB_OP_DECP); break;
    case IUAB_TOKEN_ARCH:  EMIT_BYTE(comp, IUAB_OP_INCV); break;
    case IUAB_TOKEN_LINUX: EMIT_BYTE(comp, IUAB_OP_DECV); break;
    default:               EMIT_BYTE(comp, IUAB_OP_INVAL);
    }

    EMIT_BYTE(comp, i);
}

static void iuab_compiler_io_op(struct iuab_compiler *comp) {
    switch (comp->token.type) {
    case IUAB_TOKEN_BTW:    EMIT_BYTE(comp, IUAB_OP_WRITE); break;
    case IUAB_TOKEN_BY:     EMIT_BYTE(comp, IUAB_OP_READ); break;
    case IUAB_TOKEN_GENTOO: EMIT_BYTE(comp, IUAB_OP_DEBUG); break;
    default:                EMIT_BYTE(comp, IUAB_OP_INVAL);
    }

    iuab_compiler_advance(comp);
}

static void iuab_compiler_begin_loop(struct iuab_compiler *comp) {
    if (comp->depth >= IUAB_COMPILER_STACK_DEPTH) {
        /* Stack is full: cannot begin another nested loop */
        comp->error = IUAB_ERROR_DEPTHMAX;
        return;
    }

    EMIT_BYTE(comp, IUAB_OP_JMPZ);
    /* Push the index of the operation's operand to the top of the stack, so
     * that the operand can be set once we know where the loop ends */
    STACK_PUSH(comp, comp->output->len);
    /* Reserve space for the operand */
    comp->output->len += sizeof(size_t);
    iuab_compiler_advance(comp);
}

static void iuab_compiler_end_loop(struct iuab_compiler *comp) {
    if (comp->depth < 1) {
        /* Stack is empty: no loop to end */
        comp->error = IUAB_ERROR_DEPTHMIN;
        return;
    }

    size_t jmpz_operand_pos = STACK_POP(comp);
    EMIT_BYTE(comp, IUAB_OP_JMPNZ);
    EMIT_SIZE(comp, jmpz_operand_pos + sizeof(size_t));

    if (comp->output->data) {
        /* Set the previously unknown operand for the JMPZ operation */
        *((size_t *) &comp->output->data[jmpz_operand_pos]) = comp->output->len;
    }

    iuab_compiler_advance(comp);
}

static void iuab_compiler_finalize_output(struct iuab_compiler *comp) {
    EMIT_BYTE(comp, IUAB_OP_RET);
    iuab_buffer_trim(comp->output);
}

enum iuab_error iuab_compiler_run(struct iuab_compiler *comp, uint8_t **dest) {
    while (comp->token.type != IUAB_TOKEN_EOF) {
        if (IS_ARITHM_TOKEN(comp->token.type)){
            iuab_compiler_arithm_op(comp);
        } else if (IS_IO_TOKEN(comp->token.type)) {
            iuab_compiler_io_op(comp);
        } else if (comp->token.type == IUAB_TOKEN_THE) {
            iuab_compiler_begin_loop(comp);
        } else if (comp->token.type == IUAB_TOKEN_WAY) {
            iuab_compiler_end_loop(comp);
        } else {
            comp->error = IUAB_ERROR_INVALTOK;
        }

        if (comp->error == IUAB_ERROR_SUCCESS && !comp->output->data) {
            comp->error = IUAB_ERROR_MEMORY;
        }

        if (comp->error != IUAB_ERROR_SUCCESS) {
            return comp->error;
        }
    }

    if (comp->depth) {
        comp->error = IUAB_ERROR_DEPTHNZ;
        return comp->error;
    }

    iuab_compiler_finalize_output(comp);
    *dest = comp->output->data;
    return comp->error;
}

void iuab_compiler_fini(struct iuab_compiler *comp) {
    free(comp->lexer);
    free(comp->output);
}
