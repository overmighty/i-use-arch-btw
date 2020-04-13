#include <iuab/compiler.h>
#include <iuab/vm.h>

#include <cstring>
#include <iostream>
#include <memory>

int compile(FILE *src, uint8_t **dest) {
    auto compiler = std::make_unique<iuab_compiler>();
    iuab_compiler_init(compiler.get(), src);

    iuab_error result = iuab_compiler_run(compiler.get(), dest);
    iuab_compiler_fini(compiler.get());

    if (result != IUAB_ERROR_SUCCESS) {
        std::cerr << "error: " << iuab_strerror(result) << " at line "
            << compiler.get()->token.line << ", col "
            << compiler.get()->token.col << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int execute(uint8_t *bytecode) {
    auto vm = std::make_unique<iuab_vm>();
    iuab_vm_init(vm.get(), bytecode, stdin, stdout);

    size_t last_op_pos;
    iuab_error result = iuab_vm_run(vm.get(), &last_op_pos);

    if (result != IUAB_ERROR_SUCCESS) {
        std::cerr << "error: " << iuab_strerror(result) << " at 0x" <<
            std::uppercase << std::hex << last_op_pos << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <file>" << std::endl;
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    FILE *src = fopen(filename, "rbe");

    if (src == nullptr) {
        std::cerr << "error: couldn't open " << filename << ": " <<
            strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    uint8_t *bytecode;
    int result = compile(src, &bytecode);
    fclose(src);

    if (result != EXIT_SUCCESS) {
        return result;
    }

    result = execute(bytecode);
    free(bytecode);
    return result;
}
