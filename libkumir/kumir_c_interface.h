#ifndef KUMIR_C_INTERFACE_H
#define KUMIR_C_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

    // Initialization
    void* kumir_create_instance(const char *text_encoding);

    // Compiler control
    void kumir_set_program_text(void *kumir_ptr, const char *text);
    void kumir_change_program_text(void *kumir_ptr, const char *text, int from, int length);
    int kumir_compile_errors(void *kumir_ptr, struct kumir_compile_error_t *errors);

    // Runtime control
    void kumir_reset();
    void kumir_evaluate_next_instruction();
    int kumir_has_more_instructions();
    void kumir_runtime_error(kumir_runtime_error_t *error);

#ifdef __cplusplus
}
#endif

#endif // KUMIR_C_INTERFACE_H
