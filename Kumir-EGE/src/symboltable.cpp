#include <QtCore>
#include "symboltable.h"
#include "error.h"

using namespace Kum;

SymbolTable::SymbolTable(int moduleId, int functionId, const QString &functionName)
{
    i_moduleId = moduleId;
    i_functionId = functionId;
    i_returnValueId = -1;
    b_mappedIdMode = false;
    s_funcName = functionName;
}

void SymbolTable::registerTable(SymbolTable *table)
{
    if (!l_allTables.contains(table))
        l_allTables << table;
}

void SymbolTable::deregisterTable(SymbolTable *table)
{
    l_allTables.removeAll(table);
}

int SymbolTable::addConstant(ValueType type, const QVariant &value)
{
    Element e;
    e.type = type;
    e.name = "!CONST";
    e.dimension = 0;
    e.vectorStarts[0] = e.vectorStarts[1] = e.vectorStarts[2] = 0;
    e.vectorEnds[0] = e.vectorEnds[1] = e.vectorEnds[2] = 0;
    e.modifiers = ConstModifier;
    e.reference.table = NULL;
    e.reference.id = -1;
    e.data.resize(1);
    e.data[0] = value;
    l_elements << e;
    return l_elements.size()-1;
}

int SymbolTable::addArgument(ValueType type,
                             const QString &name,
                             const QString &initialValue,
                             bool isRes, bool isArgRes, int dim,
                             int start0, int end0,
                             int start1, int end1,
                             int start2, int end2)
{
    Element e;
    e.type = type;
    e.name = name;
    e.initialValue = initialValue;
    e.dimension = dim;
    e.vectorStarts[0] = start0;
    e.vectorEnds[0] = end0;
    e.vectorStarts[1] = start1;
    e.vectorEnds[1] = end1;
    e.vectorStarts[2] = start2;
    e.vectorEnds[2] = end2;
    e.reference.table = NULL;
    e.reference.id = -1;
    e.modifiers = ArgumentModifier;
    if (isRes || isArgRes)
        e.modifiers |= ReferenceModifier;
    if (isRes)
        e.modifiers |= ResParamModifier;
    int size = 1;
    if (dim>0) {
        int size0 = end0 - start0 + 1;
        int size1 = end1 - start1 + 1;
        int size2 = end2 - start2 + 1;
        switch (dim) {
        case 1:
            size = size0;
            break;
        case 2:
            size = size0*size1;
            break;
        case 3:
            size = size0*size1*size2;
            break;
        }
    }
    e.data.resize(size);
    l_elements << e;
    return l_elements.size()-1;
}

int SymbolTable::addVariable(ValueType type,
                             const QString &name,
                             const QString &initialValue,
                             int dim,
                             int start0, int end0,
                             int start1, int end1,
                             int start2, int end2)
{
    Element e;
    e.type = type;
    e.name = name;
    e.initialValue = initialValue;
    e.dimension = dim;
    e.vectorStarts[0] = start0;
    e.vectorEnds[0] = end0;
    e.vectorStarts[1] = start1;
    e.vectorEnds[1] = end1;
    e.vectorStarts[2] = start2;
    e.vectorEnds[2] = end2;
    e.reference.table = NULL;
    e.reference.id = -1;
    e.modifiers = NoModifier;
    int size = 1;
    if (dim>0) {
        int size0 = end0 - start0 + 1;
        int size1 = end1 - start1 + 1;
        int size2 = end2 - start2 + 1;
        switch (dim) {
        case 1:
            size = size0;
            break;
        case 2:
            size = size0*size1;
            break;
        case 3:
            size = size0*size1*size2;
            break;
        }
    }
    e.data.resize(size);
    l_elements << e;
    return l_elements.size()-1;
}

