#ifndef KUMIRERRORS_H
#define KUMIRERRORS_H

#ifdef __cplusplus
#include <QString>

namespace Kumir {

    struct CompileError {
        QString text;
        int code;
        int position;
        int length;
    };

    struct RuntimeError {
        QString text;
        int code;
        int position;
        int length;
    };
}

#endif

struct kumir_compile_error_t {
    char text[256];
    int code;
    int position;
    int length;
};

struct kumir_runtime_error_t {
    char text[256];
    int code;
    int position;
    int length;
};

#endif // KUMIRERRORS_H
