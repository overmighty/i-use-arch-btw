#include "iuab/buffer.h"
#include "iuab/context.h"
#include "iuab/errors.h"
#include "iuab/targets.h"
#include "iuab/token.h"

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

struct FileDeleter {
    void operator()(std::FILE *file) {
        std::fclose(file);
    }
};

int compile(iuab_target target, std::FILE *src, iuab_buffer *dst) {
    iuab_token last_token;
    iuab_error error = iuab_compile(target, src, dst, &last_token);

    if (error != IUAB_ERROR_SUCCESS) {
        std::cerr << "compile-time error: " << iuab_strerror(error)
                  << " at line " << last_token.line << ", col "
                  << last_token.col << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void debug_handler([[maybe_unused]] iuab_context *ctx) {
    std::cout << "debug handler called\n";
}

int run(iuab_target target, iuab_buffer *program) {
    iuab_context ctx;
    iuab_context_init(&ctx, program->data, stdin, stdout, debug_handler);
    iuab_error error = iuab_run(target, &ctx);

    if (error != IUAB_ERROR_SUCCESS) {
        std::cerr << "run-time error: " << iuab_strerror(error) << " at "
                  << std::hex << std::showbase
                  << reinterpret_cast<uintptr_t>(ctx.ip - 1) << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int compile_and_run(const char *filename) {
    std::unique_ptr<std::FILE, FileDeleter> src{std::fopen(filename, "rbe")};

    if (src == nullptr) {
        std::cerr << "failed to open source file: " << std::strerror(errno)
                  << "\n";
        return EXIT_FAILURE;
    }

    iuab_buffer program;
    iuab_error error = iuab_buffer_init(&program);

    if (error != IUAB_ERROR_SUCCESS) {
        std::cerr << "failed to init program buffer: " << iuab_strerror(error)
                  << "\n";
        return EXIT_FAILURE;
    }

    iuab_target target = IUAB_TARGET_BYTECODE;

    int status = compile(target, src.get(), &program);

    if (status == EXIT_SUCCESS) {
        status = run(target, &program);
    }

    iuab_buffer_fini(&program);
    return status;
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source file>\n";
        return EXIT_FAILURE;
    }

    return compile_and_run(argv[1]);
}