int SymbolTable::addReturnValue(ValueType type)
{
    Element e;
    e.type = type;
    e.dimension = 0;
    e.name = "!RETVAL";
    e.vectorStarts[0] = e.vectorStarts[1] = e.vectorStarts[2] = 0;
    e.vectorEnds[0] = e.vectorEnds[1] = e.vectorEnds[2] = 0;
    e.modifiers = RetvalueModifier;
    e.reference.table = NULL;
    e.reference.id = -1;
    e.data.resize(1);
    l_elements << e;
    i_returnValueId = l_elements.size()-1;
    return l_elements.size()-1;
}


QList<SymbolTable*> SymbolTable::l_allTables = QList<SymbolTable*>();


ValueReference SymbolTable::reference(int id) const
{
    int realId = id;
    if (b_mappedIdMode) {
        Q_ASSERT(m_ids.contains(id));
        realId = m_ids[id];
    }
    Q_ASSERT(realId<l_elements.size());
    SymbolTable *table = const_cast<SymbolTable*>(this);
    while (table->l_elements[realId].modifiers.testFlag(ReferenceModifier)) {
        int d = realId;
        SymbolTable *t = table->l_elements[realId].reference.table;
        d = table->l_elements[realId].reference.id;
        table = t;
        realId = d;
    }
    ValueReference ref;
    ref.table = table;
    ref.id = realId;
    return ref;
}

int SymbolTable::dataIndex(const ValueReference ref, int index0, int index1, int index2) const
{
    int d = ref.table->l_elements[ref.id].dimension;
    Q_ASSERT(d<=3);
    int index = 0;
    if (d==1) {
        int start0 = ref.table->l_elements[ref.id].vectorStarts[0];
        int end0 = ref.table->l_elements[ref.id].vectorEnds[0];
        Q_ASSERT(index0>=start0 && index0<=end0);
        index = index0 - start0;
    }
    else if (d==2) {
        int start0 = ref.table->l_elements[ref.id].vectorStarts[0];
        int end0 = ref.table->l_elements[ref.id].vectorEnds[0];
        int len0 = end0 - start0 + 1;
        Q_ASSERT(index0>=start0 && index0<=end0);
        int start1 = ref.table->l_elements[ref.id].vectorStarts[1];
        int end1 = ref.table->l_elements[ref.id].vectorEnds[1];
        Q_ASSERT(index1>=start1 && index1<=end1);
        int len1 = end1 - start1 + 1;
        Q_UNUSED(len0);
        int rowNo = index0 - start0;
        int colNo = index1 - start1;
        int elementsInRow = len1;
        index = rowNo * elementsInRow + colNo;
    }
    else if (d==3) {
        int start0 = ref.table->l_elements[ref.id].vectorStarts[0];
        int end0 = ref.table->l_elements[ref.id].vectorEnds[0];
        int len0 = end0 - start0 + 1;
        Q_ASSERT(index0>=start0 && index0<=end0);
        int start1 = ref.table->l_elements[ref.id].vectorStarts[1];
        int end1 = ref.table->l_elements[ref.id].vectorEnds[1];
        int len1 = end1 - start1 + 1;
        Q_ASSERT(index1>=start1 && index1<=end1);
        int start2 = ref.table->l_elements[ref.id].vectorStarts[2];
        int end2 = ref.table->l_elements[ref.id].vectorEnds[2];
        int len2 = end2 - start2 + 1;
        Q_ASSERT(index2>=start2 && index2<=end2);
        int layerNo = index0 - start0;
        int rowNo = index1 - start1;
        int colNo = index2 - start2;
        int elementsInRow = len2;
        int elementsInLayer = elementsInRow * len1;
        Q_UNUSED(len0);
        index = layerNo * elementsInLayer + rowNo * elementsInRow + colNo;
    }
    Q_ASSERT(index>=0);
    Q_ASSERT(index<ref.table->l_elements[ref.id].data.size());
    return index;
}



PeremType SymbolTable::type(int id) const
{
    const ValueReference ref = reference(id);
    return ref.table->l_elements[ref.id].type;
}



