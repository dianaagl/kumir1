#include <QtCore>
#include <QtGui>

#include "view25d.h"
#include "test.h"

#include "protomodule.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Test* test = new Test();
    test->setLayout(new QVBoxLayout);
    View25D *view = new View25D(test,
                                QApplication::applicationDirPath()+
                                "/Addons/protoModule/resources/");
    test->view = view;
    test->layout()->addWidget(view);
    view->setMinimumSize(800, 600);
    view->setProtoScene("planet");
    view->addProtoActor("r2d2", "robot");

    const QString fileName = QApplication::applicationDirPath()+"/Addons/protoModule/resources/scripts/test.js";
    QFile f(fileName);
    Q_ASSERT ( f.open(QIODevice::ReadOnly|QIODevice::Text));
    const QString program = QString::fromUtf8(f.readAll());
    f.close();
    view->evaluateScript(program, fileName);
//    view->evaluateScript("InitScene();");


    QTextEdit *editor = new QTextEdit(test);
    test->editor = editor;
    test->layout()->addWidget(editor);

    QPushButton *btn = new QPushButton(test);
    btn->setText("Evaluate!");
    test->layout()->addWidget(btn);


    QObject::connect( btn, SIGNAL(clicked()), test, SLOT(btnEvalClicked()));

    test->show();

    new ProtoModule();

    return app.exec();
}
