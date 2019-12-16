#ifndef TEST_H
#define TEST_H

#include <QtCore>
#include <QtGui>

#include "view25d.h"

class Test: public QWidget
{
    Q_OBJECT;
public:
    QTextEdit *editor;
    View25D *view;

public slots:
    inline void btnEvalClicked() {
        editor->append("<font color='blue'>"+view->evaluateScript(editor->document()->toPlainText()).toString()+"</font>");
    }

};

#endif // TEST_H