QVariant SymbolTable::value(int id, int index0, int index1, int index2) const
{
    const ValueReference ref = reference(id);
    int index = dataIndex(ref, index0, index1, index2);
    int realId = id;
    if (b_mappedIdMode) {
        Q_ASSERT(m_ids.contains(id));
        realId = m_ids[id];
    }
//    if (l_elements[realId].modifiers.testFlag(ResParamModifier)) {
//        if (!l_elements[realId].explicitInitialized.contains(index)) {
//            return QVariant::Invalid;
//        }
//    }
    return ref.table->l_elements[ref.id].data[index];
}

QVariant SymbolTable::value(int id, int index0, int index1) const
{
    return value(id, index0, index1, 0);
}

QVariant SymbolTable::value(int id, int index0) const
{
    return value(id, index0, 0, 0);
}

QVariant SymbolTable::value(int id) const
{
    return value(id, 0, 0, 0);
}

void SymbolTable::setValue(int id, const QVariant &value)
{
    setValue(id, 0, 0, 0, value);
}

void SymbolTable::setValue(int id, int index0, const QVariant &value)
{
    setValue(id, index0, 0, 0, value);
}

void SymbolTable::setValue(int id, int index0, int index1, const QVariant &value)
{
    setValue(id, index0, index1, 0, value);
}

void SymbolTable::setValue(int id, int index0, int index1, int index2, const QVariant &value)
{
    const ValueReference ref = reference(id);
    int index = dataIndex(ref, index0, index1, index2);
    ref.table->l_elements[ref.id].data[index] = value;
    int realId = id;
    if (b_mappedIdMode) {
        Q_ASSERT(m_ids.contains(id));
        realId = m_ids[id];
    }
//    if (l_elements[realId].modifiers.testFlag(ResParamModifier)) {
//        l_elements[realId].explicitInitialized[index] = true;
//    }
}



bool SymbolTable::isInitialized(int id) const
{
    const ValueReference ref = reference(id);
    Q_ASSERT(ref.table->l_elements[ref.id].dimension==0);
    return ref.table->l_elements[ref.id].data[0].isValid();
}

QPair<int,int> SymbolTable::vectorBounds(int id, int dimension) const
{
    const ValueReference ref = reference(id);
    Q_ASSERT(dimension<ref.table->l_elements[ref.id].dimension);
    int start = ref.table->l_elements[ref.id].vectorStarts[dimension];
    int end = ref.table->l_elements[ref.id].vectorEnds[dimension];
    return QPair<int,int>(start, end);
}

int SymbolTable::dimension(int id) const
{
    const ValueReference ref = reference(id);
    Q_ASSERT(ref.table->l_elements[ref.id].dimension<=3);
    return ref.table->l_elements[ref.id].dimension;
}

bool SymbolTable::hasReturnValue() const
{
    return i_returnValueId != -1;
}

QVariant SymbolTable::returnValue() const
{
    return l_elements[i_returnValueId].data[0];
}

PeremType SymbolTable::returnValueType() const
{
    if (!hasReturnValue())
        return none;
    return l_elements[i_returnValueId].type;
}

SymbolTable* SymbolTable::clone() const
{
    SymbolTable *st = new SymbolTable(i_moduleId, i_functionId, s_funcName);
    st->m_ids = m_ids;
    st->b_mappedIdMode = b_mappedIdMode;
    st->i_returnValueId = i_returnValueId;
    st->s_modName = s_modName;
    st->s_funcName = s_funcName;
    for (int i=0; i<l_elements.size(); i++) {
        Element e;
        Element ee = l_elements[i];
        e.name = ee.name;
        e.type = ee.type;
        e.dimension = ee.dimension;
        e.data = ee.data;
        e.vectorStarts[0] = ee.vectorStarts[1];
        e.vectorStarts[1] = ee.vectorStarts[1];
        e.vectorStarts[2] = ee.vectorStarts[2];
        e.vectorEnds[0] = ee.vectorEnds[0];
        e.vectorEnds[1] = ee.vectorEnds[1];
        e.vectorEnds[2] = ee.vectorEnds[2];
        e.modifiers = ee.modifiers;
        e.initialValue = ee.initialValue;
        if (ee.modifiers.testFlag(ConstModifier)) {
            e.data = ee.data;
        }
        else {
            if (e.dimension==0) {
                e.data = QVector<QVariant>(1);
            }
        }
        e.reference.table = ee.reference.table;
        e.reference.id = ee.reference.id;
        st->l_elements << e;
    }
    return st;
}

