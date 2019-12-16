#ifndef ENUMS_H
#define ENUMS_H

namespace Kumir {

    enum ValueType {
        Undefined,
        Integer,
        Real,
        Boolean,
        String,
        Charect,
        IntegerArray,
        RealArray,
        BooleanArray,
        StringArray,
        CharectArray,
        BoolOrDecimal,
        BoolOrLiteral
    };

    enum InstructionType {
        Empty,
        VariableDeclaration,
        AlgorhitmDeclaration,
        AlgorhitmBegin,
        AlgorhitmEnd,
        LoopBegin,
        LoopEnd,
        If,
        Then,
        Else,
        Switch,
        Case,
        EndSwitchOrIf,
        Exit,
        Assignment,
        Output,
        FileOutput,
        Input,
        FileInput,
        Assert,
        Pre,
        Post,
        ConditionalEndLoop,
        ModuleDeclaration,
        ModuleEnd,
        Comment,
        Documentation,
        Import,
        AlgorhitmCall
    };

    enum LoopType {
        ForLoop,
        WhileLoop,
        TimesLoop,
        FreeLoop
    };

}

#endif // ENUMS_H
