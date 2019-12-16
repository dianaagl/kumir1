#include "kumircompilerandruntime.h"


KumirCompilerAndRuntime::KumirCompilerAndRuntime(QObject *parent) :
    QThread(parent)
{
    e_state = Idle;
}

KumirCommonInterface::State KumirCompilerAndRuntime::state() const
{
    return e_state;
}

void KumirCompilerAndRuntime::setProgramText(const QString &text)
{

}

void KumirCompilerAndRuntime::changeProgramText(const QString &text, int from, int length)
{

}

QList<Kumir::CompileError> KumirCompilerAndRuntime::compileErrors() const
{

}

Kumir::RuntimeError KumirCompilerAndRuntime::runtimeError() const
{

}

void KumirCompilerAndRuntime::reset()
{

}

void KumirCompilerAndRuntime::evaluateNextInstruction()
{

}

bool KumirCompilerAndRuntime::hasMoreInstructions() const
{

}


Q_EXPORT_PLUGIN2(libkumir, KumirCompilerAndRuntime)