void SymbolTable::setInitialValue(int id, const QVector<QVariant> &data)
{
    l_elements[id].data = data;
}

bool SymbolTable::hasMappedId(int searchId) const
{
    bool value = false;
    if (b_mappedIdMode) {
        value = m_ids.contains(searchId);
    }
    else {
        value = searchId>=0 && searchId<l_elements.size();
    }
    return value;
}

int SymbolTable::setReference(int id, const ValueReference &ref)
{
    int realId = id;
    if (b_mappedIdMode) {
        Q_ASSERT(m_ids.contains(id));
        realId = m_ids[id];
    }
    l_elements[realId].reference.table = ref.table;
    l_elements[realId].reference.id = ref.id;
    if (l_elements[realId].modifiers.testFlag(ResParamModifier)) {
        for (int i=0; i<l_elements[realId].data.size(); i++) {
            ref.table->l_elements[ref.id].data[i] = QVariant::Invalid;
        }
    }
//    if (l_elements[realId].dimension>0) {
//        for (int i=0; i<l_elements[realId].dimension; i++) {
//            int refStart = ref.table->l_elements[ref.id].vectorStarts[i];
//            int refEnd = ref.table->l_elements[ref.id].vectorEnds[i];
//            int myStart = l_elements[realId].vectorStarts[i];
//            int myEnd = l_elements[realId].vectorEnds[i];
//            if (myStart<refStart) {
//                return 8021; // TODO set error code
//            }
//            if (myEnd>refEnd) {
//                return 8022; // TODO set error code
//            }

//        }
//    }
    return 0;
}

int SymbolTable::setVectorValue(int id, const ValueReference &ref)
{
    ValueReference targetRef = reference(id);
    targetRef.table->l_elements[targetRef.id].data = ref.table->l_elements[ref.id].data;
    if (targetRef.table->l_elements[targetRef.id].dimension>0) {
        for (int i=0; i<targetRef.table->l_elements[targetRef.id].dimension; i++) {
            int refStart = ref.table->l_elements[ref.id].vectorStarts[i];
            int refEnd = ref.table->l_elements[ref.id].vectorEnds[i];
            targetRef.table->l_elements[targetRef.id].vectorStarts[i] = refStart;
            targetRef.table->l_elements[targetRef.id].vectorEnds[i] = refEnd;
        }
    }
    return 0;
}

