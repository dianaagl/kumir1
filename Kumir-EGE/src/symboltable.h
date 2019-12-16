#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <QtCore>
#include "enums.h"

namespace Kum {

//    enum ValueType {
//        Int,
//        Real,
//        Bool,
//        Char,
//        String,
//        IntVector,
//        RealVector,
//        BoolVector,
//        CharVector,
//        StringVector,

//        BoolOrLiteral = 126,
//        BoolOrNumeric = 127,

//        UndefinedType = 0
//    };

    typedef PeremType ValueType;

    enum ValueModifier {
        NoModifier = 0x00,
        ConstModifier = 0x01,
        ArgumentModifier = 0x02,
        ReferenceModifier = 0x04,
        RetvalueModifier = 0x08,
        ResParamModifier = 0x10
    };

    struct ValueReference {
        class SymbolTable *table;
        int id;
    };


    Q_ENUMS(ValueType);
    Q_FLAGS(ValueModifier ValueModifier);

    class SymbolTable
    {
    public:
        explicit SymbolTable(int moduleId,
                             int functionId,
                             const QString &functionName
                             );
        static void registerTable(SymbolTable* table);
        static void deregisterTable(SymbolTable* table);
        inline bool isGlobalTable() const { return i_functionId>-1; }
        inline int moduleId() const { return i_moduleId; }
        inline int functionId() const { return i_functionId; }
        inline int returnValueId() const { return i_returnValueId; }
        void setInitialValue(int id, const QVector<QVariant> &data);
        PeremType returnValueType() const;
        int addConstant(ValueType type, const QVariant &value);
        int addConstant(ValueType type, const QString &unparsedValue);
        int addVariable(ValueType type,
                        const QString &name,
                        const QString &initialValue,
                        int dim=0,
                        int start0=0, int end0=0,
                        int start1=0, int end1=0,
                        int start2=0, int end2=0);
        int addReturnValue(ValueType type);
        int addArgument(ValueType type,
                        const QString &name,
                        const QString &initialValue,
                        bool isRes,
                        bool isArgRes,
                        int dim=0,
                        int start0=0, int end0=0,
                        int start1=0, int end1=0,
                        int start2=0, int end2=0);
        void setValue(int id, const QVariant &value);
        void setValue(int id, int index0, const QVariant &value);
        void setValue(int id, int index0, int index1, const QVariant &value);
        void setValue(int id, int index0, int index1, int index2, const QVariant &value);
        inline void setMappedIdMode(bool v) { b_mappedIdMode = v; }
        inline bool isMappedIdMode() const { return b_mappedIdMode; }
        inline void setMapId(int tableId, int searchId) { m_ids[searchId] = tableId; }
        bool isConstant(int id) const;
        bool hasMappedId(int searchId) const;
        PeremType type(int id) const;
        QVariant returnValue() const;
        bool hasReturnValue() const;
        QVariant value(int id) const;
        QVariant value(int id, int index0) const;
        QVariant value(int id, int index0, int index1) const;
        QVariant value(int id, int index0, int index1, int index2) const;
        QPair<int,int> vectorBounds(int id, int dimension) const;
        int setBounds(int id, const QList<int> &bounds);
        int dimension(int id) const;
        bool isInitialized(int id) const;
        bool isArgument(int id) const;
        bool isRes(int id) const;
        bool isArgRes(int id) const;
        void clear();
        SymbolTable* clone() const;
        ValueReference reference(int id) const;
        int setReference(int id, const ValueReference &ref);
        int setVectorValue(int id, const ValueReference &ref);
    private:
        int dataIndex(const ValueReference ref, int index0, int index1, int index2) const;
        int i_moduleId;
        int i_functionId;
        int i_returnValueId;
        QString s_modName;
        QString s_funcName;
        int applyInitialValue(int id);
        static QList< QList <QList<QVariant> > > extract3DArray(const QString &data);
        static QList <QList<QVariant> > extract2DArray(const QString &data);
        static QList<QVariant> extract1DArray(const QString &data);

        struct Element {
            QString name;
            ValueType type;
            int dimension;
            int vectorStarts[3];
            int vectorEnds[3];
            QFlags<ValueModifier> modifiers;
            ValueReference reference;
            QVector<QVariant> data;
            QString initialValue;
//            QMap<int,bool> explicitInitialized;
        };

        QList<Element> l_elements;
        QMap<int,int> m_ids;
        bool b_mappedIdMode;

        static QList<SymbolTable*> l_allTables;
    };
}

#endif // SYMBOLTABLE_H
