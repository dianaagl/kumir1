#ifndef KUMIRCOMPILERANDRUNTIME_H
#define KUMIRCOMPILERANDRUNTIME_H

#include <QtCore>
#include "kumircommoninterface.h"


class KumirCompilerAndRuntime
    : public QThread
    , public KumirCommonInterface
{
    Q_OBJECT
public:
    KumirCompilerAndRuntime(QObject *parent = 0);

    // Plugin control
    State state() const;

    // Compiler control
    void setProgramText(const QString &text);
    void changeProgramText(const QString &text, int from, int length);
    QList<Kumir::CompileError> compileErrors() const;

    // Runtime control
    void reset();
    void evaluateNextInstruction();
    bool hasMoreInstructions() const;
    Kumir::RuntimeError runtimeError() const;

private:
    // methods

    // fields
    State e_state;
};

#endif // KUMIRCOMPILERANDRUNTIME_H