int SymbolTable::setBounds(int id, const QList<int> &bounds)
{
    int realId = id;
    if (b_mappedIdMode) {
        Q_ASSERT(m_ids.contains(id));
        realId = m_ids[id];
    }
    int size0 = 0;
    int size1 = 0;
    int size2 = 0;
    QVector<int> newStarts(3,-1);
    QVector<int> newEnds(3,-1);
    newStarts[0] = bounds[0];
    newEnds[0] = bounds[1];
    l_elements[realId].vectorStarts[0] = bounds[0];
    l_elements[realId].vectorEnds[0] = bounds[1];
    size0 = bounds[1] - bounds[0] + 1;
    int size = size0;
    if (l_elements[realId].dimension>1) {
        l_elements[realId].vectorStarts[1] = bounds[2];
        l_elements[realId].vectorEnds[1] = bounds[3];
        size1 = bounds[3] - bounds[2] + 1;
        size *= size1;
        newStarts[1] = bounds[2];
        newEnds[1] = bounds[3];
    }
    if (l_elements[realId].dimension>2) {
        l_elements[realId].vectorStarts[2] = bounds[4];
        l_elements[realId].vectorEnds[2] = bounds[5];
        size2 = bounds[5] - bounds[4] + 1;
        size *= size2;
        newStarts[2] = bounds[4];
        newEnds[2] = bounds[5];
    }
    if (l_elements[realId].reference.table) {
        // Check for reference bounds
        SymbolTable * refTable = l_elements[realId].reference.table;
        int refId = l_elements[realId].reference.id;
        for (int i=0; i<l_elements[realId].dimension; i++) {
            int myStart = l_elements[realId].vectorStarts[i];
            int refStart = refTable->l_elements[refId].vectorStarts[i];
            int myEnd = l_elements[realId].vectorEnds[i];
            int refEnd = refTable->l_elements[refId].vectorEnds[i];
            if (myStart!=refStart) {
                return 8019; // TODO error code
            }
            if (myEnd!=refEnd) {
                return 8019; // TODO error code
            }
        }
    }
    else {
        // Check our bounds
        for (int i=0; i<l_elements[realId].dimension; i++) {
            int myStart = l_elements[realId].vectorStarts[i];
            int newStart = newStarts[i];
            int myEnd = l_elements[realId].vectorEnds[i];
            int newEnd = newEnds[i];
            if (myStart!=newStart) {
                return 8019; // TODO error code
            }
            if (myEnd!=newEnd) {
                return 8019; // TODO error code
            }
        }
        // Set out bounds
        l_elements[realId].data.resize(size);
        if (!l_elements[realId].initialValue.isEmpty()) {
            int localError = applyInitialValue(realId);
            if (localError) {
                return localError;
            }
        }
    }
    return 0;
}


QList <QList<QVariant> > SymbolTable::extract2DArray(const QString &data) {
    QList< QList <QVariant> > result;
    QList< QVariant > row;
    QStringList rows = data.split(":");
    for ( int j=0; j<rows.count(); j++ ) {
        row.clear();
        QStringList elems = rows[j].split(".");
        for (int i=0; i<elems.count(); i++ ) {
            QByteArray ba = QByteArray::fromBase64(elems[i].toAscii());
            row << QVariant(QString::fromUtf8(ba));
        }
        result << row;
    }
    return result;
}

QList< QList <QList<QVariant> > > SymbolTable::extract3DArray(const QString &data) {
    QList< QList< QList <QVariant> > > result;
    QList< QVariant > row;
    QList< QList< QVariant > > page;
    QStringList pages = data.split("|");
    for ( int k=0; k<pages.count(); k++ ) {
        page.clear();
        QStringList rows = pages[k].split(":");
        for ( int j=0; j<rows.count(); j++ ) {
            row.clear();
            QStringList elems = rows[j].split(".");
            for (int i=0; i<elems.count(); i++ ) {
                QByteArray ba = QByteArray::fromBase64(elems[i].toAscii());
                row << QVariant(QString::fromUtf8(ba));
            }
            page << row;
        }
        result << page;
    }
    return result;
}

QList<QVariant> SymbolTable::extract1DArray(const QString &data) {
    QList<QVariant> result;
    QStringList elems = data.split(".");
    for (int i=0; i<elems.count(); i++ ) {
        QByteArray ba = QByteArray::fromBase64(elems[i].toAscii());
        result << QVariant(QString::fromUtf8(ba));
    }
    return result;
}

