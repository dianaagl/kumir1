#ifndef KUMIRCOMMONINTERFACE_H
#define KUMIRCOMMONINTERFACE_H

#include <QtCore>
#include "kumirerrors.h"


class KumirCommonInterface
{
public:

    enum State { Idle, Compiling, Running };

    // Plugin control
    virtual State state() const = 0;

    // Compiler control
    virtual void setProgramText(const QString &text) = 0;
    virtual void changeProgramText(const QString &text, int from, int length) = 0;
    virtual QList<Kumir::CompileError> compileErrors() const = 0;

    // Runtime control
    virtual void reset() = 0;
    virtual void evaluateNextInstruction() = 0;
    virtual bool hasMoreInstructions() const = 0;
    virtual Kumir::RuntimeError runtimeError() const = 0;
};



#endif // KUMIRCOMMONINTERFACE_H