int SymbolTable::applyInitialValue(int id)
{
    ValueType baseType = none;
	switch (l_elements[id].type) {
	case mass_integer:
		baseType = integer;
		break;
	case mass_real:
		baseType = real;
		break;
	case mass_bool:
		baseType = boolean;
		break;
	case mass_charect:
		baseType = charect;
		break;
	case mass_string:
		baseType = kumString;
		break;
	default:
		return RUN_CONST_ARRAY_OUT_OF_BOUNDS;
		break;
	}
	int x_max = l_elements[id].vectorEnds[0]-l_elements[id].vectorStarts[0]+1;
	int y_max = l_elements[id].vectorEnds[1]-l_elements[id].vectorStarts[1]+1;
	int z_max = l_elements[id].vectorEnds[2]-l_elements[id].vectorStarts[2]+1;
	int x_base = l_elements[id].vectorStarts[0];
	int y_base = l_elements[id].vectorStarts[1];
	int z_base = l_elements[id].vectorStarts[2];
	int dim = l_elements[id].dimension;
	QString s_data = l_elements[id].initialValue;
	ValueReference r;
	r.table = this;
	r.id = id;
	if ( dim==1 ) {
		QList<QVariant> data = extract1DArray(s_data);
		QVariant value;
		for ( int x=0; x<data.count(); x++ ) {
			if (x>=x_max) {
				return RUN_CONST_ARRAY_OUT_OF_BOUNDS;

			}
			if (l_elements[id].type==mass_integer)
				value = data[x].toInt();
			if (l_elements[id].type==mass_real)
				value = data[x].toDouble();
			if (l_elements[id].type==mass_bool)
				value = data[x].toBool();
			if (l_elements[id].type==mass_charect)
				value = data[x].toChar();
			if (l_elements[id].type==mass_string)
				value = data[x].toString();
			int index = dataIndex(r, x_base+x, -1, -1);
			l_elements[id].data[index] = value;
		}
	}
	if ( dim==2 ) {
		QList< QList<QVariant> > data = extract2DArray(s_data);
		QVariant value;
		for ( int y=0; y<data.count(); y++ ) {
			for ( int x=0; x<data[y].count(); x++ ) {
				if (x>=x_max || y>=y_max) {
					return RUN_CONST_ARRAY_OUT_OF_BOUNDS;
				}
				if (l_elements[id].type==mass_integer)
					value = data[y][x].toInt();
				if (l_elements[id].type==mass_real)
					value = data[y][x].toDouble();
				if (l_elements[id].type==mass_bool)
					value = data[y][x].toBool();
				if (l_elements[id].type==mass_charect)
					value = data[y][x].toChar();
				if (l_elements[id].type==mass_string)
					value = data[y][x].toString();
				int index = dataIndex(r, x_base+x, y_base+y, -1);
				l_elements[id].data[index] = value;
			}
		}
	}
	if ( dim==3 ) {
		QList< QList< QList<QVariant> > > data = extract3DArray(s_data);
		QVariant value;
		for ( int z=0; z<data.size(); z++ ) {
			for ( int y=0; y<data[z].count(); y++ ) {
				for ( int x=0; x<data[z][y].count(); x++ ) {
					if (x>=x_max || y>=y_max || z>=z_max) {
						return RUN_CONST_ARRAY_OUT_OF_BOUNDS;
					}
					if (l_elements[id].type==mass_integer)
						value = data[z][y][x].toInt();
					if (l_elements[id].type==mass_real)
						value = data[z][y][x].toDouble();
					if (l_elements[id].type==mass_bool)
						value = data[z][y][x].toBool();
					if (l_elements[id].type==mass_charect)
						value = data[z][y][x].toChar();
					if (l_elements[id].type==mass_string)
						value = data[z][y][x].toString();
					int index = dataIndex(r, x_base+x, y_base+y, z_base+z);
					l_elements[id].data[index] = value;
				}
			}
		}
	}
	return 0;
}

bool SymbolTable::isConstant(int id) const
{
    int realId = id;
    if (b_mappedIdMode) {
        Q_ASSERT(m_ids.contains(id));
        realId = m_ids[id];
    }
    return l_elements[realId].modifiers.testFlag(ConstModifier);
}
